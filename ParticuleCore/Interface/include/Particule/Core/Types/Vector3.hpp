#ifndef VECTOR3_HPP
#define VECTOR3_HPP

namespace Particule::Core
{
    template <typename T>
    class Vector3
    {
    public:
        T x, y, z;
        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        Vector3() : x(0), y(0), z(0) {}

        T length() const
        {
            return sqrt(x * x + y * y + z * z);
        }
        Vector3 normalize()
        {
            T len = length();
            if (len == 0)
                return Vector3(0, 0, 0);
            return Vector3(x / len, y / len, z / len);
        }
        Vector3 cross(const Vector3 &v) const
        {
            return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
        }
        T dot(const Vector3 &v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        template<typename U>
        Vector3(const Vector3<U>& other)
            : x(static_cast<T>(other.x)), 
            y(static_cast<T>(other.y)), 
            z(static_cast<T>(other.z)) {}

        Vector3 operator+(const Vector3 &v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }
        Vector3 operator-(const Vector3 &v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }
        Vector3 operator*(const T &s) const
        {
            return Vector3(x * s, y * s, z * s);
        }
        Vector3 operator/(const T &s) const
        {
            return Vector3(x / s, y / s, z / s);
        }
        Vector3 &operator+=(const Vector3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        Vector3 &operator-=(const Vector3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        Vector3 &operator*=(const T &s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }
        Vector3 &operator/=(const T &s)
        {
            x /= s;
            y /= s;
            z /= s;
            return *this;
        }
        Vector3 operator-() const
        {
            return Vector3(-x, -y, -z);
        }
        bool operator==(const Vector3 &v) const
        {
            return x == v.x && y == v.y && z == v.z;
        }
        bool operator!=(const Vector3 &v) const
        {
            return x != v.x || y != v.y || z != v.z;
        }
    };
}
#endif // VECTOR3_HPP