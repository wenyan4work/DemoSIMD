# A collection of simple demos for SIMD and cache tutorials.

To demo simd tricks, cache blocking tricks, and more to be added.

## To compile:
### Case 1: Compile with Eigen and pvfmm
Just modify the paths in the Makefile, and type 'make'. CXXFLAGS and LDFLAGS are set to use PVFMM values.

### Case 2: Compile without Eigen and pvfmm
Comment out eigen and pvfmm dependences in gemm.cpp and kernelsum.cpp, manually add CXXFLAGS and LDFLAGS.

## memcpy.cpp:
Demonstrates memcpy() vs simple loop. Compiler is smart enough to recognize the simple loop pattern and auto optimize the loop

No external dependence

## arrayscan.cpp:
Demonstrates different array operations. The simplest one could be the fasted because the double loop could be auto-switched by the compiler to significantly improve the reuse of data in cache.

No external dependence

## rsqrt.cpp:
Demonstrates different fast inverse square root routines. 

No external dependence

## gemm.cpp:
Demonstrates cache blocking.

The Eigen dgemm function relies on eigen to compile.

## kernelsum.cpp:
Demonstrates the kernel sum performance.

The pvfmm kernel sum function relies pvfmm to compile. 