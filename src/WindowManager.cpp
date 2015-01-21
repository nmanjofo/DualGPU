//Windows implementation of a window

#include "WindowManager.hpp"
#include "KMInput.hpp"

#include <Windows.h>

WindowManager::WindowManager(std::thread::id master) : Thread(master)
{
	_isValid = false;
	_isVisible = false;
	_isHasFocus = false;

	_handle = nullptr;

	_isQueueBlocked = true; //starting with message queue blocked!
}

WindowManager::~WindowManager()
{
	if (_isValid)
	{
		kill();
	}
}

//Windows message callback function
//Pushes messages into msg queues
LRESULT CALLBACK WindowManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			std::cout << "Minimalizujem sa\n";
			break;
		}
		else if (wParam == SC_RESTORE)
		{
			std::cout << "Obnovujem sa\n";
			break;
		}
		break;

	case WM_SETFOCUS:
		_isHasFocus = true;
		break;

	case WM_KILLFOCUS:
		_isHasFocus = false;
		break;

	case WM_INPUT:
		const KMInput::KMInputMessage* msg = _km->processInputMessage(lParam);
		if (msg != nullptr)
		{
			if (msg->messageType == KMInput::KMINPUT_MESSAGE_KEYBOARD)
			{
				std::cout << "klavesa\n";
			}
			//Can be restored via clicking - this click must be ignored
			else if (msg->messageType == KMInput::KMINPUT_MESSAGE_MOUSE)
			{
				std::cout << "mys\n";
			}
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool WindowManager::isHasFocus()
{
	return _isHasFocus;
}

bool WindowManager::isMinimized()
{
	return _isVisible;
}

bool WindowManager::isWindowActive()
{
	return _isHasFocus && _isVisible;
}

void WindowManager::minimize()
{
	if (_isVisible)
	{
		SendMessage((HWND)(*((HWND*)_handle->getRawHandle())), WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}

void WindowManager::restore()
{
	if (!_isVisible)
	{
		SendMessage((HWND)(*((HWND*)_handle->getRawHandle())), WM_SYSCOMMAND, SC_RESTORE, 0);
	}
}


bool WindowManager::init(unsigned int width, unsigned int height, const wchar_t* className, const wchar_t* title, unsigned int flags)
{
	//Get program instance
	HINSTANCE programInstance = GetModuleHandle(nullptr);

	//Register windows class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = programInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = className;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex))
		return false;

	RECT console_rect = { 0, 0, 0, 0 };
	GetWindowRect(GetConsoleWindow(), &console_rect);

	// Create window
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWND = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		className,
		title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		console_rect.left + (console_rect.right - console_rect.left),
		console_rect.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		programInstance,
		nullptr);

	if (!hWND)
	{
		ErrorMessage("Failed to initialize window!");
		return false;
	}

	ShowWindow(hWND, SW_SHOWNORMAL);

	_width = width;
	_height = height;

	//Init keyboard and mouse input, keep it blocked for now
	_km = new KMInput(hWND);
	if (_km->isValid() == false)
	{
		ErrorMessage("Failed to init KMInput");
		return false;
	}

	_isQueueBlocked = true;

	//Spawn window thread function - message loop
	run(nullptr);

	_isValid = true;

	return true;
}

//RUNS IN SEPARATE THREAD!
void WindowManager::threadMain(void* arguments)
{
	bool quitByWindow = false;
	bool quitExternal = false;

	while (!(quitByWindow | quitExternal))
	{
		//check for messages from other threads
		static ThreadMessage tmsg = peekMessage();
		switch (tmsg.messageType)
		{
		case MessageType::NO_MESSAGE:
			break;
		case MessageType::TERMINATE:
			quitExternal = true;
		default:
			break;
		}

		//check for window messages
		MSG msg = { 0 };
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{+
			TranslateMessage(&msg);
			if (msg.hwnd != 0)
				DispatchMessage(&msg);
		}

		if (msg.message != WM_QUIT)
			Sleep(5); //tak toto nie...
		else
			quitByWindow = true;
	}

	//Sends back quitting message, if it is from window
	if (quitByWindow)
	{

	}
}