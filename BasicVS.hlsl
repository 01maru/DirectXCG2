float4 main(float4 pos : POSITION) : SV_POSITION
{
	//return pos + float4(1,1,0,0);	//	X�I�t�Z�b�g,Y�I�t�Z�b�g
	return pos * float4(0.5,0.5,1,1);	//	X�{��,Y�{��
}