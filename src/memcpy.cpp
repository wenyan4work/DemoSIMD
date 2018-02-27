#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#define REPEAT (1)

void test_memcpy(int size) {
    double *fromPtr = new double[size];
    double *toPtr = new double[size];
    Timer timer;
    timer.start();
    for (int i = 0; i < REPEAT; i++) {
        memcpy(toPtr, fromPtr, sizeof(double) * size);
    }
    timer.stop("memcpy complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    delete[] fromPtr;
    delete[] toPtr;
}

void test_simple(int size) {
    double *fromPtr = new double[size];
    double *toPtr = new double[size];
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
    delete[] fromPtr;
    delete[] toPtr;
}

void test_alignedsimple(int size) {
    AlignedMemory<double, 32> frommem(size);
    AlignedMemory<double, 32> tomem(size);
    double *fromPtr = frommem.alignedPtr;
    double *toPtr = tomem.alignedPtr;
    Timer timer;
    timer.start();
    for (int j = 0; j < REPEAT; j++) {
        for (int i = 0; i < size; i++) {
            toPtr[i] = fromPtr[i];
        }
    }
    timer.stop("aligned simple copy complete, ");
    timer.dump();
    printf("data %lf MB, bandwidth: %lf MB/s\n", (size * 8 / (1024.0 * 1024.0)),
           (size * 8 / (1024.0 * 1024.0)) * REPEAT / timer.getTime());
    printf("%f\n", toPtr[size - 1]);
}

void test_ompsimd(int size) {
    AlignedMemory<double, 32> frommem(size);
    AlignedMemory<double, 32> tomem(size);
    double *fromPtr = frommem.alignedPtr;
    double *toPtr = tomem.alignedPtr;
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
    // warming up
    test_ompsimd(1000000);
    printf("--------warming up completed---------\n");

    for (int i = 4; i < 9; i++) {
        printf("-----------------\n");
        const int size = pow(4, i + 5);
        test_memcpy(size);
        test_simple(size);
        test_alignedsimple(size);
        test_ompsimd(size);
    }

    return 0;
}