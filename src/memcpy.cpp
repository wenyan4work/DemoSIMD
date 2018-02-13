#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#define REPEAT (100)

void test_memcpy(int size) {
    double *__restrict__ fromPtr = new double[size];
    double *__restrict__ toPtr = new double[size];
    Timer timer;
    timer.start();
    for (int i = 0; i < REPEAT; i++) {
        memcpy(toPtr, fromPtr, sizeof(double) * size);
    }
    timer.stop("memcpy complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
}

void test_simple(int size) {
    double *__restrict__ fromPtr = new double[size];
    double *__restrict__ toPtr = new double[size];
    Timer timer;
    timer.start();
    for (int j = 0; j < REPEAT; j++) {
        for (int i = 0; i < size; i++) {
            toPtr[i] = fromPtr[i];
        }
    }
    timer.stop("simple copy complete, ");
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
    Timer timer;
    timer.start();
    for (int j = 0; j < REPEAT; j++) {
#pragma omp simd
        for (int i = 0; i < size; i++) {
            toPtr[i] = fromPtr[i];
        }
    }
    timer.stop("omp simd copy complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    printf("%f\n", toPtr[size - 1]);
}

int main() {
    for (int i = 1; i < 8; i++) {
        printf("-----------------\n");
        test_memcpy(pow(4, i + 5));
        test_simple(pow(4, i + 5));
        test_ompsimd(pow(4, i + 5));
    }

    return 0;
}