#pragma once
#include "Window.h"
#include <dsound.h>

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"winmm.lib")

class DxSound
{
private:
	LPDIRECTSOUND8 lpDS = NULL;
	LPDIRECTSOUNDBUFFER lpPrimary = NULL;
	LPDIRECTSOUNDBUFFER lpSound = NULL;
public:
	DxSound(Window win);
	~DxSound();
	void CreateSoundBuff(const char* filename);
	void PlaySoundWav();
};

