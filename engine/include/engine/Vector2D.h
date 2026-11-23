// ============================================================================
// Vector2D.h
// Lightweight 2D math vector shared across engine and game code so the game
// never has to depend on GLM or Box2D headers directly.
// ============================================================================
#ifndef ENGINE_VECTOR2D_H
#define ENGINE_VECTOR2D_H

#include <cmath>

namespace engine {

struct Vector2D {
    float x{0.0f};
    float y{0.0f};

    // Constructs a zero vector (0, 0).
    constexpr Vector2D() = default;

    // Constructs a vector with the given horizontal (xVal) and vertical (yVal) components.
    constexpr Vector2D(float xVal, float yVal) noexcept : x{xVal}, y{yVal} {}

    // Adds another vector component-wise into this vector.
    constexpr Vector2D& operator+=(Vector2D other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Subtracts another vector component-wise from this vector.
    constexpr Vector2D& operator-=(Vector2D other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Scales both components of this vector by a scalar factor.
    constexpr Vector2D& operator*=(float scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Divides both components of this vector by a scalar factor.
    constexpr Vector2D& operator/=(float scalar) noexcept {
        x /= scalar;
        y /= scalar;
        return *this;
    }
};

// Returns the component-wise sum of two vectors (a + b).
constexpr Vector2D operator+(Vector2D a, Vector2D b) noexcept {
    a += b;
    return a;
}

// Returns the component-wise difference of two vectors (a - b).
constexpr Vector2D operator-(Vector2D a, Vector2D b) noexcept {
    a -= b;
    return a;
}

// Returns the negated vector pointing in the exact opposite direction (-x, -y).
constexpr Vector2D operator-(Vector2D v) noexcept {
    return Vector2D{-v.x, -v.y};
}

// Returns a vector scaled by s (vector * scalar form).
constexpr Vector2D operator*(Vector2D v, float s) noexcept {
    v *= s;
    return v;
}

// Returns a vector scaled by s (scalar * vector form).
constexpr Vector2D operator*(float s, Vector2D v) noexcept {
    v *= s;
    return v;
}

// Returns a vector divided by scalar s.
constexpr Vector2D operator/(Vector2D v, float s) noexcept {
    v /= s;
    return v;
}

// Returns true when both x and y components are equal.
constexpr bool operator==(Vector2D a, Vector2D b) noexcept {
    return a.x == b.x && a.y == b.y;
}

// Returns true when either component differs.
constexpr bool operator!=(Vector2D a, Vector2D b) noexcept {
    return !(a == b);
}

// Returns x^2 + y^2; avoids calling std::sqrt() when only comparing vector magnitudes.
constexpr float lengthSquared(Vector2D v) noexcept {
    return v.x * v.x + v.y * v.y;
}

// Returns the Euclidean length (magnitude) of v via Pythagoras: sqrt(x^2 + y^2).
inline float length(Vector2D v) noexcept {
    return std::sqrt(lengthSquared(v));
}

// Returns a unit-length vector pointing in the same direction as v, or (0, 0) if v is near zero.
inline Vector2D normalized(Vector2D v) noexcept {
    const float len = length(v);
    // Prevent division by zero (or NaN) when normalizing a zero-length vector.
    if (len <= 0.00001f) {
        return Vector2D{0.0f, 0.0f};
    }
    return v / len;
}

// Linearly interpolates between a (t = 0) and b (t = 1) for smooth render interpolation.
constexpr Vector2D lerp(Vector2D a, Vector2D b, float t) noexcept {
    return Vector2D{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

} // namespace engine

#endif
