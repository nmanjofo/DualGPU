#include "globals.h"

void initGlobals()
{
	g_pDevice = nullptr;
	g_pContext = nullptr;
}

void deleteGlobals()
{
	SAFE_RELEASE(g_pContext);
	SAFE_RELEASE(g_pDevice);
}