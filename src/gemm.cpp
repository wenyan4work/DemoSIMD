#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

void test_align32(int size) {
    AlignedMemory<double, 32> Amem(size * size);
    AlignedMemory<double, 32> Bmem(size * size);
    AlignedMemory<double, 32> Cmem(size * size);
    double *A = Amem.alignedPtr;
    double *B = Bmem.alignedPtr;
    double *C = Cmem.alignedPtr;

    for (int i = 0; i < size * size; i++) {
        A[i] = 1;
        B[i] = 1;
        C[i] = 0;
    }

    Timer timer;
    timer.start();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
    }

    timer.stop("align32 simple gemm complete, ");
    timer.dump();
    printf("C[0,0]: %lf\n", C[0]);
    printf("data %lf MB, %lf DP GFLOP/s\n", pow(size / 1024.0, 2.0) * 8, 2 * pow(size / 1024.0, 3.0) / timer.getTime());
}

#ifdef HAVE_EIGEN

#include "EigenDef.hpp"

void test_eigen(int size) {
    using Mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    Mat A(size, size), B(size, size), C(size, size);
    A.setOnes();
    B.setOnes();

    Timer timer;
    timer.start();
    C = (A * B).eval();
    timer.stop("eigen gemm complete, ");
    timer.dump();
    printf("C[0,0]: %lf\n", C(0, 0));
    printf("data %lf MB, %lf DP GFLOP/s\n", pow(size / 1024.0, 2.0) * 8, 2 * pow(size / 1024.0, 3.0) / timer.getTime());
}

#endif

void test_align32blocking(int size) {
    AlignedMemory<double, 32> Amem(size * size);
    AlignedMemory<double, 32> Bmem(size * size);
    AlignedMemory<double, 32> Cmem(size * size);
    double *A = Amem.alignedPtr;
    double *B = Bmem.alignedPtr;
    double *C = Cmem.alignedPtr;

    for (int i = 0; i < size * size; i++) {
        A[i] = 1;
        B[i] = 1;
        C[i] = 0;
    }

    Timer timer;
    timer.start();

    const int blockSize = 32; // blockSize always divides size. a simple example
    // 32 gives better performance than 16

    for (int i = 0; i < size; i += blockSize)
        for (int k = 0; k < size; k += blockSize)
            for (int j = 0; j < size; j += blockSize)
                for (int iInner = i; iInner < i + blockSize; iInner++)
                    for (int kInner = k; kInner < k + blockSize; kInner++)
                        for (int jInner = j; jInner < j + blockSize; jInner++)
                            C[iInner * size + jInner] += A[iInner * size + kInner] * B[kInner * size + jInner];

    timer.stop("align32 blocking gemm complete, ");
    timer.dump();
    printf("C[0,0]: %lf\n", C[0]);
    printf("data %lf MB, %lf DP GFLOP/s\n", pow(size / 1024.0, 2.0) * 8, 2 * pow(size / 1024.0, 3.0) / timer.getTime());
}

int main() {
    for (int i = 6; i < 11; i++) {
        printf("-----------------\n");
        test_align32(pow(2, i));
        test_align32blocking(pow(2, i));
#ifdef HAVE_EIGEN
        test_eigen(pow(2, i));
#endif
    }

    return 0;
}