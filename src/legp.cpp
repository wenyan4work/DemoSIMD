#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#define REPEAT (100000)

void test_simple(int size) {
    Timer timer;
    timer.start();
    // evaluate LegendreP(10,x)
    double sum = 0;
    for (double i = 0; i < size; i += 1) {
        const double x = cos(i);
        sum +=
            (-63 + 3465 * pow(x, 2) - 30030 * pow(x, 4) + 90090 * pow(x, 6) - 109395 * pow(x, 8) + 46189 * pow(x, 10)) /
            256.;
    }
    timer.stop("Simple eval complete.");
    timer.dump();
    printf("eval LegP(10,x) %d times, sum: %18.16lf\n", size, sum);
}

void test_horner(int size) {
    Timer timer;
    timer.start();
    // evaluate LegendreP(10,x)
    double sum = 0;
    for (double i = 0; i < size; i += 1) {
        const double x = cos(i);
        const double x2 = x * x;
        sum +=
            -0.24609375 +
            x2 * (13.53515625 + x2 * (-117.3046875 + x2 * (351.9140625 + x2 * (-427.32421875 + (46189 * x2) / 256.))));
    }
    timer.stop("HornerForm eval complete.");
    timer.dump();
    printf("eval LegP(10,x) %d times, sum: %18.16lf\n", size, sum);
}

int main() {
    // warming up

    for (int i = 4; i < 9; i++) {
        printf("-----------------\n");
        const int size = pow(4, i + 5);
        test_simple(size);
        test_horner(size);
    }

    return 0;
}