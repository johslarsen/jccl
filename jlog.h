#ifndef JLOG_H
#define JLOG_H

#include <limits.h>
#include <pthread.h>
#include <stdio.h>

/* A simple logger
 *
 * format conversion patterns (mimicking those from log4j):
 *
 * %%      => '%'
 * %c      => category
 * %d{...} => data and time in default format or strftime format specified by ...
 * %F      => filename
 * %L      => line number
 * %l      => location := %M(%F:%L)
 * %m      => message
 * %M      => method name
 * %n      => '\n'
 * %p      => priority, aka. tag
 * %r      => milliseconds since start of program, integer
 * %t      => thread ID
 */

enum jlog_tag { // mimicking syslog
	TEMERGENCY = 1<<0, //  system is unusable
	TALERT     = 1<<1, //  action must be taken immediately
	TCRITICAL  = 1<<2, //  critical conditions
	TERROR     = 1<<3, //  error conditions
	TWARN      = 1<<4, //  warning conditions
	TNOTICE    = 1<<5, //  normal but significant condition
	TINFO      = 1<<6, //  informational messages
	TDEBUG     = 1<<7, //  debug-level messages
};
enum jlog_mask {
	JLOG_MASK_NOTHING = 0,
	JLOG_MASK_EVERYTHING = ULONG_MAX,

	JLOG_MASK_EMERGENCY = (TEMERGENCY << 1) - 1,
	JLOG_MASK_ALERT     = (TALERT << 1) - 1,
	JLOG_MASK_CRITICAL  = (TCRITICAL << 1) - 1,
	JLOG_MASK_ERROR     = (TERROR << 1) - 1,
	JLOG_MASK_WARN      = (TWARN << 1) - 1,
	JLOG_MASK_NOTICE    = (TNOTICE << 1) - 1,
	JLOG_MASK_INFO      = (TINFO << 1) - 1,
	JLOG_MASK_DEBUG     = (TDEBUG << 1) - 1,
};

#ifndef JLOG_DEFAULT_TIMESTAMP_FORMAT
#define JLOG_DEFAULT_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S %z"
#endif /*JLOG_DEFAULT_TIMESTAMP_FORMAT*/

#ifndef JLOG_FORMAT_MINIMAL
#define JLOG_FORMAT_MINIMAL "%d\t%c\t%m"
#endif /*JLOG_FORMAT_MINIMAL*/
#ifndef JLOG_FORMAT_EXTENSIVE
#define JLOG_FORMAT_EXTENSIVE "%d\t%r\t%t\t%l\t%p\t%c\t%m"
#endif /*JLOG_FORMAT_EXTENSIVE*/

enum jlog_timezone {
	JLOG_TIMEZONE_UTC,
	JLOG_TIMEZONE_LOCAL,
};

struct jlog_writer_output {
	FILE *fp;
	unsigned long mask;

	const char *format;
	enum jlog_timezone timezone;
};
struct jlog_writer {
	pthread_mutex_t mutex;
	size_t noutput;
	struct jlog_writer_output outputs[];
};
#define JLOG_WRITER_STATIC_INIT(nwriter, mask, format) {\
	PTHREAD_MUTEX_INITIALIZER,\
	nwriter,\
	{\
		[0 ... nwriter-1] = {\
			NULL,\
			mask,\
			format,\
			JLOG_TIMEZONE_UTC,\
		}\
	}\
}

struct jlogger {
	const char *category;
	unsigned long mask;

	struct jlog_writer *writer;
};
#define JLOGGER_STATIC_INIT(category, mask, writer) {\
	category,\
	mask,\
	writer,\
}

void vjlogprintf(const struct jlogger *logger, enum jlog_tag tag, const char *function, const char *filename, size_t linenumber, const char *fmt, ...);

#ifdef JLOG_DISABLE
#define jlog(logger, tag, ...) (void)0;
#else
#define jlog(logger, tag, ...) do {\
	if (((logger)->mask & tag)) {\
		vjlogprintf(logger, tag, __func__, __FILE__, __LINE__, __VA_ARGS__);\
	}\
} while(0)
#endif /*JLOG_DISABLE*/

#endif /*JLOG_H*/
