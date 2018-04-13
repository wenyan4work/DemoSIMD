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

## legp.cpp:
Demonstrates the effect of HornerForm in polynomial evaluations.

# References:
1. What Every Programmer Should Know About Memory
2. Intel® 64 and IA-32 Architectures Optimization Reference Manual
3. Intel instrinsics guide: https://software.intel.com/sites/landingpage/IntrinsicsGuide/ 
4. Agner Fog’s website: http://www.agner.org/
5. Online Compiler Explorer: https://gcc.godbolt.org/
