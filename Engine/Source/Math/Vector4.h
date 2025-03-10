//
// Created by lepag on 3/9/25.
//

#ifndef VECTOR4_H
#define VECTOR4_H

#include <cmath>

namespace Trin::Math {
    class Vector4 {
    public:
        Vector4(float x, float y, float z, float w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        Vector4(float scalar) {
            this->x = scalar;
            this->y = scalar;
            this->z = scalar;
            this->w = scalar;
        }

        Vector4() {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }

        Vector4 operator+(const Vector4 &v) {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            this->w += v.w;
            return *this;
        }

        Vector4 operator-(const Vector4 &v) {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            this->w -= v.w;
            return *this;
        }

        Vector4 operator*(const Vector4 &v) {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            this->w *= v.w;
            return *this;
        }

        Vector4 operator/(const Vector4 &v) {
            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            this->w /= v.w;
            return *this;
        }

        Vector4 operator-() {
            this->x = -this->x;
            this->y = -this->y;
            this->z = -this->z;
            this->w = -this->w;
            return *this;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        void set(const float ox, const float oy, const float oz, const float ow) {
            this->x = ox;
            this->y = oy;
            this->z = oz;
            this->w = ow;
        }

        void set(const Vector4 &v) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = v.w;
        }

        [[nodiscard]] float dot(const Vector4 &other) const {
          return this->x * other.x + this->y * other.y + this->z * other.z + this->w * other.w;
        }

        [[nodiscard]] float magnitude() const {
          return sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
        }

        Vector4 normalize() {
            float mag = magnitude();

            // Avoid division by zero
            if (mag > 0.0f) {
                x /= mag;
                y /= mag;
                z /= mag;
                w /= mag;
            }
            return *this;
        }

        static Vector4 zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; };
        static Vector4 one() { return {1.0f, 1.0f, 1.0f, 1.0f}; };
        static Vector4 unit_x() { return {1.0f, 0.0f, 0.0f, 0.0f}; };
        static Vector4 unit_y() { return {0.0f, 1.0f, 0.0f, 0.0f}; };
        static Vector4 unit_z() { return {0.0f, 0.0f, 1.0f, 0.0f}; };
        static Vector4 unit_w() { return {0.0f, 0.0f, 0.0f, 1.0f}; };
    };
}

#endif //VECTOR4_H
