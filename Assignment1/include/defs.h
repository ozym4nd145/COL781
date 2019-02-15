#pragma once

#include <Eigen/Core>

typedef Eigen::Matrix<float, 1, 3, Eigen::RowMajor> Vector3f;
typedef Eigen::Matrix<float, 1, 4, Eigen::RowMajor> Vector4f;
typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> Matrix3f;
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> Matrix4f;

typedef Vector3f Point;
typedef Vector3f Color;

const double PI = 3.14159265358979323846;
const double EPSILON = 1e-3;