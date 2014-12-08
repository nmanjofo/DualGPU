#pragma once

#include <d3d11.h>


#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

//Global variables
void initGlobals();
void deleteGlobals();

extern ID3D11Device*             g_pDevice;
extern ID3D11DeviceContext*      g_pContext;

