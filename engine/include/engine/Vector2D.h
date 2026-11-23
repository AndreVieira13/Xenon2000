// ============================================================================
// Vector2D.h
// Lightweight 2D math vector shared across engine and game code so the game
// never has to depend on GLM or Box2D headers directly.
// Declarations live here; definitions live in Vector2D.cpp.
// ============================================================================
#ifndef ENGINE_VECTOR2D_H
#define ENGINE_VECTOR2D_H

namespace engine {

struct Vector2D {
    float x{0.0f};
    float y{0.0f};

    // Constructs a zero vector (0, 0).
    constexpr Vector2D() = default;

    // Constructs a vector with the given horizontal (xVal) and vertical (yVal) components.
    constexpr Vector2D(float xVal, float yVal) noexcept : x{xVal}, y{yVal} {}

    // Adds another vector component-wise into this vector.
    Vector2D& operator+=(Vector2D other) noexcept;

    // Subtracts another vector component-wise from this vector.
    Vector2D& operator-=(Vector2D other) noexcept;

    // Scales both components of this vector by a scalar factor.
    Vector2D& operator*=(float scalar) noexcept;

    // Divides both components of this vector by a scalar factor.
    Vector2D& operator/=(float scalar) noexcept;
};

// Returns the component-wise sum of two vectors (a + b).
Vector2D operator+(Vector2D a, Vector2D b) noexcept;

// Returns the component-wise difference of two vectors (a - b).
Vector2D operator-(Vector2D a, Vector2D b) noexcept;

// Returns the negated vector pointing in the exact opposite direction (-x, -y).
Vector2D operator-(Vector2D v) noexcept;

// Returns a vector scaled by s (vector * scalar form).
Vector2D operator*(Vector2D v, float s) noexcept;

// Returns a vector scaled by s (scalar * vector form).
Vector2D operator*(float s, Vector2D v) noexcept;

// Returns a vector divided by scalar s.
Vector2D operator/(Vector2D v, float s) noexcept;

// Returns true when both x and y components are equal.
bool operator==(Vector2D a, Vector2D b) noexcept;

// Returns true when either component differs.
bool operator!=(Vector2D a, Vector2D b) noexcept;

// Returns x^2 + y^2; avoids calling std::sqrt() when only comparing vector magnitudes.
float lengthSquared(Vector2D v) noexcept;

// Returns the Euclidean length (magnitude) of v via Pythagoras: sqrt(x^2 + y^2).
float length(Vector2D v) noexcept;

// Returns a unit-length vector pointing in the same direction as v, or (0, 0) if v is near zero.
Vector2D normalized(Vector2D v) noexcept;

// Linearly interpolates between a (t = 0) and b (t = 1) for smooth render interpolation.
Vector2D lerp(Vector2D a, Vector2D b, float t) noexcept;

} // namespace engine

#endif
