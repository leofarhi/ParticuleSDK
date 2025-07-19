#ifndef VECTOR2_HPP
#define VECTOR2_HPP

namespace Particule::Core
{
    template <typename T>
    class Vector2
    {
    public:
        T x, y;
        Vector2(T x, T y) : x(x), y(y) {}
        Vector2() : x(0), y(0) {}

        T length() const
        {
            return sqrt(x * x + y * y);
        }
        Vector2 normalize()
        {
            T len = length();
            if (len == 0)
                return Vector2(0, 0);
            return Vector2(x / len, y / len);
        }

        template<typename U>
        Vector2(const Vector2<U>& other)
            : x(static_cast<T>(other.x)), 
            y(static_cast<T>(other.y)) {}

        Vector2 operator+(const Vector2 &v) const
        {
            return Vector2(x + v.x, y + v.y);
        }
        Vector2 operator-(const Vector2 &v) const
        {
            return Vector2(x - v.x, y - v.y);
        }
        Vector2 operator*(const T &s) const
        {
            return Vector2(x * s, y * s);
        }
        Vector2 operator/(const T &s) const
        {
            return Vector2(x / s, y / s);
        }
        Vector2 &operator+=(const Vector2 &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        Vector2 &operator-=(const Vector2 &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        Vector2 &operator*=(const T &s)
        {
            x *= s;
            y *= s;
            return *this;
        }
        Vector2 &operator/=(const T &s)
        {
            x /= s;
            y /= s;
            return *this;
        }
        Vector2 operator-() const
        {
            return Vector2(-x, -y);
        }
        bool operator==(const Vector2 &v) const
        {
            return x == v.x && y == v.y;
        }
        bool operator!=(const Vector2 &v) const
        {
            return x != v.x || y != v.y;
        }
    };
}

#endif // VECTOR2_HPP