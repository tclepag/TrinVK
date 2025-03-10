//
// Created by lepag on 3/9/25.
//

#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

namespace Trin::Math {
    class Vector3 {
    public:
        Vector3(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        Vector3(float scalar) {
            this->x = scalar;
            this->y = scalar;
            this->z = scalar;
        }

        Vector3() {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }

        Vector3 operator+(const Vector3 &v) {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }

        Vector3 operator-(const Vector3 &v) {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }

        Vector3 operator*(const Vector3 &v) {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
        }

        Vector3 operator/(const Vector3 &v) {
            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            return *this;
        }

        Vector3 operator-() {
            this->x = -this->x;
            this->y = -this->y;
            this->z = -this->z;
            return *this;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        void set(const float ox, const float oy, const float oz) {
            this->x = ox;
            this->y = oy;
            this->z = oz;
        }

        void set(const Vector3 &v) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
        }

        [[nodiscard]] float dot(const Vector3 &other) const {
          return this->x * other.x + this->y * other.y + this->z * other.z;
        }

        [[nodiscard]] float magnitude() const {
          return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        }

        Vector3 normalize() {
            float mag = magnitude();

            // Avoid division by zero
            if (mag > 0.0f) {
                x /= mag;
                y /= mag;
                z /= mag;
            }
            return *this;
        }

        static Vector3 zero() { return {0.0f, 0.0f, 0.0f}; };
        static Vector3 one() { return {1.0f, 1.0f, 1.0f}; };
        static Vector3 unit_x() { return {1.0f, 0.0f, 0.0f}; };
        static Vector3 unit_y() { return {0.0f, 1.0f, 0.0f}; };
        static Vector3 unit_z() { return {0.0f, 0.0f, 1.0f}; };
    };
}

#endif //VECTOR3_H
