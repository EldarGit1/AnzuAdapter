#pragma once

namespace anzu
{
    class Vec3
    {
    public:
        float x, y, z;

        explicit Vec3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z)
        {}

        Vec3& operator=(const Vec3& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            x = rhs.x;
            y = rhs.y;
            z = rhs.z;

            return *this;
        }

        Vec3 operator*(float scalar) const
        {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }

        Vec3& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;

            return *this;
        }

        Vec3 operator/(float scalar) const
        {
            return Vec3(x / scalar, y / scalar, z / scalar);
        }

        Vec3& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;

            return *this;
        }

        Vec3 operator-() const
        {
            return Vec3(-x, -y, -z);
        }

        bool operator==(const Vec3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        bool operator!=(const Vec3& rhs) const
        {
            return !( *this == rhs );
        }

        Vec3 operator+(const Vec3& rhs) const
        {
            return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vec3& operator+=(const Vec3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;

            return *this;
        }

        Vec3 operator-(const Vec3& rhs) const
        {
            return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        Vec3& operator-=(const Vec3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;

            return *this;
        }

        Vec3 operator*(const Vec3& other) const
        {
            return Vec3(x * other.x, y * other.y, z * other.z);
        }

        Vec3& operator*=(const Vec3& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;

            return *this;
        }
    };
}
