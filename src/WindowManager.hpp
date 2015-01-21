#pragma once

#include "Thread.hpp"
#include "Error.hpp"
#include "WindowHandle.hpp"
#include "KMInput.hpp"

#include <queue>
#include <mutex>

//Threaded window manager
class WindowManager : public Thread, public Error
{
public:

	//Flags used for window initialization
	enum class WindowFlags : unsigned int
	{

	};

	WindowManager(std::thread::id master);
	~WindowManager();

	bool init(unsigned int width, unsigned int height, const wchar_t* className, const wchar_t* title, unsigned int flags);

	bool isValid(){ return _isValid; }

	void setTitle(const wchar_t* title);
	void setCursorVisible(bool isCursorVisible);

	const WindowHandle* const getHandle() const;

	//ThreadMain from Thread class, implements message loop
	void threadMain(void* arguments) override;

	unsigned int getWidth() const;
	unsigned int getHeight() const;

	//Removes all messages from queue
	void flushMessageQueue();

	//Musi vediet zamykat vstup z mysi a klavesnice
	//Posielat spravy o zmene velkosti

	void minimize();
	void restore();

	bool isMinimized();
	bool isHasFocus();

	//Active window = visible (not minimized) and has focus (= accepting input messages)
	bool isWindowActive();

protected:
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Members
	WindowHandle*			_handle;
	bool					_isValid;
	bool					_isVisible;
	bool					_isHasFocus;

	unsigned int			_width;
	unsigned int			_height;

	std::queue<>			_messageQueue;
	std::mutex				_queueMutex;
	bool					_isQueueBlocked;

	KMInput*				_km;
};