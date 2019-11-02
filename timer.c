#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CuTest/CuTest.h"
#include "jlog.h"

static struct jlog_writer jw = JLOG_WRITER_STATIC_INIT(1, JLOG_MASK_EVERYTHING, "%m");
static struct jlogger jl = JLOGGER_STATIC_INIT("TIMED_BLOCK", JLOG_MASK_EVERYTHING, &jw);
#define TIMED_BLOCK_ACTION(description, mean_duration) vjlogprintf(&jl, TINFO, __func__, __FILE__, __LINE__, "%s %.2f", (description), (mean_duration))
#include "timer.h"

void TestTIMED_BLOCK(CuTest *tc) {
#define DESCRIPTION_OUTER "wrapping"
#define DESCRIPTION_INNER "usleep(10000)"
	jw.outputs[0].fp = tmpfile();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
	TIMED_BLOCK(2, DESCRIPTION_OUTER) { // nested TIMED_BLOCKs works, but makes little sense with outer niteration != 1
		TIMED_BLOCK(5, DESCRIPTION_INNER) {
			usleep(10000);
		}
	}
#pragma GCC diagnostic pop

	rewind(jw.outputs[0].fp);

	char buf[1024];
	fread(buf, 1, sizeof(buf), jw.outputs[0].fp);
	CuAssertStrEquals(tc, DESCRIPTION_INNER" 0.01\n"DESCRIPTION_INNER" 0.01\n"DESCRIPTION_OUTER" 0.05\n", buf);
}
