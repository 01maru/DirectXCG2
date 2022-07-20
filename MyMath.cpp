#include "MyMath.h"
#include <cassert>

Matrix MyMath::LookAtLH(const Vector3D& eye, const Vector3D& target, const Vector3D& up)
{
	//Vector3D eye(0, 0, -100);	//	視点座標
	//Vector3D target(0, 0, 0);	//	注視点座標
	//Vector3D up(0, 1, 0);		//	上方向ベクトル

	Matrix mat2;
	mat2.m[0][3] = -eye.x;
	mat2.m[1][3] = -eye.y;
	mat2.m[2][3] = -eye.z;

	Vector3D baseX = up;
	Vector3D baseY;
	Vector3D baseZ = target - eye;

	baseZ.normalize();

	baseX = baseX.cross(baseZ);
	baseX.normalize();

	baseY = baseZ;
	baseY = baseY.cross(baseX);

	Matrix matView;

	matView.m[0][0] = baseX.x;
	matView.m[0][1] = baseX.y;
	matView.m[0][2] = baseX.z;

	matView.m[1][0] = baseY.x;
	matView.m[1][1] = baseY.y;
	matView.m[1][2] = baseY.z;

	matView.m[2][0] = baseZ.x;
	matView.m[2][1] = baseZ.y;
	matView.m[2][2] = baseZ.z;

	matView.m[3][0] = -baseX.dot(eye);
	matView.m[3][1] = -baseY.dot(eye);
	matView.m[3][2] = -baseZ.dot(eye);

	return matView;
}

float MyMath::ConvertToRad(float angle)
{
	return angle / 180.0f * PI;
}

Matrix MyMath::PerspectiveFovLH(const int winwidth, const int winheight, float fovY, float nearZ, float farZ)
{
	assert(nearZ > 0.f && farZ > 0.f);
	//assert(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
	//assert(!XMScalarNearEqual(AspectRatio, 0.0f, 0.00001f));
	//assert(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

	float aspect = (float)winwidth / winheight;

	//float tan = (float)sin(fovY/2.0f) / cos(fovY/2.0f) * aspect;
	float height = 1.0f / tanf(fovY / 2.0f);

	Matrix matProjection;
	matProjection.Identity();
	matProjection.m[0][0] = height;
	matProjection.m[1][1] = height * aspect;
	matProjection.m[2][2] = (float)(farZ + nearZ) / (farZ - nearZ);
	matProjection.m[2][3] = 1.0f;
	matProjection.m[3][2] = -2.0f * farZ * nearZ / (float)(farZ - nearZ);
	matProjection.m[3][3] = 0.0f;

	return matProjection;
}

//MyMath::MatView::MatView(Vector3D _eye, Vector3D _target, Vector3D _up)
//{
//	Init(_eye, _target, _up);
//}

MyMath::MatView::MatView()
{
	Init(Vector3D(0.0f, 0.0f, -100.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
}

void MyMath::MatView::Init(Vector3D _eye, Vector3D _target, Vector3D _up)
{
	eye = _eye;
	target = _target;
	up = _up;

	MatUpdate();
}

void MyMath::MatView::MatUpdate()
{
	mat = LookAtLH(eye, target, up);
}

MyMath::float4::float4() :
	x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

MyMath::float4::float4(float x, float y, float z, float w) :
	x(x), y(y), z(z), w(w)
{
}
