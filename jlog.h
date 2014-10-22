#ifndef JLOG_H
#define JLOG_H

#include <stdio.h>
#include <limits.h>

#ifndef JLOG_DEFAULT_TIMESTAMP_FORMAT
#define JLOG_DEFAULT_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S %z"
#endif /*JLOG_DEFAULT_TIMESTAMP*/
#ifndef JLOG_DEFAULT_SEPARATOR
#define JLOG_DEFAULT_SEPARATOR "\t"
#endif /*JLOG_DEFAULT_SEPARATOR*/

enum jlog_tag {
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

enum jlog_field {
	JLOG_FIELD_TIMESTAMP = 1<<0,
	JLOG_FIELD_TAG =       1<<1,
	JLOG_FIELD_PREFIX =    1<<2,
	JLOG_FIELD_MESSAGE =   1<<3,
	JLOG_FIELD_ALL =      (1<<4)-1,
};

enum jlog_timezone {
	JLOG_TIMEZONE_UTC,
	JLOG_TIMEZONE_LOCAL,
};


struct jlogger_writer {
	FILE *fp;
	unsigned long mask;

	enum jlog_timezone timezone;
	enum jlog_field field_mask;

	const char *timestamp_format;
	const char *separator;
};
struct jlogger {
	size_t nwriter;
	struct jlogger_writer writers[];
};
#define JLOG_STATIC_INIT(nwriter, mask, timezone, field_mask) {\
	nwriter,\
	{\
		[0 ... nwriter-1] = {\
			NULL,\
			mask,\
			timezone,\
			field_mask,\
			JLOG_DEFAULT_TIMESTAMP_FORMAT,\
			JLOG_DEFAULT_SEPARATOR,\
		}\
	}\
}

void jlog(const struct jlogger *logger, enum jlog_tag tag, const char *prefix, const char *fmt, ...);

#endif /*JLOG_H*/
