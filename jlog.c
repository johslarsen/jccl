#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "jlog.h"
#include "CuTest/CuTest.h"

static inline void write_timestamp(const struct jlog_writer_output *output, const char *format) {
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
	strftime(timestamp, sizeof(timestamp), format, &tm);
#pragma GCC diagnostic pop

	fprintf(output->fp, "%s", timestamp);
}
static inline const char *extract_convert_parameter(char *buf, size_t nbuf, const char *format) {
	const char *sof = format+2; // format char + '{'
	const char *eof = strchr(sof, '}');
	assert(eof != NULL);

	size_t nf = eof-sof;
	assert(nf < nbuf);
	strncpy(buf, sof, nf);
	buf[nf] = '\0';

	return eof;
}

void vjlogprintf(const struct jlogger *logger, enum jlog_tag tag, const char *function, const char *filename, size_t linenumber, const char *fmt, ...) {
	if (logger->writer == NULL) {
		return;
	}

	va_list original_args;
	va_start(original_args, fmt);

	pthread_mutex_lock(&logger->writer->mutex);

	for (size_t i = 0; i < logger->writer->noutput; i++) {
		const struct jlog_writer_output *output = &logger->writer->outputs[i];
		if ((output->mask & tag) == 0 || output->fp == NULL) {
			continue;
		}

		bool convert = false;
		for (const char *p = output->format; *p != '\0'; p++) {
			char c = *p;
			if (convert) {
				convert = false;

				switch(c) {
					case '%': fputc('%', output->fp); break;
					case 'c': fprintf(output->fp, "%s", logger->category!=NULL?logger->category:""); break;
					case 'd':
						if (*(p+1) == '{') {
							char timeformat[64];
							p = extract_convert_parameter(timeformat, sizeof(timeformat), p);
							write_timestamp(output, timeformat);
						} else {
							write_timestamp(output, JLOG_DEFAULT_TIMESTAMP_FORMAT);
						}
						break;
					case 'F': fprintf(output->fp, "%s", filename); break;
					case 'L': fprintf(output->fp, "%lu", linenumber); break;
					case 'l': fprintf(output->fp, "%s(%s:%lu)", function, filename, linenumber); break;
					case 'm': {
						va_list args;
						va_copy(args, original_args);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
						vfprintf(output->fp, fmt, args);
#pragma GCC diagnostic pop
						va_end(args);
						break;
					}
					case 'M': fprintf(output->fp, "%s", function); break;
					case 'n': fputc('\n', output->fp); break;
					case 'p': fprintf(output->fp, "0x%x", tag); break;
					case 'r': fprintf(output->fp, "%lu", clock() / (CLOCKS_PER_SEC/1000)); break;
					case 't': fprintf(output->fp, "0x%lx", pthread_self()); break;

					// conversion characters defined in log4j, but not implemented
					case 'C': // class
					case 'x': // nested diagnostic context
					case 'X': // mapped diagnostic context, user defined context fields
						break;

					default:
						assert("unknown logger format conversion" == NULL);
						break;
				}
			} else if (c == '%') {
				convert = true;
			} else {
				putc(c, output->fp);
			}
		}

		fputc('\n', output->fp);
		fflush(output->fp);
	}

	pthread_mutex_unlock(&logger->writer->mutex);

	va_end(original_args);
}

void TestJlogNoSegfaultOnInitilizedLoggerWithoutWriters(CuTest *tc) {
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, NULL);
	jlog(&jlogger, TWARN, "Where do I get printed?");
	CuAssertPtrNotNull(tc, "passed by getting here");
}

void TestJlogNoSegfaultOnInitializedStructure(CuTest *tc) {
	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, JLOG_FORMAT_EXTENSIVE);
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);
	jlog(&jlogger, TWARN, "Where do I get printed?");
	CuAssertPtrNotNull(tc, "passed by getting here");
}
void TestJlogMessageAndCategoryExistInDefaultFormats(CuTest *tc) {
#define MSG "Some message"
#define CATEGORY "A category"

	FILE *extensive = tmpfile();
	FILE *minimal = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, JLOG_FORMAT_EXTENSIVE);
	jwriter.outputs[0].fp = extensive;
	jwriter.outputs[1].fp = minimal;
	jwriter.outputs[1].format = JLOG_FORMAT_MINIMAL;

	static struct jlogger jlogger = JLOGGER_STATIC_INIT(CATEGORY, JLOG_MASK_EVERYTHING, &jwriter);

	jlog(&jlogger, TINFO, MSG);
	rewind(extensive);
	rewind(minimal);

	char buf[1024];
	size_t nread;
	nread = fread(buf, 1, sizeof(buf), extensive);
	buf[nread] = '\0';
	CuAssertPtrNotNull(tc, strstr(buf, MSG));
	CuAssertPtrNotNull(tc, strstr(buf, CATEGORY));
	nread = fread(buf, 1, sizeof(buf), minimal);
	buf[nread] = '\0';
	CuAssertPtrNotNull(tc, strstr(buf, MSG));
	CuAssertPtrNotNull(tc, strstr(buf, CATEGORY));
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

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, JLOG_FORMAT_EXTENSIVE);
	jwriter.outputs[0].fp = everything;
	jwriter.outputs[1].fp = warnings;
	jwriter.outputs[1].mask = JLOG_MASK_WARN;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

	jlog(&jlogger, TINFO, "A message with %d printf arguments", 1);
	jlog(&jlogger, TWARN, "A warning");
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

void TestJlogTimeformatAndTimezone(CuTest *tc) {
	FILE *utc = tmpfile();
	FILE *local = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_EVERYTHING, "%d{%H}");
	jwriter.outputs[0].fp = utc;
	jwriter.outputs[1].fp = local;
	jwriter.outputs[1].timezone = JLOG_TIMEZONE_LOCAL;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);


	time_t now = time(NULL);
	jlog(&jlogger, TINFO, "Some message");
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

void *logger_thread(void *struct_jlogger) {
	struct jlogger *jlogger = (struct jlogger *)struct_jlogger;
	jlog(jlogger, TINFO, "Logging from a pthread child");
	return NULL;
}
void TestJlogThreaded(CuTest *tc) {
	FILE *fp = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, "%t");
	jwriter.outputs[0].fp = fp;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

	pthread_t pthread;
	pthread_create(&pthread, NULL, logger_thread, &jlogger);

	jlog(&jlogger, TINFO, "Logging from parent thread");

	pthread_join(pthread, NULL);

	rewind(fp);

	unsigned long tid1, tid2;
	fscanf(fp, "0x%lx\n", &tid1);
	fscanf(fp, "0x%lx\n", &tid2);

	CuAssertTrue(tc, tid1 != tid2);
	CuAssertTrue(tc, tid1 == pthread_self() || tid2 == pthread_self());
}

void TestJlogCustomTags(CuTest *tc) {
	enum {
		CUSTOM_TAG = 1<<31,
	};
	FILE *predefined = tmpfile();
	FILE *custom = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(2, JLOG_MASK_DEBUG, JLOG_FORMAT_EXTENSIVE);
	jwriter.outputs[0].fp = predefined;
	jwriter.outputs[1].fp = custom;
	jwriter.outputs[1].mask |= CUSTOM_TAG;
	static struct jlogger jlogger = JLOGGER_STATIC_INIT("foo", JLOG_MASK_EVERYTHING, &jwriter);

	jlog(&jlogger, CUSTOM_TAG, "Some message");
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

void TestJlogCategory(CuTest *tc) {
	FILE *fp = tmpfile();

	static struct jlog_writer jwriter = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, "%c");
	jwriter.outputs[0].fp = fp;

	static struct jlogger jlogger_everything = JLOGGER_STATIC_INIT("everything", JLOG_MASK_EVERYTHING, &jwriter);
	static struct jlogger jlogger_errors = JLOGGER_STATIC_INIT("errors", JLOG_MASK_ERROR, &jwriter);

	jlog(&jlogger_everything, TERROR, "An error");
	jlog(&jlogger_everything, TINFO, "Some information");
	jlog(&jlogger_errors, TERROR, "An error");
	jlog(&jlogger_errors, TINFO, "Some information"); // should not be logged

	rewind(fp);

	char buf[1024];
	size_t nread;
	nread = fread(buf, 1, sizeof(buf), fp);
	buf[nread] = '\0';
	CuAssertStrEquals(tc, "everything\neverything\nerrors\n", buf);
}
