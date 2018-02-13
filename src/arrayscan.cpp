#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#define REPEAT (100)

void test_simple(int size) {
    double *__restrict__ fromPtr = new double[size];
    double *__restrict__ toPtr = new double[size];
    for (int i = 0; i < size; i++) {
        fromPtr[i] = i;
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
}

void test_ompsimd(int size) {
    AlignedMemory<double, 32> frommem(size);
    AlignedMemory<double, 32> tomem(size);
    double *__restrict__ fromPtr = frommem.alignedPtr;
    double *__restrict__ toPtr = tomem.alignedPtr;

    for (int i = 0; i < size; i++) {
        fromPtr[i] = i;
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

int main() {
    for (int i = 1; i < 8; i++) {
        printf("-----------------\n");
        test_simple(pow(4, i + 5));
        test_ompsimd(pow(4, i + 5));
    }

    return 0;
}