#pragma once
#include "Vector3D.h"
#include "Matrix.h"
const float PI = 3.14159265358979f;

//	視点座標、注視点座標、上方向ベクトル
Matrix LookAtLH(const Vector3D& eye, const Vector3D& target, const Vector3D& up);

Matrix PerspectiveFovLH(const int winwidth, const int winheight, float fovY, float nearZ, float farZ);

float ConvertToRad(float angle);

class MatView{
public:
	Matrix mat;
	Vector3D eye;		//	視点座標
	Vector3D target;	//	注視点座標
	Vector3D up;		//	上方向ベクトル
public:
	MatView(Vector3D _eye, Vector3D _target, Vector3D _up);
	void MatUpdate();
};