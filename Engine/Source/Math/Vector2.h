//
// Created by lepag on 3/9/25.
//

#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

namespace Trin::Math {
    class Vector2 {
    public:
        Vector2(float x, float y) {
            this->x = x;
            this->y = y;
        }

        Vector2(float scalar) {
            this->x = scalar;
            this->y = scalar;
        }

        Vector2() {
            x = 0.0f;
            y = 0.0f;
        }

        Vector2 operator+(const Vector2 &v) {
            this->x += v.x;
            this->y += v.y;
            return *this;
        }

        Vector2 operator-(const Vector2 &v) {
            this->x -= v.x;
            this->y -= v.y;
            return *this;
        }

        Vector2 operator*(const Vector2 &v) {
            this->x *= v.x;
            this->y *= v.y;
            return *this;
        }

        Vector2 operator/(const Vector2 &v) {
            this->x /= v.x;
            this->y /= v.y;
            return *this;
        }

        Vector2 operator-() {
            this->x = -this->x;
            this->y = -this->y;
            return *this;
        }

        float x = 0.0f;
        float y = 0.0f;

        void set(const float x, const float y) {
            this->x = x;
            this->y = y;
        }

        void set(const Vector2 &v) {
            this->x = v.x;
            this->y = v.y;
        }

        [[nodiscard]] float dot(const Vector2 &other) const {
            return this->x * other.x + this->y * other.y;
        }

        [[nodiscard]] float magnitude() const {
            return sqrt(this->x * this->x + this->y * this->y);
        }

        Vector2 normalize() {
            float mag = magnitude();

            // Avoid division by zero
            if (mag > 0.0f) {
                x /= mag;
                y /= mag;
            }
            return *this;
        }

        static Vector2 zero() { return {0.0f, 0.0f}; };
        static Vector2 one() { return {1.0f, 1.0f}; };
        static Vector2 unit_x() { return {1.0f, 0.0f}; };
        static Vector2 unit_y() { return {0.0f, 1.0f}; };
    };
}

#endif //VECTOR2_H
