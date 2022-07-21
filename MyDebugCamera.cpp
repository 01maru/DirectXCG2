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
	rightVec = Vector3D(0, 1, 0).cross(frontVec);
	downVec = rightVec.cross(frontVec);

	target += -rightVec * (input.GetKey(DIK_RIGHT) - input.GetKey(DIK_LEFT));
	target += -downVec * (input.GetKey(DIK_DOWN) - input.GetKey(DIK_UP));
	target += -frontVec * (input.GetKey(DIK_Z) - input.GetKey(DIK_X));

	up.y = cosf(cursorSpd.y);
	eye.x = target.x -50 * sinf(cursorSpd.x) * cosf(cursorSpd.y);
	eye.y = target.y+ 50 * sinf(cursorSpd.y);
	eye.z = target.z -50 * cosf(cursorSpd.x) * cosf(cursorSpd.y);
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
