#pragma once

#include <Eigen/Core>

typedef Eigen::Matrix<float, 1, 3, Eigen::RowMajor> Vector3f;  // Row Vector
typedef Eigen::Matrix<float, 1, 4, Eigen::RowMajor> Vector4f;
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> Matrix4f;

typedef Vector3f Point;
typedef Vector3f Color;

const double PI = 3.14159265358979323846;
const double EPSILON = 1e-3;

Vector4f augment(const Vector3f &vec, float val) {
    Vector4f new_vec;
    new_vec.head(3) = vec;
    new_vec[3] = val;
    return new_vec;
}

Vector3f apply_transformation(const Vector3f &point, const Matrix4f &trans) {
    Vector4f p_temp = augment(point, 1.0);
    Vector4f new_point = p_temp * trans;
    new_point[0] /= new_point[3];
    new_point[1] /= new_point[3];
    new_point[2] /= new_point[3];
    return Vector3f(new_point.head(3));
}

std::optional<std::pair<float, float>> solve_quadratic(float a, float b,
                                                       float c) {
    if (a == 0) {
        if (b == 0) {
            if (c == 0) return std::make_pair(c, c);
            return {};
        } else
            return std::make_pair(-c / b, -c / b);
    }
    float D = b * b - (4.0 * a * c);
    if (D < 0) return {};
    float sqrt_D = sqrt(D);
    float x1 = (-b - sqrt_D) * 0.5 / a;
    float x2 = (-b + sqrt_D) * 0.5 / a;
    return std::make_pair(std::min(x1, x2), std::max(x1, x2));
}
