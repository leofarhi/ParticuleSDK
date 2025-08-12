#ifndef VECTOR2_HPP
#define VECTOR2_HPP
#include <cmath>
#include <type_traits>

namespace Particule::Core
{
    
    template <typename T>
    class Vector2 {
    public:
        T x{}, y{};

        // Ctors
        constexpr Vector2() = default;
        constexpr Vector2(T X, T Y) : x(X), y(Y) {}

        template<typename U>
        constexpr explicit Vector2(const Vector2<U>& o)
            : x(static_cast<T>(o.x)), y(static_cast<T>(o.y)) {}

        // Longueur / normalisation
        T length() const {
            using std::sqrt;
            return static_cast<T>(sqrt(x*x + y*y));
        }

        Vector2 normalized() const {
            T len = length();
            if (len == T{}) return Vector2{};
            return Vector2(x/len, y/len);
        }

        // Ops avec scalaire
        constexpr Vector2 operator*(const T& s) const { return { x*s, y*s }; }
        constexpr Vector2 operator/(const T& s) const { return { x/s, y/s }; }
        constexpr Vector2& operator*=(const T& s) { x*=s; y*=s; return *this; }
        constexpr Vector2& operator/=(const T& s) { x/=s; y/=s; return *this; }

        // Ops vectoriels (classiques)
        constexpr Vector2& operator+=(const Vector2& v) { x+=v.x; y+=v.y; return *this; }
        constexpr Vector2& operator-=(const Vector2& v) { x-=v.x; y-=v.y; return *this; }

        constexpr Vector2 operator-() const { return { -x, -y }; }

        constexpr bool operator==(const Vector2& v) const { return x==v.x && y==v.y; }
        constexpr bool operator!=(const Vector2& v) const { return !(*this==v); }

        // Produits “math”
        constexpr T dot(const Vector2& v) const { return x*v.x + y*v.y; }
        // "cross" 2D = composante Z du cross 3D (utile pour aire/sens)
        constexpr T cross(const Vector2& v) const { return x*v.y - y*v.x; }

        // Perp (rotation 90°)
        constexpr Vector2 perp() const { return { -y, x }; }
    };

    // ——— free functions (ADL) ———

    // + / - (deux arguments)
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> operator+(Vector2<T> a, const Vector2<T>& b) { a += b; return a; }

    template<typename T>
    [[nodiscard]] constexpr Vector2<T> operator-(Vector2<T> a, const Vector2<T>& b) { a -= b; return a; }

    // Hadamard (composant-par-composant) vec * vec et vec / vec
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> hadamard_mul(const Vector2<T>& a, const Vector2<T>& b) {
        return { a.x*b.x, a.y*b.y };
    }
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> hadamard_div(const Vector2<T>& a, const Vector2<T>& b) {
        return { a.x/b.x, a.y/b.y };
    }

    // Surcharges directes
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> operator*(const Vector2<T>& a, const Vector2<T>& b) {
        return hadamard_mul(a,b);
    }
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> operator/(const Vector2<T>& a, const Vector2<T>& b) {
        return hadamard_div(a,b);
    }

    // Commutativité scalaire*vec
    template<typename T>
    [[nodiscard]] constexpr Vector2<T> operator*(const T& s, const Vector2<T>& v) { return { v.x*s, v.y*s }; }
}

#endif // VECTOR2_HPP