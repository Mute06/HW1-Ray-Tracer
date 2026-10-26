#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <stdexcept>

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    constexpr Vec3() = default;

    constexpr Vec3(double x, double y, double z)
        : x(x), y(y), z(z) {}


    double Length() const {
        return std::hypot(x,y,z); // equals to sqrt(x * x + y * y + z * z); but prevents overflow/underflow
    }

    double LengthSquared() const {
        return x * x + y * y + z * z;
    }

    static double Distance(const Vec3& a, const Vec3& b) {
        return (a - b).Length();
    }

    static Vec3 Add(const Vec3& a, const Vec3& b) {
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    Vec3 operator+(const Vec3& b) const {
        return Vec3(this->x + b.x, this->y + b.y, this->z + b.z);
    }

    static Vec3 Subtract(const Vec3& a, const Vec3& b) {
        return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    Vec3 operator-(const Vec3& b) const {
        return Vec3(this->x - b.x, this->y - b.y, this->z - b.z);
    }

    static double Dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    Vec3 operator*(const double scalar) const {
        return Vec3(this->x * scalar, this->y *scalar, this->z * scalar);
    }

    friend Vec3 operator*(const double scalar, const Vec3& vec) {
        return vec * scalar;
    }


    Vec3 operator/(const double scalar) const {
        if (scalar == 0.0) {
            throw std::domain_error("Vector3: division by zero");
        }
        return Vec3(this->x / scalar, this->y / scalar, this->z / scalar);
    }

    Vec3 operator-() const{
        return Vec3(-this->x, -this->y, -this->z);
    }

    bool operator==(const Vec3 &other) const {
        return other.x == this->x && other.y == this->y && other.z == this->z;
    }
    bool operator!=(const Vec3 &other) const {

        return !(*this == other);
    }

    static bool IsEqual(const Vec3& a, const Vec3& b,
                        double epsilon = 0.0001) {
        return std::abs(a.x - b.x) <= epsilon &&
               std::abs(a.y - b.y) <= epsilon &&
               std::abs(a.z - b.z) <= epsilon;
    }

    bool IsEqual(const Vec3& other,
             double epsilon = 0.0001) const {
        return std::abs(x - other.x) <= epsilon &&
               std::abs(y - other.y) <= epsilon &&
               std::abs(z - other.z) <= epsilon;
    }

    //Only returns the normalized result, doesnt change original vector
    Vec3 Normalize() const {
        const double len = Length();
        if (len > 1e-14) {
            return Vec3(x / len, y / len, z / len);

        }
        return Vec3(0.0, 0.0, 0.0);
    }

    bool IsNormalized() const {
        return LengthSquared() == 1.0;
    }

    //Constants
    static constexpr Vec3 Zero() {
        return {0.0, 0.0, 0.0};
    }

    static constexpr Vec3 One() {
        return {1.0, 1.0, 1.0};
    }

    static constexpr Vec3 UnitX() {
        return {1.0, 0.0, 0.0};
    }

    static constexpr Vec3 UnitY() {
        return {0.0, 1.0, 0.0};
    }

    static constexpr Vec3 UnitZ() {
        return {0.0, 0.0, 1.0};
    }

};

struct Ray {
    Vec3 origin;
    Vec3 direction;
    int depth = 0;

    Ray() = default;

    Ray(const Vec3& origin, const Vec3& direction, int depth = 0)
        : origin(origin), direction(direction), depth(depth) {  }

    Vec3 PointAt(const double t) const {
        return origin + (direction * t);
    }
};

#endif // MATH_H