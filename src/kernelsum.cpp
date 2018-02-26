#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AlignedMemory.hpp"
#include "Timer.hpp"

#include "pvfmm.hpp"

PeriodicType periodicType;


constexpr double Pi = 3.141592653589793238462643383279;

inline double Abs(double x) { return std::abs(x); }

inline double Sqrt(double x) { return std::sqrt(x); }

inline double Power(double x, double y) { return std::pow(x, y); }

inline void StokesSLVel(double *s, double *t, double *f, double *vel) {
    const double sx = s[0];
    const double sy = s[1];
    const double sz = s[2];

    const double tx = t[0];
    const double ty = t[1];
    const double tz = t[2];

    if (sx == tx && sy == ty && sz == tz) {
        vel[0] = vel[1] = vel[2] = 0;
        return;
    }

    const double fx = f[0];
    const double fy = f[1];
    const double fz = f[2];

    double &vx = vel[0];
    double &vy = vel[1];
    double &vz = vel[2];

    vx = ((sx - tx) * (fy * (sy - ty) + fz * (sz - tz)) +
          fx * (2 * Power(sx, 2) + Power(sy, 2) + Power(sz, 2) - 4 * sx * tx + 2 * Power(tx, 2) - 2 * sy * ty +
                Power(ty, 2) - 2 * sz * tz + Power(tz, 2))) /
         (8. * Pi * Power(Power(sx - tx, 2) + Power(sy - ty, 2) + Power(sz - tz, 2), 1.5));
    vy = ((sy - ty) * (fx * (sx - tx) + fz * (sz - tz)) +
          fy * (Power(sx, 2) + 2 * Power(sy, 2) + Power(sz, 2) - 2 * sx * tx + Power(tx, 2) - 4 * sy * ty +
                2 * Power(ty, 2) - 2 * sz * tz + Power(tz, 2))) /
         (8. * Pi * Power(Power(sx - tx, 2) + Power(sy - ty, 2) + Power(sz - tz, 2), 1.5));
    vz = ((fx * (sx - tx) + fy * (sy - ty)) * (sz - tz) +
          fz * (Power(sx, 2) + Power(sy, 2) + 2 * Power(sz, 2) - 2 * sx * tx + Power(tx, 2) - 2 * sy * ty +
                Power(ty, 2) - 4 * sz * tz + 2 * Power(tz, 2))) /
         (8. * Pi * Power(Power(sx - tx, 2) + Power(sy - ty, 2) + Power(sz - tz, 2), 1.5));
}

void test_simple(int npts) {
    double *cPtr = new double[3 * npts]; // coordinate
    double *fPtr = new double[3 * npts]; // force
    double *vPtr = new double[3 * npts]; // velocity
    for (int i = 0; i < 3 * npts; i++) {
        fPtr[i] = 1;
        vPtr[i] = 0;
        cPtr[i] = i;
    }

    Timer timer;
    timer.start();

    for (int i = 0; i < npts; i++) {
        double v[3] = {0, 0, 0};
        for (int j = 0; j < npts; j++) {
            double vij[3] = {0, 0, 0};
            // i is target
            // j is source
            StokesSLVel(cPtr + 3 * j, cPtr + 3 * i, fPtr + 3 * j, vij);
            v[0] += vij[0];
            v[1] += vij[1];
            v[2] += vij[2];
        }
        vPtr[3 * i] = v[0];
        vPtr[3 * i + 1] = v[1];
        vPtr[3 * i + 2] = v[2];
    }

    timer.stop("simple sum complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf M interactions/s\n", vPtr[0], (npts * npts) * (1e-6) / timer.getTime());

    delete[] cPtr;
    delete[] fPtr;
    delete[] vPtr;
}

void test_expand(int npts) {
    double *cPtr = new double[3 * npts]; // coordinate
    double *fPtr = new double[3 * npts]; // force
    double *vPtr = new double[3 * npts]; // velocity
    for (int i = 0; i < 3 * npts; i++) {
        fPtr[i] = 1;
        vPtr[i] = 0;
        cPtr[i] = i;
    }

    const double factor8pi = 1 / (8 * Pi);

    Timer timer;
    timer.start();

    for (int i = 0; i < npts; i++) {
        const double tx = cPtr[3 * i];
        const double ty = cPtr[3 * i + 1];
        const double tz = cPtr[3 * i + 2];
        double trgValueX = 0;
        double trgValueY = 0;
        double trgValueZ = 0;
        // rely on compiler for auto-vectorization
        for (int j = 0; j < npts; j++) {
            const double lx = cPtr[3 * j];
            const double ly = cPtr[3 * j + 1];
            const double lz = cPtr[3 * j + 2];
            const double fx = fPtr[3 * j];
            const double fy = fPtr[3 * j + 1];
            const double fz = fPtr[3 * j + 2];
            const double rx = (tx - lx);
            const double ry = (ty - ly);
            const double rz = (tz - lz);
            const double rnorm2 = rx * rx + ry * ry + rz * rz;
            if (rnorm2 != 0) {
                const double rinv = 1 / sqrt(rnorm2);
                const double rinv3 = rinv * rinv * rinv;
                const double commonFac = (rx * fx + ry * fy + rz * fz);
                trgValueX += fx * rinv + commonFac * rx * rinv3;
                trgValueY += fy * rinv + commonFac * ry * rinv3;
                trgValueZ += fz * rinv + commonFac * rz * rinv3;
            }
        }
        vPtr[3 * i] += trgValueX * factor8pi;
        vPtr[3 * i + 1] += trgValueY * factor8pi;
        vPtr[3 * i + 2] += trgValueZ * factor8pi;
    }

    timer.stop("expand sum complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf M interactions/s\n", vPtr[0], (npts * npts) * (1e-6) / timer.getTime());
    delete[] cPtr;
    delete[] fPtr;
    delete[] vPtr;
}

void test_pvfmm(int npts) {
    MPI_Init(nullptr,nullptr);
    double *cPtr = new double[3 * npts]; // coordinate
    double *fPtr = new double[3 * npts]; // force
    double *vPtr = new double[3 * npts]; // velocity
    for (int i = 0; i < 3 * npts; i++) {
        fPtr[i] = 1;
        vPtr[i] = 0;
        cPtr[i] = i;
    }

    const pvfmm::Kernel<double> &kernel = pvfmm::StokesKernel<double>::velocity();
    pvfmm::PtFMM matrixG;
    // pvfmm::PtFMM_Tree *treePtrG;
    // pvfmm::PtFMM_Data treeDataG;
    matrixG.Initialize(12, MPI_COMM_WORLD, &kernel);

    Timer timer;
    timer.start();

    kernel.k_s2t->ker_poten(cPtr, npts, fPtr, 1, cPtr, npts, vPtr, NULL);

    timer.stop("pvfmm sum complete, ");
    timer.dump();
    printf("result0: %lf; Speed: %lf M interactions/s\n", vPtr[0], (npts * npts) * (1e-6) / timer.getTime());
    delete[] cPtr;
    delete[] fPtr;
    delete[] vPtr;
    MPI_Finalize();
}

void test_ompsimd(int size) {}

int main() {
    test_simple(2000);
    test_expand(2000);
    test_pvfmm(2000);
    return 0;
}