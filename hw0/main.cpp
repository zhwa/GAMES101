#include <cmath>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>
#include <numbers>

Eigen::Vector2f RotateVector(const Eigen::Vector2f& p, float rad)
{
    const Eigen::Vector3f point{ p[0], p[1], 1.F };
    const Eigen::Matrix3f rotation{ {std::cos(rad), -std::sin(rad), 0}, {std::sin(rad), std::cos(rad), 0}, {0, 0, 1.F} };
    const Eigen::Vector3f transformed = rotation * point;
    return { transformed[0], transformed[1] };
}

Eigen::Vector2f TranslateVector(const Eigen::Vector2f& p, float x, float y)
{
    const Eigen::Vector3f point{ p[0], p[1], 1.F };
    const Eigen::Matrix3f translation{ {1.F, 0, x}, {0, 1.F, y}, {0, 0, 1.F} };
    const Eigen::Vector3f transformed = translation * point;
    return { transformed[0], transformed[1] };
}

int main(){

    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a/b << std::endl;
    std::cout << std::sqrt(b) << std::endl;
    std::cout << std::acos(-1) << std::endl;
    std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl;

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f,2.0f,3.0f);
    Eigen::Vector3f w(1.0f,0.0f,0.0f);
    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    std::cout << v + w << std::endl;
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl;
    std::cout << 2.0f * v << std::endl;

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix3f i,j;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl;
    // matrix add i + j
    Eigen::Matrix3f sum = i + j;
    std::cout << "Sum of i and j: \n" << sum << std::endl;
    // matrix scalar multiply i * 2.0
    Eigen::Matrix3f doubled = i * 2.0F;
    std::cout << "Twice of i: \n" << doubled << std::endl;
    // matrix multiply i * j
    Eigen::Matrix3f prod = i * j;
    std::cout << "Product of i and j: \n" << prod << std::endl;
    // matrix multiply vector i * v
    Eigen::Vector3f out = i * v;
    std::cout << "Product of i and v: \n" << out << std::endl;

    // P = [2, 1]
    const Eigen::Vector2f p{ 2.F, 1.F };
    const auto roated = RotateVector(p, std::numbers::pi / 4);
    std::cout << "Rotated point: \n" << roated << std::endl;
    const auto translated = TranslateVector(roated, 1, 2);
    std::cout << "Translated point: \n" << translated << std::endl;

    return 0;
}