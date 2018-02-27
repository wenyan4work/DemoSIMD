# A collection of simple demos for SIMD and cache tutorials.

To demo simd tricks, cache blocking tricks, and more to be added.

## To compile:
Set the three switches to 'yes' or 'no' at the beginning of Makefile. 'VECREPORT' works only with intel compilers 

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