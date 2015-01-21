#pragma once

#include <Windows.h>

//Class defining window handle for MS Windows
class WindowHandle
{
public:
	WindowHandle(void* handle);
	const void* const getRawHandle() const;

protected:
	HWND _hWND;
};