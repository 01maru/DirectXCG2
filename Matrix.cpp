#include "Matrix.h"

Matrix::Matrix()
{
	Identity();
}

void Matrix::Identity()
{
	for (size_t x = 0; x < sizeof(m); x++)
	{
		for (size_t y = 0; y < sizeof(m[0]); y++)
		{
			if (x == y) m[y][x] = 1;
			else		m[y][x] = 0;
		}
	}
}

Matrix& Matrix::operator*=(const Matrix& m2)
{
	Matrix prev(*this);

	for (size_t y = 0; y < 4; y++)
	{
		for (size_t x = 0; x < 4; x++)
		{
			m[y][x] = prev.m[y][0] * m2.m[0][x] + prev.m[y][1] * m2.m[1][x] + prev.m[y][2] * m2.m[2][x] + prev.m[y][3] * m2.m[3][x];
		}
	}

	return *this;
}