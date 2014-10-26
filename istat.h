#ifndef ISTAT_H
#define ISTAT_H

#include <stdbool.h>
#include <stddef.h>
#include <math.h>

static inline double istat_min(double min, double x) {
	return x < min ? x : min;
}
static inline double istat_max(double max, double x) {
	return x > max ? x : max;
}

struct istat {
	size_t n;
	double mean;
	double sum_square_delta_mean;
	double sum;
};
#define ISTAT_STATIC_INIT() { 0 }

static inline size_t istat_add(struct istat *istat, double x) {
	istat->n++;

	istat->sum += x;

	double delta = x - istat->mean;
	istat->mean += delta / istat->n;
	istat->sum_square_delta_mean += delta * (x-istat->mean);

	return istat->n;
}

enum istat_bias_correction {
	UNCORRECTED = 0,
	BESSELS_CORRECTED = 1,
};
static inline double istat_variance(struct istat *istat, enum istat_bias_correction correction) {
	return istat->sum_square_delta_mean / (istat->n-correction) ;
}
static inline double istat_standard_deviation(struct istat *istat, enum istat_bias_correction correction) {
	return sqrt(istat_variance(istat, correction));
}
static inline double istat_standard_error(struct istat *istat, enum istat_bias_correction correction) {
	return istat_standard_deviation(istat, correction) / sqrt(istat->n);
}

#endif /*ISTAT_H*/
