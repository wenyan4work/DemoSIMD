/*
 * EigenDef.hpp
 *
 *  Created on: Oct 27, 2016
 *      Author: wyan
 */

#ifndef EIGENDEF_HPP_
#define EIGENDEF_HPP_

#define EIGEN_DONT_PARALLELIZE  // 1 thread for any eigen stuff
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <vector>
#include <cmath>

/*
 *
 * Do not use auto with Eigen
 *
 * Eigen defaults to column-major storage.
 * Mat(i,j) is not affected by row major or column major storage format
 *
 * */

// unaligned fixed size vectors
using Evec2 = Eigen::Matrix<double, 2, 1, Eigen::DontAlign>;
using Evec3 = Eigen::Matrix<double, 3, 1, Eigen::DontAlign>;
using Evec4 = Eigen::Matrix<double, 4, 1, Eigen::DontAlign>;
using Evec6 = Eigen::Matrix<double, 6, 1, Eigen::DontAlign>;

// unaligned fixed size matrices
using Emat2 = Eigen::Matrix<double, 2, 2, Eigen::DontAlign>;
using Emat3 = Eigen::Matrix<double, 3, 3, Eigen::DontAlign>;
using Emat4 = Eigen::Matrix<double, 4, 4, Eigen::DontAlign>;
using Emat6 = Eigen::Matrix<double, 6, 6, Eigen::DontAlign>;

// unaligned quaternion
using Equatn = Eigen::Quaternion<double, Eigen::DontAlign>;

// aligned fixed size vectors
using EAvec2 = Eigen::Vector2d;
using EAvec3 = Eigen::Vector3d;
using EAvec4 = Eigen::Vector4d;
using EAvec6 = Eigen::Matrix<double, 6, 1>;

// aligned fixed size matrices
using EAmat2 = Eigen::Matrix2d;
using EAmat3 = Eigen::Matrix3d;
using EAmat4 = Eigen::Matrix4d;
using EAmat6 = Eigen::Matrix<double, 6, 6>;

// aligned quaternion
using EAquatn = Eigen::Quaternion<double>;

// dynamic size types
using Evec = Eigen::VectorXd;
using Espmat = Eigen::SparseMatrix<double>;                         // default to column major
using Emat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>; // default to column major

#endif /* EIGENDEF_HPP_ */
