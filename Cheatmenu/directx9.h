#pragma once

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);

static HWND window;
bool GetD3D9Device(void** pTable, size_t size);
HWND GetProcessWindow();

extern LPDIRECT3DDEVICE9 pDevice;
extern int windowHeight, windowWidth;