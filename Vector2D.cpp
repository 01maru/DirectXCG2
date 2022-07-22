#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D() :
	x(0), y(0)
{
}

Vector2D::Vector2D(float x, float y) :
	x(x), y(y)
{
}

Vector2D Vector2D::operator+() const
{
	return *this;
}

Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

float Vector2D::length() const
{
	return sqrt(x * x + y * y);
}

Vector2D& Vector2D::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

Vector2D& Vector2D::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

Vector2D& Vector2D::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

const Vector2D operator-(const Vector2D& v1, const Vector2D& v2)
{
	Vector2D temp(v1);
	return temp -= v2;
}

const Vector2D operator*(const Vector2D& v, float s)
{
	Vector2D temp(v);
	return temp *= s;
}

const Vector2D operator*(float s, const Vector2D& v)
{
	return v * s;
}
