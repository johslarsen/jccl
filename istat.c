#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "CuTest/CuTest.h"
#include "istat.h"

static double population[] = { 3.2, -5.3, 6.1, -2.3, 4.6, 1.0, 2.4 }; // some random numbers
static size_t npopulation = sizeof(population) / sizeof(*population);

static int double_compar(const void *a, const void *b) {
	return *((double *)a) - *((double *)b);
}
void TestIstatMinMax(CuTest *tc) {
	double min = DBL_MAX, max = DBL_MIN;
	for (size_t i = 0; i < npopulation; i++) {
		min = istat_min(min, population[i]);
		max = istat_max(max, population[i]);
	}

	qsort(population, npopulation, sizeof(*population), double_compar);

	CuAssertDblEquals(tc, population[0], min, 0.0);
	CuAssertDblEquals(tc, population[npopulation-1], max, 0.0);
}

void TestIstat(CuTest *tc) {
	struct istat istat = ISTAT_STATIC_INIT();

	double sum = 0.0;
	for (size_t i = 0; i < npopulation; i++) {
		istat_add(&istat, population[i]);
		sum += population[i];
	}

	double mean = sum / npopulation;
	double sum_square_delta_mean = 0.0;
	for (size_t i = 0; i < npopulation; i++) {
		sum_square_delta_mean += pow(population[i]-mean, 2);
	}

	CuAssertIntEquals(tc, npopulation, istat.n);
	CuAssertDblEquals(tc, sum, istat.sum, 0.01);
	CuAssertDblEquals(tc, mean, istat.mean, 0.01);
	CuAssertDblEquals(tc, sum_square_delta_mean, istat.sum_square_delta_mean, 0.01);
}

void TestIstatVarianceAndStdDevErr(CuTest *tc) {
	struct istat istat = ISTAT_STATIC_INIT();
	istat.n = 42;
	istat.sum_square_delta_mean = 13.37;

	// NOTE: preceding initialization are irrelevant, but we would like them to stand out if they have any effect
	istat.mean = DBL_MAX;
	istat.sum = DBL_MIN;

	double uncorrected_variance = istat.sum_square_delta_mean / istat.n;
	double corrected_variance = istat.sum_square_delta_mean / (istat.n-1);

	double uncorrected_stddev = sqrt(uncorrected_variance);
	double corrected_stddev = sqrt(corrected_variance);

	double uncorrected_stderr = uncorrected_stddev / sqrt(istat.n);
	double corrected_stderr = corrected_stddev / sqrt(istat.n);

	CuAssertDblEquals(tc, uncorrected_variance, istat_variance(&istat, UNCORRECTED), 0.01);
	CuAssertDblEquals(tc, corrected_variance, istat_variance(&istat, BESSELS_CORRECTED), 0.01);
	CuAssertDblEquals(tc, uncorrected_stddev, istat_standard_deviation(&istat, UNCORRECTED), 0.01);
	CuAssertDblEquals(tc, corrected_stddev, istat_standard_deviation(&istat, BESSELS_CORRECTED), 0.01);
	CuAssertDblEquals(tc, uncorrected_stderr, istat_standard_error(&istat, UNCORRECTED), 0.01);
	CuAssertDblEquals(tc, corrected_stderr, istat_standard_error(&istat, BESSELS_CORRECTED), 0.01);
}
