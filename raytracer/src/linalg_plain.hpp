#ifndef LINALG_PLAIN_HPP
#define LINALG_PLAIN_HPP

#include <cstdint>
#include <cmath>
#include <algorithm>

class Vec3f {
private:
    float x, y, z;

public:

    Vec3f(float x, float y, float z): x(x), y(y), z(z) {}

    Vec3f(const Vec3f& other): x(other.x), y(other.y), z(other.z) {}

#ifdef __CL_PLATFORM_H
    cl_float4 toCL() const {
        cl_float4 c;
        c.s[0] = x;
        c.s[1] = y;
        c.s[2] = z;
        c.s[3] = 0;

        return c;
    }
#endif

    Vec3f cross(const Vec3f& other) const {
        return Vec3f(y * other.z - z * other.y,
                     z * other.x - x * other.z,
                     x * other.y - y * other.x);
    }

    float dot(const Vec3f &other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }

    void operator+=(const Vec3f& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    Vec3f operator +(const Vec3f& other) const {
        return Vec3f(x + other.x, y + other.y, z + other.z);
    }

    Vec3f operator -(const Vec3f& other) const {
        return Vec3f(x - other.x, y - other.y, z - other.z);
    }

    Vec3f operator *(const Vec3f& other) const {
        return Vec3f(x * other.x, y * other.y, z * other.z);
    }

    Vec3f operator *(float a) const {
        return Vec3f(x * a, y * a, z * a);
    }

    friend Vec3f operator*(float a, const Vec3f& v) {
        return v * a;
    }

    Vec3f operator /(float a) const {
        float inv = 1 / a;
        return Vec3f(inv * x, inv * y, inv * z);
    }

    Vec3f normalized_accurate() const {
        float length = sqrt(dot(*this));
        return *this / length;
    }

    // Could use the magic inverse sqrt formula, but SSE does that in
    // hardware anyway.
    Vec3f normalized() const {
        return normalized_accurate();
    }

    void setX(float a) {
        x = a;
    }

    void setY(float a) {
        y = a;
    }

    void setZ(float a) {
        z = a;
    }
};

#endif
