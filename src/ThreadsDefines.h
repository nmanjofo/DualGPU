#pragma once

#include <Windows.h>
#include <d3d11.h>


typedef struct _tThreadParam
{
	UINT masterThreadID;
	IDXGIAdapter* adapter;
	unsigned int texWidth;
	unsigned int texHeight;
} threadParam;


enum ThreadMessage
{
	TMSG_TERMINATE = WM_USER + 1,
	TMSG_SLAVE_INIT_FAILED = WM_USER + 2,
	TMSG_SLAVE_INIT_OK = WM_USER + 3,
	TMSG_SLAVE_DATA_READY = WM_USER + 4,
	TMSG_SLAVE_QUIT = WM_USER + 5,
	TMSG_SLAVE_ID = WM_USER + 6,
};