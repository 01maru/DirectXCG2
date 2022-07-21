#include "MyDebugCamera.h"

MyDebugCamera::MyDebugCamera(Vector3D _eye, Vector3D _target, Vector3D _up)
{
	Init(_eye, _target, _up);
}

void MyDebugCamera::Update(Input& input)
{
	prevCursor = cursor;
	input.CursorPos(cursor);
	moveCursor = cursor - prevCursor;
	moveCursor.normalize();

	if (input.Click(Input::LeftClick)) {
		moveCursor /= 50;
		if (up.y < 0) {
			//moveCursor.x = -moveCursor.x;
		}
		cursorSpd += moveCursor;
	}
	
	frontVec = target - eye;
	disEyeTarget = frontVec.length();
	frontVec.normalize();
	up.y = cosf(cursorSpd.y);
	eye.z = -50 * sinf(cursorSpd.x) * cosf(cursorSpd.y);
	eye.y =  50 * sinf(cursorSpd.y);
	eye.x = -50 * cosf(cursorSpd.x) * cosf(cursorSpd.y);
	MatUpdate();
}

void MyDebugCamera::Init(Vector3D _eye, Vector3D _target, Vector3D _up)
{
	eye = _eye;
	target = _target;
	up = _up;

	MatUpdate();
}

void MyDebugCamera::MatUpdate()
{
	mat = MyMath::LookAtLH(eye, target, up);
}
