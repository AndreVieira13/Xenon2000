// ============================================================================
// Vector2D.cpp
// Implements Vector2D arithmetic operators and 2D vector math helpers.
// ============================================================================
#include "engine/Vector2D.h"
#include <cmath>

namespace engine {

// Adds another vector component-wise into this vector.
Vector2D& Vector2D::operator+=(Vector2D other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
}

// Subtracts another vector component-wise from this vector.
Vector2D& Vector2D::operator-=(Vector2D other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
}

// Scales both components of this vector by a scalar factor.
Vector2D& Vector2D::operator*=(float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    return *this;
}

// Divides both components of this vector by a scalar factor.
Vector2D& Vector2D::operator/=(float scalar) noexcept {
    x /= scalar;
    y /= scalar;
    return *this;
}

// Returns the component-wise sum of two vectors (a + b) using operator+=.
Vector2D operator+(Vector2D a, Vector2D b) noexcept {
    a += b;
    return a;
}

// Returns the component-wise difference of two vectors (a - b) using operator-=.
Vector2D operator-(Vector2D a, Vector2D b) noexcept {
    a -= b;
    return a;
}

// Returns the negated vector (-x, -y).
Vector2D operator-(Vector2D v) noexcept {
    return Vector2D{-v.x, -v.y};
}

// Returns a vector scaled by s (vector * scalar).
Vector2D operator*(Vector2D v, float s) noexcept {
    v *= s;
    return v;
}

// Returns a vector scaled by s (scalar * vector).
Vector2D operator*(float s, Vector2D v) noexcept {
    v *= s;
    return v;
}

// Returns a vector divided by scalar s.
Vector2D operator/(Vector2D v, float s) noexcept {
    v /= s;
    return v;
}

// Returns true when both x and y components are equal.
bool operator==(Vector2D a, Vector2D b) noexcept {
    return a.x == b.x && a.y == b.y;
}

// Returns true when either component differs.
bool operator!=(Vector2D a, Vector2D b) noexcept {
    return !(a == b);
}

// Returns x^2 + y^2 without invoking std::sqrt().
float lengthSquared(Vector2D v) noexcept {
    return v.x * v.x + v.y * v.y;
}

// Returns the Euclidean length (magnitude) via Pythagoras: sqrt(x^2 + y^2).
float length(Vector2D v) noexcept {
    return std::sqrt(lengthSquared(v));
}

// Returns a unit vector pointing in the same direction as v, or (0, 0) if near zero.
Vector2D normalized(Vector2D v) noexcept {
    const float len = length(v);
    // Prevent division by zero (or NaN) when normalizing a zero-length vector.
    if (len <= 0.00001f) {
        return Vector2D{0.0f, 0.0f};
    }
    return v / len;
}

// Linearly interpolates between a (t = 0) and b (t = 1).
Vector2D lerp(Vector2D a, Vector2D b, float t) noexcept {
    return Vector2D{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

} // namespace engine
