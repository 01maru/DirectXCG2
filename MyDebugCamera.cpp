#include "MyDebugCamera.h"

MyDebugCamera::MyDebugCamera(Vector3D _eye, Vector3D _target, Vector3D _up)
{
	Init(_eye, _target, _up);
	frontVec = target - eye;
	disEyeTarget = frontVec.length();
}

void MyDebugCamera::Update(Input& input)
{
	prevCursor = cursor;
	input.CursorPos(cursor);
	moveCursor = cursor - prevCursor;
	float cursorDisPrev = moveCursor.length();
	moveCursor.normalize();

	if (input.Click(Input::LeftClick)) {
		moveCursor /= 1000;
		moveCursor *= cursorDisPrev;
		if (up.y < 0) {
			moveCursor.x = -moveCursor.x;
		}
		cursorSpd += moveCursor;
	}
	disEyeTarget += -input.Wheel() * (disEyeTarget * 0.001f);
	if (disEyeTarget < 10) {
		disEyeTarget = 10;
	}
	target += rightVec * (input.GetKey(DIK_RIGHT) - input.GetKey(DIK_LEFT));
	target += downVec * (input.GetKey(DIK_DOWN) - input.GetKey(DIK_UP));
	target += -frontVec * (input.GetKey(DIK_Z) - input.GetKey(DIK_X));

	frontVec = target - eye;
	frontVec.normalize();
	rightVec = Vector3D(0, 1, 0).cross(frontVec);
	downVec = rightVec.cross(frontVec);

	up.y = cosf(cursorSpd.y);
	eye.x = target.x - disEyeTarget * cosf(cursorSpd.y) * sinf(cursorSpd.x);
	eye.y = target.y + disEyeTarget * sinf(cursorSpd.y);
	eye.z = target.z - disEyeTarget * cosf(cursorSpd.y) * cosf(cursorSpd.x);
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
