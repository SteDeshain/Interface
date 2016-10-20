#ifndef __STEF_CORE_H__
#define __STEF_CORE_H__

#ifndef NULL
#define NULL    0
#endif

#include "precision.h"

#define PixelsPerMeter  64
#define ToPix(METER)    ((int)((METER) * (PixelsPerMeter)))
#define ToMeter(PIX)    (((stef::real)(PIX)) / ((stef::real)(PixelsPerMeter)))

namespace stef
{

class Vector3
{

public:
    real x;
    real y;
    real z;

    const static Vector3 ZERO;
    const static Vector3 GRAVITY;
    const static Vector3 HIGH_GRAVITY;
    const static Vector3 RIGHT;
    const static Vector3 LEFT;
    const static Vector3 FRONT;
    const static Vector3 BACK;
    const static Vector3 TOP;
    const static Vector3 BOTTOM;
//    const static Vector3 X;
//    const static Vector3 Y;
//    const static Vector3 Z;

private:
    //padding to ensure 4-word alignment
    real pad;

public:
    Vector3()
        :x(0), y(0), z(0)
    {}

    Vector3(const real x, const real y, const real z)
        :x(x), y(y), z(z)
    {}

    void Clear()
    {
        x = y = z = 0;
    }

    void Invert()
    {
        x *= -1;
        y *= -1;
        z *= -1;
    }

    real GetMagnitude() const
    {
        return real_sqrt(x * x + y * y + z * z);
    }

    real GetSquareMagnitude() const
    {
        return x * x + y * y + z * z;
    }

    void Normalize()
    {
        real l = GetMagnitude();
        if(l > 0)
        {
            (*this) *= ((real)1) / l;
        }
    }

    bool operator==(const Vector3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vector3& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }

    Vector3& operator*=(const real value)
    {
        x *= value;
        y *= value;
        z *= value;

        return *this;
    }

    Vector3 operator*(const real value) const
    {
        return Vector3(x * value, y * value, z * value);
    }

    friend Vector3 operator*(const real value, const Vector3& v)
    {
        return Vector3(v.x * value, v.y * value, v.z * value);
    }

    Vector3& operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3& operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    void AddScaledVector(const Vector3& v, real scale)
    {
        x += v.x * scale;
        y += v.y * scale;
        z += v.z * scale;
    }

    Vector3 ComponentProduct(const Vector3& v) const
    {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    void ComponentProductUpdate(const Vector3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    real operator*(const Vector3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 VectorProduct(const Vector3& v) const
    {
        return Vector3(y * v.z - z * v.y,
                       z * v.x - x * v.z,
                       x * v.y - y * v.x);
    }

    void VectorProductUpdate(const Vector3& v)
    {
        *this = VectorProduct(v);
    }

};

}

#endif // __STEF_CORE_H__
