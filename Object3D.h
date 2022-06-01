#pragma once
#include "ConstBuff.h"
class Object3D
{
public:
	XMMATRIX matWorld;
	XMMATRIX matScale;
	XMMATRIX matRot;
	XMMATRIX matTrans;

public:
	Object3D();
	void Update();
	void Draw();
};

