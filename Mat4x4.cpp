#include "Mat4x4.h"

Mat4x4::Mat4x4()
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
