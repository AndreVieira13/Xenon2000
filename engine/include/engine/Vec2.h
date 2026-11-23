// ============================================================================
// Vec2.h
// Lightweight 2D math vector shared across engine and game code so the game
// never has to depend on GLM or Box2D headers directly.
// ============================================================================
#ifndef ENGINE_VEC2_H
#define ENGINE_VEC2_H

#include <cmath>

namespace engine {

struct Vec2 {
    float x{0.0f};
    float y{0.0f};

    // Constructs a zero vector (0, 0).
    constexpr Vec2() = default;

    // Constructs a vector with the given horizontal (xVal) and vertical (yVal) components.
    constexpr Vec2(float xVal, float yVal) noexcept : x{xVal}, y{yVal} {}

    // Adds another vector component-wise into this vector.
    constexpr Vec2& operator+=(Vec2 other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Subtracts another vector component-wise from this vector.
    constexpr Vec2& operator-=(Vec2 other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Scales both components of this vector by a scalar factor.
    constexpr Vec2& operator*=(float scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Divides both components of this vector by a scalar factor.
    constexpr Vec2& operator/=(float scalar) noexcept {
        x /= scalar;
        y /= scalar;
        return *this;
    }
};

// Returns the component-wise sum of two vectors (a + b).
constexpr Vec2 operator+(Vec2 a, Vec2 b) noexcept {
    a += b;
    return a;
}

// Returns the component-wise difference of two vectors (a - b).
constexpr Vec2 operator-(Vec2 a, Vec2 b) noexcept {
    a -= b;
    return a;
}

// Returns the negated vector pointing in the exact opposite direction (-x, -y).
constexpr Vec2 operator-(Vec2 v) noexcept {
    return Vec2{-v.x, -v.y};
}

// Returns a vector scaled by s (vector * scalar form).
constexpr Vec2 operator*(Vec2 v, float s) noexcept {
    v *= s;
    return v;
}

// Returns a vector scaled by s (scalar * vector form).
constexpr Vec2 operator*(float s, Vec2 v) noexcept {
    v *= s;
    return v;
}

// Returns a vector divided by scalar s.
constexpr Vec2 operator/(Vec2 v, float s) noexcept {
    v /= s;
    return v;
}

// Returns true when both x and y components are equal.
constexpr bool operator==(Vec2 a, Vec2 b) noexcept {
    return a.x == b.x && a.y == b.y;
}

// Returns true when either component differs.
constexpr bool operator!=(Vec2 a, Vec2 b) noexcept {
    return !(a == b);
}

// Returns x^2 + y^2; avoids calling std::sqrt() when only comparing vector magnitudes.
constexpr float lengthSquared(Vec2 v) noexcept {
    return v.x * v.x + v.y * v.y;
}

// Returns the Euclidean length (magnitude) of v via Pythagoras: sqrt(x^2 + y^2).
inline float length(Vec2 v) noexcept {
    return std::sqrt(lengthSquared(v));
}

// Returns a unit-length vector pointing in the same direction as v, or (0, 0) if v is near zero.
inline Vec2 normalized(Vec2 v) noexcept {
    const float len = length(v);
    // Prevent division by zero (or NaN) when normalizing a zero-length vector.
    if (len <= 0.00001f) {
        return Vec2{0.0f, 0.0f};
    }
    return v / len;
}

// Linearly interpolates between a (t = 0) and b (t = 1) for smooth render interpolation.
constexpr Vec2 lerp(Vec2 a, Vec2 b, float t) noexcept {
    return Vec2{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

} // namespace engine

#endif
