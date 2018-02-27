#include <cmath>
#include <cstdio>

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
    conv.d = conv.d * (threehalfs - (x2 * conv.d * conv.d));
    // Newton iteration: rinv = 0.5 rinv_approx ( 3 - r2 rinv_approx^2 )

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
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_simplef(int size) {

    float data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    float result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < 8; i++) {
            result[i] += 1 / sqrt(data[i] + result[i]);
        }
    }
    timer.stop("simple rsqrt complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_avxf0(int size) {
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
    const __m256 half = _mm256_set1_ps(0.5f);
    const __m256 three = _mm256_set1_ps(3.0f);

    __m256 r = _mm256_load_ps(result);

    for (unsigned int j = 0; j < size; j++) {
        __m256 x = _mm256_add_ps(r, d);
        __m256 nr = _mm256_rsqrt_ps(x); // inverse sqrt approximate
        // __m256 muls = _mm256_mul_ps(_mm256_mul_ps(x, nr), nr);
        // __m256 result = _mm256_mul_ps(_mm256_mul_ps(half, nr), _mm256_sub_ps(three, muls)); // newton iteration

        r = _mm256_add_ps(r, nr);
    }
    _mm256_store_ps(result, r);

    timer.stop("avx rsqrtf NWTN 0 complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_avxf1(int size) {
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
    const __m256 half = _mm256_set1_ps(0.5f);
    const __m256 three = _mm256_set1_ps(3.0f);

    __m256 r = _mm256_load_ps(result);

    for (unsigned int j = 0; j < size; j++) {
        __m256 x = _mm256_add_ps(r, d);
        __m256 nr = _mm256_rsqrt_ps(x); // inverse sqrt approximate
        __m256 muls = _mm256_mul_ps(_mm256_mul_ps(x, nr), nr);
        __m256 result = _mm256_mul_ps(_mm256_mul_ps(half, nr), _mm256_sub_ps(three, muls)); // newton iteration

        r = _mm256_add_ps(r, result);
    }
    _mm256_store_ps(result, r);

    timer.stop("avx rsqrtf NWTN 1 complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_avxd2(int size) {
    AlignedMemory<double, 32> datamem(8);
    AlignedMemory<double, 32> resultmem(8);

    double *data = datamem.alignedPtr;
    double *result = resultmem.alignedPtr;

    for (int i = 0; i < 8; i++) {
        data[i] = i + 1;
        result[i] = 0;
    }
    // float data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // float result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();

    const __m256d d0 = _mm256_load_pd(data);
    const __m256d d1 = _mm256_load_pd(data + 4);
    const __m256d half = _mm256_set1_pd(0.5);
    const __m256d three = _mm256_set1_pd(3.0);

    __m256d r0 = _mm256_load_pd(result);
    __m256d r1 = _mm256_load_pd(result + 4);

    for (unsigned int j = 0; j < size; j++) {
        // 0,1,2,3
        __m256d x = _mm256_add_pd(r0, d0);
        __m256d nr = _mm256_cvtps_pd(_mm_rsqrt_ps(_mm256_cvtpd_ps(x))); // inverse sqrt approximate
        __m256d muls = _mm256_mul_pd(_mm256_mul_pd(x, nr), nr);
        __m256 result = _mm256_mul_pd(_mm256_mul_pd(half, nr), _mm256_sub_pd(three, muls)); // newton iteration
        muls = _mm256_mul_pd(_mm256_mul_pd(x, nr), nr);
        result = _mm256_mul_pd(_mm256_mul_pd(half, nr), _mm256_sub_pd(three, muls)); // newton iteration
        r0 = _mm256_add_pd(r0, result);
        // 4,5,6,7
        x = _mm256_add_pd(r1, d1);
        nr = _mm256_cvtps_pd(_mm_rsqrt_ps(_mm256_cvtpd_ps(x))); // inverse sqrt approximate
        muls = _mm256_mul_pd(_mm256_mul_pd(x, nr), nr);
        result = _mm256_mul_pd(_mm256_mul_pd(half, nr), _mm256_sub_pd(three, muls)); // newton iteration
        muls = _mm256_mul_pd(_mm256_mul_pd(x, nr), nr);
        result = _mm256_mul_pd(_mm256_mul_pd(half, nr), _mm256_sub_pd(three, muls)); // newton iteration
        r1 = _mm256_add_pd(r1, result);
    }
    _mm256_store_pd(result, r0);
    _mm256_store_pd(result + 4, r1);

    timer.stop("avx rsqrt double NWTN 2 complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_magicf(int size) {

    float data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    float result[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    Timer timer;
    timer.start();
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < 8; i++) {
            result[i] += frsqrtf(data[i] + result[i]);
        }
    }
    timer.stop("FLOAT magic rsqrt complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
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
    timer.stop("DOUBLE magic rsqrt complete, ");
    timer.dump();
    printf("result0: %.17g; Speed: %lf MRSQRT/s\n", result[0], (size * 8) * (1e-6) / timer.getTime());
}

void test_ompsimd(int size) {}

int main() {
    constexpr int size = 1000000;
    test_avxf0(size);
    printf("--Warming up completed--\n");
    printf("------------------------\n");

    printf("**double precision results**\n");
    test_simple(size);
    test_magicd(size);
    test_avxd2(size);
    printf("------------------------\n");

    printf("**single precision results**\n");
    test_simplef(size);
    test_magicf(size);
    test_avxf0(size);
    test_avxf1(size);
    printf("------------------------\n");
    return 0;
}