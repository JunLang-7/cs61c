#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "simd.h"

long long int sum(int vals[NUM_ELEMS]) {
	clock_t start = clock();

	long long int sum = 0;
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		for(unsigned int i = 0; i < NUM_ELEMS; i++) {
			if(vals[i] >= 128) {
				sum += vals[i];
			}
		}
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	long long int sum = 0;

	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
			if(vals[i] >= 128) sum += vals[i];
			if(vals[i + 1] >= 128) sum += vals[i + 1];
			if(vals[i + 2] >= 128) sum += vals[i + 2];
			if(vals[i + 3] >= 128) sum += vals[i + 3];
		}

		//This is what we call the TAIL CASE
		//For when NUM_ELEMS isn't a multiple of 4
		//NONTRIVIAL FACT: NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
		for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				sum += vals[i];
			}
		}
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	__m128i _127 = _mm_set1_epi32(127);		// This is a vector with 127s in it... Why might you need this?
	long long int result = 0;				   // This is where you should put your final result!
	/* DO NOT DO NOT DO NOT DO NOT WRITE ANYTHING ABOVE THIS LINE. */
	unsigned int res[4] = {0, 0, 0, 0};
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		/* YOUR CODE GOES HERE */
		__m128i _sum = _mm_setzero_si128();
		for (unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
			__m128i _vals = _mm_loadu_si128((__m128i *)(vals + i));
			__m128i _mask = _mm_cmpgt_epi32(_vals, _127);
			__m128i _tmp = _mm_and_si128(_vals, _mask);
			_sum = _mm_add_epi32(_sum, _tmp);
		}
		_mm_storeu_si128((__m128i *)res, _sum);
		/* You'll need a tail case. */
		for (unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				res[0] += vals[i];
			}
		}
		result += res[0] + res[1] + res[2] + res[3];
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	__m128i _127 = _mm_set1_epi32(127);
	long long int result = 0;
	unsigned int res[4] = {0, 0, 0, 0};
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		/* COPY AND PASTE YOUR sum_simd() HERE */
		/* MODIFY IT BY UNROLLING IT */
		__m128i _sum = _mm_setzero_si128();
		for (unsigned int i = 0; i < NUM_ELEMS / 16 * 16; i += 4*4) {
			__m128i _vals = _mm_loadu_si128((__m128i *)(vals + i));
			__m128i _mask = _mm_cmpgt_epi32(_vals, _127);
			__m128i _tmp = _mm_and_si128(_vals, _mask);
			_sum = _mm_add_epi32(_sum, _tmp);

			_vals = _mm_loadu_si128((__m128i *)(vals + i + 4));
			_mask = _mm_cmpgt_epi32(_vals, _127);
			_tmp = _mm_and_si128(_vals, _mask);
			_sum = _mm_add_epi32(_sum, _tmp);

			_vals = _mm_loadu_si128((__m128i *)(vals + i + 8));
			_mask = _mm_cmpgt_epi32(_vals, _127);
			_tmp = _mm_and_si128(_vals, _mask);
			_sum = _mm_add_epi32(_sum, _tmp);

			_vals = _mm_loadu_si128((__m128i *)(vals + i + 12));
			_mask = _mm_cmpgt_epi32(_vals, _127);
			_tmp = _mm_and_si128(_vals, _mask);
			_sum = _mm_add_epi32(_sum, _tmp);
		}
		_mm_storeu_si128((__m128i *)res, _sum);
		/* You'll need 1 or maybe 2 tail cases here. */
		for (unsigned int i = NUM_ELEMS / 16 * 16; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				res[0] += vals[i];
			}
		}
		result += res[0] + res[1] + res[2] + res[3];
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return result;
}