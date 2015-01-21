#include "WindowHandle.hpp"

WindowHandle::WindowHandle(void* handle)
{
	_hWND = *(HWND*)handle;
}

const void* const WindowHandle::getRawHandle() const
{
	return &_hWND;
}