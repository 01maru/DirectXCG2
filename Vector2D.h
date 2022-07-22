#pragma once
class Vector2D
{
public:
	float x;
	float y;
public:
	Vector2D();
	Vector2D(float x, float y);

	Vector2D operator+() const;
	Vector2D& operator+=(const Vector2D& v);
	Vector2D& operator-=(const Vector2D& v);

	float length() const;

	Vector2D& normalize();
	Vector2D& operator/=(float s);
	Vector2D& operator*=(float s);
};

const Vector2D operator-(const Vector2D& v1, const Vector2D& v2);
const Vector2D operator*(const Vector2D& v, float s);
const Vector2D operator*(float s, const Vector2D& v);
