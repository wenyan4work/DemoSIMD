#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <immintrin.h>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

inline double frsqrtd(double number) {
    union {
        double d;
        long i;
    } conv;

    double x2;
    const double threehalfs = 1.5;

    x2 = number * 0.5;
    conv.d = number;
    conv.i = 0x5FE6EB50C7B537A9 - (conv.i >> 1); // magic number for float
    conv.d = conv.d * (threehalfs - (x2 * conv.d * conv.d));
    conv.d = conv.d * (threehalfs - (x2 * conv.d * conv.d));
    return conv.d;
}

inline float frsqrtf(float number) {
    union {
        float f;
        long i;
    } conv;

    float x2;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    conv.f = number;
    conv.i = 0x5f3759df - (conv.i >> 1); // magic number for float
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
}

void test_simple(int size) {

    double data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < 8; i++) {
            result[i] += 1 / sqrt(data[i] + result[i]);
        }
    }
    timer.stop("simple rsqrt complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_avxf(int size) {
    AlignedMemory<float, 32> datamem(8);
    AlignedMemory<float, 32> resultmem(8);

    float *data = datamem.alignedPtr;
    float *result = resultmem.alignedPtr;

    for (int i = 0; i < 8; i++) {
        data[i] = i + 1;
        result[i] = 0;
    }
    // float data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // float result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();

    const __m256 d = _mm256_load_ps(data);
    for (unsigned int j = 0; j < size; j++) {
        __m256 r = _mm256_load_ps(result);
        __m256 rsqrt = _mm256_rsqrt_ps(_mm256_add_ps(r, d)); // fast inv sqrt
        r = _mm256_add_ps(r, rsqrt);
        _mm256_store_ps(result, r);
    }

    timer.stop("simple rsqrt complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_magicf(int size) {

    double data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < 8; i++) {
            result[i] += frsqrtf(data[i] + result[i]);
        }
    }
    timer.stop("FLOAT rsqrt complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_magicd(int size) {

    double data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < 8; i++) {
            result[i] += frsqrtd(data[i] + result[i]);
        }
    }
    timer.stop("DOUBLE rsqrt complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_ompsimd(int size) {}

int main() {
    test_simple(1000000);
    test_magicf(1000000);
    test_magicd(1000000);
    test_avxf(1000000);
    return 0;
}