#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jlog.h"
#include "CuTest/CuTest.h"

static inline void write_timestamp(const struct jlog_writer_output *output) {
	char timestamp[64];
	time_t now = time(NULL);
	struct tm tm;
	switch (output->timezone) {
	case JLOG_TIMEZONE_UTC:   gmtime_r(&now, &tm); break;
	case JLOG_TIMEZONE_LOCAL: localtime_r(&now, &tm); break;
	default:
		assert("unknown timezone" == NULL);
		break;
	}
	strftime(timestamp, sizeof(timestamp), output->timestamp_format, &tm);

	fprintf(output->fp, "%s", timestamp);
}

void vjlogprintf(const struct jlogger *logger, enum jlog_tag tag, const char *prefix, const char *filename, size_t linenumber, const char *fmt, ...) {
	va_list original_args;
	va_start(original_args, fmt);
	if (logger->writer == NULL) {
		return;
	}
	for (int i = 0; i < logger->writer->noutput; i++) {
		const struct jlog_writer_output *output = &logger->writer->outputs[i];
		if ((output->mask & tag) == 0 || output->fp == NULL) {
			continue;
		}

		for (enum jlog_field field = 1; field < JLOG_FIELDS_ALL; field <<= 1) {
			if ((output->field_mask & field) == 0) {
				continue;
			}
			if ((output->field_mask & (field-1))) { // any previous fields
				fprintf(output->fp, "%s", output->separator);
			}

			switch(field) {
			case JLOG_FIELD_CONTEXT:   fprintf(output->fp, "%s", logger->context!=NULL?logger->context:""); break;
			case JLOG_FIELD_FILENAME:  fprintf(output->fp, "%s", filename); break;
			case JLOG_FIELD_FILEPOS:   fprintf(output->fp, "%lu", linenumber); break;
			case JLOG_FIELD_PREFIX:    fprintf(output->fp, "%s", prefix!=NULL?prefix:""); break;
			case JLOG_FIELD_TAG:       fprintf(output->fp, "0x%x", tag); break;
			case JLOG_FIELD_THREAD:    fprintf(output->fp, "0x%lx", pthread_self()); break;
			case JLOG_FIELD_TICKS:     fprintf(output->fp, "%lu", clock()); break;
			case JLOG_FIELD_TIMESTAMP: write_timestamp(output); break;

			case JLOG_FIELD_MESSAGE: {
				va_list args;
				va_copy(args, original_args);
				vfprintf(output->fp, fmt, args);
				break;
			}

			default:
				assert("unknown field" == NULL);
				break;
			}
		}
		fprintf(output->fp, "\n");
		fflush(output->fp);
	}
	va_end(original_args);
}

void TestJlogNoSegfaultOnInitilizedLoggerWithoutWriters(CuTest *tc) {
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, NULL);
	jlog(&jlogger, TWARN, NULL, "Where do I get printed?");
}

void TestJlogNoSegfaultOnInitializedStructure(CuTest *tc) {
	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, JLOG_FIELDS_ALL);
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);
	jlog(&jlogger, TWARN, NULL, "Where do I get printed?");
}
void TestJlogMessageAndPrefixExists(CuTest *tc) {
#define MSG "Some message"
#define PREFIX "PREFIX"

	FILE *fp = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, JLOG_FIELDS_ALL);
	jwriter.outputs[0].fp = fp;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

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

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, JLOG_FIELDS_ALL);
	jwriter.outputs[0].fp = everything;
	jwriter.outputs[1].fp = warnings;
	jwriter.outputs[1].mask = JLOG_MASK_WARN;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

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

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, JLOG_FIELDS_ALL);
	jwriter.outputs[0].fp = normal;
	jwriter.outputs[1].fp = without_tags;
	jwriter.outputs[1].field_mask ^= JLOG_FIELD_TAG;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

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

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, JLOG_FIELDS_ALL);
	jwriter.outputs[0].fp = utc;
	jwriter.outputs[0].timestamp_format = "%H";
	jwriter.outputs[1].fp = local;
	jwriter.outputs[1].timestamp_format = "%H";
	jwriter.outputs[1].timezone = JLOG_TIMEZONE_LOCAL;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);


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

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_DEBUG, JLOG_FIELDS_ALL);
	jwriter.outputs[0].fp = predefined;
	jwriter.outputs[1].fp = custom;
	jwriter.outputs[1].mask |= CUSTOM_TAG;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

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
