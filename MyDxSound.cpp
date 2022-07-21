#include "MyDxSound.h"

DxSound::DxSound(Window win)
{
	HRESULT result = DirectSoundCreate8(NULL, &lpDS, NULL);

	result = lpDS->SetCooperativeLevel(win.hwnd, DSSCL_EXCLUSIVE | DSSCL_PRIORITY);

	WAVEFORMATEX wf;

	DSBUFFERDESC dsdesc;
	ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
	dsdesc.dwSize = sizeof(DSBUFFERDESC);
	dsdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsdesc.dwBufferBytes = 0;
	dsdesc.lpwfxFormat = NULL;
	result = lpDS->CreateSoundBuffer(&dsdesc, &lpPrimary, NULL);

	wf.cbSize = sizeof(WAVEFORMATEX);
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 2;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	result = lpPrimary->SetFormat(&wf);
}

DxSound::~DxSound()
{
    lpSound->Release();
    lpSound = NULL;

    lpPrimary->Release();
    lpPrimary = NULL;

    lpDS->Release();
    lpDS = NULL;
}

void DxSound::CreateSoundBuff(const char* filename)
{
    HRESULT result;
    MMCKINFO mSrcWaveFile;
    MMCKINFO mSrcWaveFmt;
    MMCKINFO mSrcWaveData;
    LPWAVEFORMATEX wf;

    // WAVファイルロード
    HMMIO hSrc;
    hSrc = mmioOpenA((LPSTR)filename, NULL, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);

    // "WAVE"チャンクチェック
    ZeroMemory(&mSrcWaveFile, sizeof(mSrcWaveFile));
    result = mmioDescend(hSrc, &mSrcWaveFile, NULL, MMIO_FINDRIFF);
    if (mSrcWaveFile.fccType != mmioFOURCC('W', 'A', 'V', 'E')) {
        mmioClose(hSrc, 0);
    }

    // "fmt "チャンクチェック
    ZeroMemory(&mSrcWaveFmt, sizeof(mSrcWaveFmt));
    result = mmioDescend(hSrc, &mSrcWaveFmt, &mSrcWaveFile, MMIO_FINDCHUNK);
    if (mSrcWaveFmt.ckid != mmioFOURCC('f', 'm', 't', ' ')) {
        mmioClose(hSrc, 0);
    }

    // ヘッダサイズの計算
    int iSrcHeaderSize = mSrcWaveFmt.cksize;
    if (iSrcHeaderSize < sizeof(WAVEFORMATEX))
        iSrcHeaderSize = sizeof(WAVEFORMATEX);

    // ヘッダメモリ確保
    wf = (LPWAVEFORMATEX)malloc(iSrcHeaderSize);
    if (!wf) {
        mmioClose(hSrc, 0);
    }
    ZeroMemory(wf, iSrcHeaderSize);

    // WAVEフォーマットのロード
    result = mmioRead(hSrc, (char*)wf, mSrcWaveFmt.cksize);
    if (FAILED(result)) {
        free(wf);
        mmioClose(hSrc, 0);
    }

    // fmtチャンクに戻る
    mmioAscend(hSrc, &mSrcWaveFmt, 0);

    // dataチャンクを探す
    while (1) {
        // 検索
        result = mmioDescend(hSrc, &mSrcWaveData, &mSrcWaveFile, 0);
        if (FAILED(result)) {
            free(wf);
            mmioClose(hSrc, 0);
        }
        if (mSrcWaveData.ckid == mmioStringToFOURCCA("data", 0))
            break;
        // 次のチャンクへ
        result = mmioAscend(hSrc, &mSrcWaveData, 0);
    }

    // サウンドバッファの作成
    DSBUFFERDESC dsdesc;
    ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
    dsdesc.dwSize = sizeof(DSBUFFERDESC);
    dsdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC | DSBCAPS_LOCDEFER;
    dsdesc.dwBufferBytes = mSrcWaveData.cksize;
    dsdesc.lpwfxFormat = wf;
    dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    result = lpDS->CreateSoundBuffer(&dsdesc, &lpSound, NULL);
    if (FAILED(result)) {
        free(wf);
        mmioClose(hSrc, 0);
    }

    // ロック開始
    LPVOID pMem1, pMem2;
    DWORD dwSize1, dwSize2;
    result = lpSound->Lock(0, mSrcWaveData.cksize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
    if (FAILED(result)) {
        free(wf);
        mmioClose(hSrc, 0);
    }

    // データ書き込み
    mmioRead(hSrc, (char*)pMem1, dwSize1);
    mmioRead(hSrc, (char*)pMem2, dwSize2);

    // ロック解除
    lpSound->Unlock(pMem1, dwSize1, pMem2, dwSize2);

    // ヘッダ用メモリを開放
    free(wf);

    // WAVを閉じる
    mmioClose(hSrc, 0);
}

void DxSound::PlaySoundWav()
{
    lpSound->SetCurrentPosition(0);
    lpSound->Play(0, 0, 0);
}
