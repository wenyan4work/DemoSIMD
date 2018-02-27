#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#include "immintrin.h"

#define REPEAT (100)

void test_simple(int size) {
    double *fromPtr = new double[size];
    double *toPtr = new double[size];
    for (int i = 0; i < size; i++) {
        fromPtr[i] = i;
        toPtr[i] = 0;
    }

    Timer timer;
    timer.start();
    for (int j = 0; j < REPEAT; j++) {
        for (int i = 0; i < size; i++) {
            toPtr[i] += fromPtr[i] * 10 + 5;
        }
    }
    timer.stop("simple scan complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    printf("%f\n", toPtr[size - 1]);
    delete[] fromPtr;
    delete[] toPtr;
}

void test_ompsimd(int size) {
    AlignedMemory<double, 32> frommem(size);
    AlignedMemory<double, 32> tomem(size);
    double *fromPtr = frommem.alignedPtr;
    double *toPtr = tomem.alignedPtr;

    for (int i = 0; i < size; i++) {
        fromPtr[i] = i;
        toPtr[i] = 0;
    }

    Timer timer;
    timer.start();
    for (int j = 0; j < REPEAT; j++) {
#pragma omp simd aligned(fromPtr, toPtr : 32) safelen(8)
        for (int i = 0; i < size; i++) {
            toPtr[i] += fromPtr[i] * 10 + 5;
        }
    }
    timer.stop("omp simd scan complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    printf("%f\n", toPtr[size - 1]);
}

void test_manual(int size) {
    AlignedMemory<double, 32> frommem(size);
    AlignedMemory<double, 32> tomem(size);
    double *fromPtr = frommem.alignedPtr;
    double *toPtr = tomem.alignedPtr;

    for (int i = 0; i < size; i++) {
        fromPtr[i] = i;
        toPtr[i] = 0;
    }
    const __m256d five = _mm256_set1_pd(5.0);
    const __m256d ten = _mm256_set1_pd(10.0);

    Timer timer;
    timer.start();

    for (int j = 0; j < REPEAT; j++) {
        for (int i = 0; i < size; i += 4) {
            _mm_prefetch((char *)fromPtr + i + 4, _MM_HINT_T0); // no performance gain
            _mm_prefetch((char *)toPtr + i + 4, _MM_HINT_T0);

            __m256d to = _mm256_load_pd(toPtr + i);
            __m256d from = _mm256_load_pd(fromPtr + i);

            to = _mm256_add_pd(to, _mm256_fmadd_pd(from, ten, five));

            _mm256_store_pd(toPtr + i, to);
        }
    }

    timer.stop("manual simd scan complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    printf("%f\n", toPtr[size - 1]);
}

int main() {
    // warming up
    test_manual(1000000);

    for (int i = 1; i < 8; i++) {
        printf("-----------------\n");
        test_simple(pow(4, i + 5));
        test_ompsimd(pow(4, i + 5));
        test_manual(pow(4, i + 5));
    }

    return 0;
}