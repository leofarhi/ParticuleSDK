#ifndef VECTOR3_HPP
#define VECTOR3_HPP
#include <cmath>
    #include <type_traits>

namespace Particule::Core
{
    template <typename T>
    class Vector3 {
    public:
        T x{}, y{}, z{};

        constexpr Vector3() = default;
        constexpr Vector3(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

        template<typename U>
        constexpr explicit Vector3(const Vector3<U>& o)
            : x(static_cast<T>(o.x)), y(static_cast<T>(o.y)), z(static_cast<T>(o.z)) {}

        // Norme / normalisation
        T length() const { using std::sqrt; return static_cast<T>(sqrt(x*x + y*y + z*z)); }

        Vector3 normalized() const {
            T len = length();
            if (len == T{}) return Vector3{};
            return Vector3(x/len, y/len, z/len);
        }

        // Ops avec scalaire
        constexpr Vector3 operator*(const T& s) const { return {x*s, y*s, z*s}; }
        constexpr Vector3 operator/(const T& s) const { return {x/s, y/s, z/s}; }
        constexpr Vector3& operator*=(const T& s) { x*=s; y*=s; z*=s; return *this; }
        constexpr Vector3& operator/=(const T& s) { x/=s; y/=s; z/=s; return *this; }

        // Ops vectoriels classiques
        constexpr Vector3& operator+=(const Vector3& v){ x+=v.x; y+=v.y; z+=v.z; return *this; }
        constexpr Vector3& operator-=(const Vector3& v){ x-=v.x; y-=v.y; z-=v.z; return *this; }
        constexpr Vector3 operator-() const { return {-x,-y,-z}; }

        constexpr bool operator==(const Vector3& v) const { return x==v.x && y==v.y && z==v.z; }
        constexpr bool operator!=(const Vector3& v) const { return !(*this==v); }

        // Produits “math”
        constexpr T      dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
        constexpr Vector3 cross(const Vector3& v) const {
            return { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x };
        }
    };

    // ——— free functions (ADL), nécessaires pour avoir 2 arguments ———
    template<typename T>
    [[nodiscard]] constexpr Vector3<T> operator+(Vector3<T> a, const Vector3<T>& b){ a += b; return a; }

    template<typename T>
    [[nodiscard]] constexpr Vector3<T> operator-(Vector3<T> a, const Vector3<T>& b){ a -= b; return a; }

    // Hadamard (composant-par-composant) vec * vec et vec / vec
    template<typename T>
    [[nodiscard]] constexpr Vector3<T> hadamard_mul(const Vector3<T>& a, const Vector3<T>& b){
        return { a.x*b.x, a.y*b.y, a.z*b.z };
    }
    template<typename T>
    [[nodiscard]] constexpr Vector3<T> hadamard_div(const Vector3<T>& a, const Vector3<T>& b){
        return { a.x/b.x, a.y/b.y, a.z/b.z };
    }

    // Si tu tiens à la surcharge directe:
    template<typename T>
    [[nodiscard]] constexpr Vector3<T> operator*(const Vector3<T>& a, const Vector3<T>& b){
        return hadamard_mul(a,b);
    }
    template<typename T>
    [[nodiscard]] constexpr Vector3<T> operator/(const Vector3<T>& a, const Vector3<T>& b){
        return hadamard_div(a,b);
    }

}
#endif // VECTOR3_HPP