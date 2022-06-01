#pragma once
class Matrix
{
public:
	int m[4][4];
public:
	Matrix();
	void Identity();
	Matrix& operator*=(const Matrix& m2)
};

