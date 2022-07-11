#pragma once
#include "Vector3D.h"
class Vector4D
{
public:
	float x;
	float y;
	float z;
	float w;
public:
	Vector4D();
	Vector4D(float x, float y, float z, float w);
	Vector4D(Vector3D v3d, float w);

	float length() const;
	Vector4D& normalize();

	Vector4D operator+() const;

	Vector4D operator-() const;

	Vector4D& operator-=(const Vector4D& v);

	Vector4D& operator/=(float s);
};

const Vector4D operator-(const Vector4D& v1, const Vector4D& v2);