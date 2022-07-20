#pragma once
#include "MyMath.h"
#include "Input.h"
class MyDebugCamera
{
public:
	Matrix mat;
	Vector3D eye;		//	���_���W
	Vector3D target;	//	�����_���W
	Vector3D up;		//	������x�N�g��
private:
	float disEyeTarget;
	Vector3D frontVec;
	Vector2D cursor;
	Vector2D prevCursor;
	Vector2D moveCursor;
	Vector2D cursorSpd;
public:
	MyDebugCamera(Vector3D _eye, Vector3D _target, Vector3D _up);
	void Update(Input& input);
	void Init(Vector3D _eye, Vector3D _target, Vector3D _up);
	void MatUpdate();
};

