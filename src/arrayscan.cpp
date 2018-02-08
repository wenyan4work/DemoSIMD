#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Timer.hpp"

void test_simple(int size) {
    double *fromPtr = new double[size];
    double *toPtr = new double[size];
    Timer timer;
    timer.start();
    for (int i = 0; i < size; i++) {
        toPtr[i] = fromPtr[i] * 10 + 5;
    }
    timer.stop("simple scan complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) / timer.getTime());
}

void test_ompsimd(int size) {
    double *fromPtr = new double[size];
    double *toPtr = new double[size];
    Timer timer;
    timer.start();
#pragma omp simd
    for (int i = 0; i < size; i++) {
        toPtr[i] = fromPtr[i] * 10 + 5;
    }
    timer.stop("omp simd scan complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) / timer.getTime());
}

int main() {
    for (int i = 1; i < 8; i++) {
        printf("-----------------\n");
        test_simple(pow(4, i + 5));
        test_ompsimd(pow(4, i + 5));
    }

    return 0;
}