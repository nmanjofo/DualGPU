#pragma once

#include <Windows.h>

#include "Thread.hpp"
#include "Error.hpp"

//Threaded window manager
//
class WindowManager : public Thread, public Error
{
public:
	WindowManager();

	bool init(unsigned int width, unsigned int height, bool isResizable);
	void hide();
	void show();
	bool isValid();

	void setIsWindowResizeable(bool isResizable);
	bool getIsWindowResizable();

	void setTitle();

	HWND getHandle();
	
protected:
	//Members
	HWND	_hwnd;
	bool	_isValid;
	bool	_isVisible;
	bool	_isHasFocus;

	//Callback functions

};