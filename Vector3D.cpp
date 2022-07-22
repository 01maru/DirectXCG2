#include "Vector3D.h"

#include <cmath>

Vector3D::Vector3D()
	:x(0), y(0), z(0)
{
}

Vector3D::Vector3D(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

float Vector3D::length() const
{
	return sqrt(x * x + y * y + z * z);
}

Vector3D& Vector3D::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector3D::dot(const Vector3D& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D::cross(const Vector3D& v) const
{
	return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector3D Vector3D::operator+() const
{
	return *this;
}

Vector3D Vector3D::operator-() const
{
	return Vector3D(-x, -y, -z);
}

Vector3D& Vector3D::operator+=(const Vector3D& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3D& Vector3D::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vector3D& Vector3D::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

const Vector3D operator-(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D temp(v1);
	return temp -= v2;
}

const Vector3D operator*(const Vector3D& v, float s)
{
	Vector3D temp(v);
	return temp *= s;
}

const Vector3D operator*(float s, const Vector3D& v)
{
	return v * s;
}

Vector3D CreatePolygonNormal(Vector3D a, Vector3D b, Vector3D c)
{
	Vector3D AB(b - a);
	Vector3D BC(c - b);

	Vector3D normal = AB;
	normal.cross(BC);
	normal.normalize();

	return normal;
}
