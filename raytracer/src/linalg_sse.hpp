#ifndef LINALG_SSE_HPP
#define LINALG_SSE_HPP

#include "xmmintrin.h"
#include <stdint.h>
#include <algorithm>

#ifdef _MSC_VER
#define ALIGN16(x) __declspec(align(16)) x
#else // GCC or Clang
#define ALIGN16(x) x __attribute__((aligned(16)))
#endif

class Vec3f {
private:
    __m128 v;
    Vec3f(__m128 v): v(v) {}

    __m128 dot_m128(const Vec3f &other) const {
        __m128 p = _mm_mul_ps(v, other.v);
        __m128 s2 = _mm_add_ps(p, _mm_shuffle_ps(p, p, _MM_SHUFFLE(3, 3, 3, 2)));
        return _mm_add_ps(s2, _mm_shuffle_ps(s2, s2, _MM_SHUFFLE(3, 3, 3, 1)));
    }

public:
    Vec3f(float x, float y, float z) {
        v = _mm_set_ps(0, z, y, x);
    }

    Vec3f(const Vec3f& other): v(other.v) {}

#ifdef __CL_PLATFORM_H
    cl_float4 toCL() const {
        ALIGN16(float vv[4]);
        _mm_store_ps(vv, v);

        cl_float4 c;
        c.s[0] = vv[0];
        c.s[1] = vv[1];
        c.s[2] = vv[2];
        c.s[3] = 0;

        return c;
    }
#endif

    Vec3f cross(const Vec3f& other) const {
        __m128 im = _mm_sub_ps(_mm_mul_ps(v, _mm_shuffle_ps(other.v, other.v, _MM_SHUFFLE(3, 0, 2, 1))),
                               _mm_mul_ps(other.v, _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1))));
        return Vec3f(_mm_shuffle_ps(im, im, _MM_SHUFFLE(3, 0, 2, 1)));
    }

    float dot(const Vec3f &other) const {
		ALIGN16(float a[4]);
        // float a[4] ALIGN16;
        _mm_store_ps(a, dot_m128(other));
        return a[0];
    }

    void operator+=(const Vec3f& other) {
        v = _mm_add_ps(v, other.v);
    }

    Vec3f operator +(const Vec3f& other) const {
        return Vec3f(_mm_add_ps(v, other.v));
    }

    Vec3f operator -(const Vec3f& other) const {
        return Vec3f(_mm_sub_ps(v, other.v));
    }

    Vec3f operator *(const Vec3f& other) const {
        return Vec3f(_mm_mul_ps(v, other.v));
    }

    Vec3f operator *(float a) const {
        __m128 aa = _mm_load1_ps(&a);
        return Vec3f(_mm_mul_ps(v, aa));
    }

    friend Vec3f operator*(float a, const Vec3f& v) {
        return v * a;
    }

    Vec3f operator /(float a) const {
        __m128 aa = _mm_load1_ps(&a);
        return Vec3f(_mm_div_ps(v, aa));
    }

    Vec3f normalized() const {
        __m128 len2 = dot_m128(*this);
        len2 = _mm_shuffle_ps(len2, len2, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 invlen = _mm_rsqrt_ps(len2);
        return Vec3f(_mm_mul_ps(v, invlen));
    }

    /**
     * Used for the initial rays from camera. If they are off, the
     * errors are multiplied in reflections and become visible.
     */
    Vec3f normalized_accurate() const {
        __m128 len2 = dot_m128(*this);
        len2 = _mm_shuffle_ps(len2, len2, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 len = _mm_sqrt_ps(len2);
        return Vec3f(_mm_div_ps(v, len));
    }

    void setX(float a) {
        ALIGN16(float vv[4]);
        _mm_store_ps(vv, v);
        vv[0] = a;
        v = _mm_load_ps(vv);
    }

    void setY(float a) {
        ALIGN16(float vv[4]);
        _mm_store_ps(vv, v);
        vv[1] = a;
        v = _mm_load_ps(vv);
    }

    void setZ(float a) {
        ALIGN16(float vv[4]);
        _mm_store_ps(vv, v);
        vv[2] = a;
        v = _mm_load_ps(vv);
    }
};

#endif
