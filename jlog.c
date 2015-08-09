#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jlog.h"
#include "CuTest/CuTest.h"

static inline void write_timestamp(const struct jlogger_writer *writer) {
	char timestamp[64];
	time_t now = time(NULL);
	struct tm tm;
	switch (writer->timezone) {
	case JLOG_TIMEZONE_UTC:   gmtime_r(&now, &tm); break;
	case JLOG_TIMEZONE_LOCAL: localtime_r(&now, &tm); break;
	default:
		assert("unknown timezone" == NULL);
		break;
	}
	strftime(timestamp, sizeof(timestamp), writer->timestamp_format, &tm);

	fprintf(writer->fp, "%s", timestamp);
}

void vjlogprintf(const struct jlogger *logger, enum jlog_tag tag, const char *prefix, const char *filename, size_t linenumber, const char *fmt, ...) {
	va_list original_args;
	va_start(original_args, fmt);
	for (int i = 0; i < logger->nwriter; i++) {
		const struct jlogger_writer *writer = &logger->writers[i];
		if (writer->fp == NULL || (writer->mask & tag) == 0) {
			continue;
		}

		for (enum jlog_field field = 1; field < JLOG_FIELDS_ALL; field <<= 1) {
			if ((writer->field_mask & field) == 0) {
				continue;
			}
			if ((writer->field_mask & (field-1))) { // any previous fields
				fprintf(writer->fp, "%s", writer->separator);
			}

			switch(field) {
			case JLOG_FIELD_CONTEXT:   fprintf(writer->fp, "%s", logger->context!=NULL?logger->context:""); break;
			case JLOG_FIELD_FILENAME:  fprintf(writer->fp, "%s", filename); break;
			case JLOG_FIELD_FILEPOS:   fprintf(writer->fp, "%lu", linenumber); break;
			case JLOG_FIELD_PREFIX:    fprintf(writer->fp, "%s", prefix!=NULL?prefix:""); break;
			case JLOG_FIELD_TAG:       fprintf(writer->fp, "0x%x", tag); break;
			case JLOG_FIELD_THREAD:    fprintf(writer->fp, "0x%lx", pthread_self()); break;
			case JLOG_FIELD_TICKS:     fprintf(writer->fp, "%lu", clock()); break;
			case JLOG_FIELD_TIMESTAMP: write_timestamp(writer); break;

			case JLOG_FIELD_MESSAGE: {
				va_list args;
				va_copy(args, original_args);
				vfprintf(writer->fp, fmt, args);
				break;
			}

			default:
				assert("unknown field" == NULL);
				break;
			}
		}
		fprintf(writer->fp, "\n");
		fflush(writer->fp);
	}
	va_end(original_args);
}

void TestJlogNoSegfaultOnInitializedStructure(CuTest *tc) {
	static struct jlogger jlogger = JLOG_STATIC_INIT(NULL, 1, JLOG_MASK_EVERYTHING, JLOG_TIMEZONE_UTC, JLOG_FIELDS_ALL);
	jlog(&jlogger, TWARN, NULL, "Where do I get printed?");
}

void TestJlogMessageAndPrefixExists(CuTest *tc) {
#define MSG "Some message"
#define PREFIX "PREFIX"

	FILE *fp = tmpfile();

	static struct jlogger jlogger = JLOG_STATIC_INIT("foo", 2, JLOG_MASK_EVERYTHING, JLOG_TIMEZONE_UTC, JLOG_FIELDS_ALL);
	jlogger.writers[0].fp = fp;

	jlog(&jlogger, TINFO, PREFIX, MSG);
	rewind(fp);

	char buf[1024];
	fread(buf, 1, sizeof(buf), fp);
	CuAssertPtrNotNull(tc, strstr(buf, PREFIX));
	CuAssertPtrNotNull(tc, strstr(buf, MSG));
}

static size_t strcount(const char *haystack, const char *needle) {
	size_t i;
	for (i = 0; (haystack = strstr(haystack, needle)) != NULL; i++) {
		haystack += strlen(needle);
	}

	return i;
}

void TestJlogMasking(CuTest *tc) {
	FILE *everything = tmpfile();
	FILE *warnings = tmpfile();

	static struct jlogger jlogger = JLOG_STATIC_INIT("foo", 2, JLOG_MASK_EVERYTHING, JLOG_TIMEZONE_UTC, JLOG_FIELDS_ALL);
	jlogger.writers[0].fp = everything;
	jlogger.writers[1].fp = warnings;
	jlogger.writers[1].mask = JLOG_MASK_WARN;

	jlog(&jlogger, TINFO, NULL, "A message with %d printf arguments", 1);
	jlog(&jlogger, TWARN, NULL, "A warning");
	rewind(everything);
	rewind(warnings);

	char buf[1024];
	size_t nread;
	nread = fread(buf, 1, sizeof(buf), everything);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 2, strcount(buf, "\n"));
	nread = fread(buf, 1, sizeof(buf), warnings);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 1, strcount(buf, "\n"));
}

void TestJlogFieldMask(CuTest *tc) {
	FILE *normal = tmpfile();
	FILE *without_tags = tmpfile();

	static struct jlogger jlogger = JLOG_STATIC_INIT("foo", 2, JLOG_MASK_EVERYTHING, JLOG_TIMEZONE_UTC, JLOG_FIELDS_ALL);
	jlogger.writers[0].fp = normal;
	jlogger.writers[1].fp = without_tags;
	jlogger.writers[1].field_mask ^= JLOG_FIELD_TAG;

	jlog(&jlogger, TINFO, NULL, "Some message");
	rewind(normal);
	rewind(without_tags);

	char buf[1024];
	size_t nread;
	nread = fread(buf, 1, sizeof(buf), normal);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 8, strcount(buf, JLOG_DEFAULT_SEPARATOR));
	nread = fread(buf, 1, sizeof(buf), without_tags);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 7, strcount(buf, JLOG_DEFAULT_SEPARATOR));
}

void TestJlogTimeformatAndTimezone(CuTest *tc) {
	FILE *utc = tmpfile();
	FILE *local = tmpfile();

	static struct jlogger jlogger = JLOG_STATIC_INIT("foo", 2, JLOG_MASK_EVERYTHING, JLOG_TIMEZONE_UTC, JLOG_FIELD_TIMESTAMP);
	jlogger.writers[0].fp = utc;
	jlogger.writers[0].timestamp_format = "%H";
	jlogger.writers[1].fp = local;
	jlogger.writers[1].timestamp_format = "%H";
	jlogger.writers[1].timezone = JLOG_TIMEZONE_LOCAL;

	time_t now = time(NULL);
	jlog(&jlogger, TINFO, NULL, "Some message");
	rewind(utc);
	rewind(local);

	char buf[16];
	fread(buf, 1, sizeof(buf), utc);
	int utc_hour = atoi(buf);
	fread(buf, 1, sizeof(buf), local);
	int local_hour = atoi(buf);

	struct tm utctm, localtm;
	gmtime_r(&now, &utctm);
	localtime_r(&now, &localtm);

	CuAssertIntEquals(tc, localtm.tm_hour-utctm.tm_hour, local_hour-utc_hour);
}

void TestJlogCustomTags(CuTest *tc) {
	enum {
		CUSTOM_TAG = 1<<31,
	};
	FILE *predefined = tmpfile();
	FILE *custom = tmpfile();

	static struct jlogger jlogger = JLOG_STATIC_INIT("foo", 2, JLOG_MASK_DEBUG, JLOG_TIMEZONE_UTC, JLOG_FIELDS_ALL);
	jlogger.writers[0].fp = predefined;
	jlogger.writers[1].fp = custom;
	jlogger.writers[1].mask |= CUSTOM_TAG;

	jlog(&jlogger, CUSTOM_TAG, NULL, "Some message");
	rewind(predefined);
	rewind(custom);

	char buf[1024];
	size_t nread;
	nread = fread(buf, 1, sizeof(buf), predefined);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 0, strcount(buf, "\n"));
	nread = fread(buf, 1, sizeof(buf), custom);
	buf[nread] = '\0';
	CuAssertIntEquals(tc, 1, strcount(buf, "\n"));
}
