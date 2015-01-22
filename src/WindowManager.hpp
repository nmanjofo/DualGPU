#pragma once

#include "Thread.hpp"
#include "Error.hpp"
#include "WindowHandle.hpp"
#include "KMInput.hpp"


//Threaded window manager
class WindowManager : public Thread, public Error
{
public:

	//Flags used for window initialization
	enum class WindowFlags : unsigned int
	{

	};

	WindowManager();
	~WindowManager();

	bool createWindow(unsigned int width, unsigned int height, const wchar_t* className, const wchar_t* title, unsigned int flags);

	bool isValid(){ return _isValid; }

	void setTitle(const wchar_t* title);
	void setCursorVisible(bool isCursorVisible);

	const WindowHandle* const getHandle() const;

	//ThreadMain from Thread class, implements message loop
	void threadMain(void* arguments) override;

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	
	void setWindowSize(unsigned int width, unsigned int height);
	void setWindowPosition(unsigned int x, unsigned int y);

	//Sets keys, which will be accepted for input


	//Resets key settings, calls back all keys
	void acceptAllKeys();

	//Musi vediet zamykat vstup z mysi a klavesnice
	//Posielat spravy o zmene velkosti
	void minimize();
	void restore();

	bool isMinimized();
	bool isHasFocus();

	void resize(unsigned int width, unsigned int height);

	//Active window = visible (not minimized) and has focus (= accepting input messages)
	bool isWindowActive();

protected:
	LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Members
	WindowHandle*			_handle;
	bool					_isValid;
	bool					_isVisible;
	bool					_isHasFocus;

	unsigned int			_width;
	unsigned int			_height;
	
	//Any time there was a call from other object to move, resize, minimize
	//this variable indicates that there is a request pending
	bool					_isStateChangeRequested;

	KMInput*				_km;
};