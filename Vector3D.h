#pragma once
class Vector3D
{
public:
	float x;
	float y;
	float z;
public:
	Vector3D();
	Vector3D(float x, float y, float z);

	float length() const;

	Vector3D& normalize();
	float dot(const Vector3D& v) const;
	Vector3D cross(const Vector3D& v) const;

	Vector3D operator+() const;

	Vector3D operator-() const;

	Vector3D& operator-=(const Vector3D& v);

	Vector3D& operator/=(float s);
};

const Vector3D operator-(const Vector3D& v1, const Vector3D& v2);
