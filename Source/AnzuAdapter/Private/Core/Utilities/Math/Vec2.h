#pragma once

namespace anzu
{
    class Vec2
    {
    public:
        float x, y;

        explicit Vec2(float _x = 0, float _y = 0) : x(_x), y(_y)
        {}

        Vec2& operator=(const Vec2& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            x = rhs.x;
            y = rhs.y;

            return *this;
        }

        Vec2 operator*(float scalar) const
        {
            return Vec2(x * scalar, y * scalar);
        }

        Vec2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;

            return *this;
        }

        Vec2 operator/(float scalar) const
        {
            return Vec2(x / scalar, y / scalar);
        }

        Vec2& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;

            return *this;
        }

        Vec2 operator-() const
        {
            return Vec2(-x, -y);
        }

        bool operator==(const Vec2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        bool operator!=(const Vec2& rhs) const
        {
            return !( *this == rhs );
        }

        Vec2 operator+(const Vec2& rhs) const
        {
            return Vec2(x + rhs.x, y + rhs.y);
        }

        Vec2& operator+=(const Vec2& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vec2 operator-(const Vec2& rhs) const
        {
            return Vec2(x - rhs.x, y - rhs.y);
        }

        Vec2& operator-=(const Vec2& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        Vec2 operator*(const Vec2& other) const
        {
            return Vec2(x * other.x, y * other.y);
        }

        Vec2& operator*=(const Vec2& other)
        {
            x *= other.x;
            y *= other.y;

            return *this;
        }
    };
}
