#ifndef TIMER_H
#define TIMER_H

#include <time.h>

#ifndef TIMED_BLOCK_CLOCK
#define TIMED_BLOCK_CLOCK CLOCK_MONOTONIC_RAW
#endif /*TIMED_BLOCK_CLOCK*/

#ifndef TIMED_BLOCK_ACTION
#define TIMED_BLOCK_ACTION(description, mean_duration) printf("%s %g\n", (description), (mean_duration))
#endif /*TIMED_BLOCK_ACTION*/

#define TIMED_BLOCK(niteration, description) for (\
			struct timespec /* INIT: (i.e. before first iteration) */\
				_tb_start, _tb_stop\
				, _tb_sum={0}\
				, _tb_i={0}/* comma expression is limited to a single type, so exploit struct timespec.tv_sec (time_t) as integer */\
				,_tb_niteration = {(niteration), 0} /* expand niteration once */\
			; /* CONDITION: (i.e. before first and after every iteration)*/\
				(\
					clock_gettime(TIMED_BLOCK_CLOCK, &_tb_start) /* start current timer */\
					, _tb_i.tv_sec++ < _tb_niteration.tv_sec /* loop increment and condition */\
				) || ( /* when CONDITION fails (i.e. after last loop), do action: */\
					TIMED_BLOCK_ACTION((description), (_tb_sum.tv_sec + _tb_sum.tv_nsec/1e9)/(_tb_niteration.tv_sec))\
					, 0 /* then ignore its result and break the loop */\
				)\
			; /* INCREMENT: (i.e. after each iteration)*/\
				clock_gettime(TIMED_BLOCK_CLOCK, &_tb_stop) /* stop current timer */\
				, _tb_sum.tv_sec += _tb_stop.tv_sec - _tb_start.tv_sec /* aggregate seconds */\
				, _tb_sum.tv_nsec += _tb_stop.tv_nsec - _tb_start.tv_nsec /* aggregate nanoseconds */\
		)


#endif /*TIMER_H*/
