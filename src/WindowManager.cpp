//Windows implementation of a window, MS WINDOWS

#include "WindowManager.hpp"
#include "KMInput.hpp"

#include <Windows.h>

WindowManager::WindowManager() : Thread()
{
	_isValid = false;
	_isVisible = false;
	_isHasFocus = false;

	_handle = nullptr;
}

WindowManager::~WindowManager()
{
	//Kill the msg loop thread, if runnig
	if (_isValid)
	{
		kill();
	}

	//Cleanup
	if (_km != nullptr)		delete _km;
	if (_handle != nullptr) delete _handle;
}

//Windows message callback function
LRESULT CALLBACK WindowManager::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WindowManager* manager = nullptr;

	if (message == WM_NCCREATE)
	{
		//Get managet pointer from lpCreateParams
		manager = (WindowManager*)((LPCREATESTRUCT)lParam)->lpCreateParams;

		//Set Window User data to Window Manager Pointer
		//May by used later
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)manager);

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return manager->processMessage(hWnd, message, wParam, lParam);
}

LRESULT WindowManager::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Callback<WindowManager> clb;
	clb.bind(&WindowManager::bla, this);

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
	{
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
	}
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

void WindowManager::resize(unsigned int width, unsigned int height)
{
	SetWindowPos((HWND)(*((HWND*)_handle->getRawHandle())), nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void WindowManager::setWindowPosition(unsigned int x, unsigned int y)
{
	SetWindowPos((HWND)(*((HWND*)_handle->getRawHandle())), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

bool WindowManager::createWindow(unsigned int width, unsigned int height, const wchar_t* className, const wchar_t* title, unsigned int flags)
{
	if (_handle == nullptr)
	{
		//Get program instance
		HINSTANCE programInstance = GetModuleHandle(nullptr);

		//Register windows class
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)(WindowManager::_WndProc);
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
			(LPVOID)this); //pass myself to the msg loop function, so it can interact with this object

		if (!hWND)
		{
			ErrorMessage("Failed to initialize window!");
			return false;
		}

		ShowWindow(hWND, SW_SHOWNORMAL);

		_width = width;
		_height = height;

		//Handle
		_handle = new WindowHandle(&hWND);
		if (_handle == nullptr)
		{
			ErrorMessage("Failed to create WindowHandle!");
			return false;
		}

		//Init keyboard and mouse input, keep it blocked for now
		_km = new KMInput(hWND);
		if (_km->isValid() == false)
		{
			ErrorMessage("Failed to init KMInput!");
			return false;
		}

		//Spawn window thread function - message loop
		run(nullptr);

		_isValid = true;
	}

	return true;
}

//RUNS IN SEPARATE THREAD!
void WindowManager::threadMain(void* arguments)
{
	bool quitByWindow = false;
	bool quitExternal = false;

	//TREBA odchytit flagy! aj na zabitie okna, a pod

	HWND hwnd = (HWND)(*((HWND*)_handle->getRawHandle()));

	while (!(quitByWindow | quitExternal))
	{
		static MSG msg = { 0 };
		static const unsigned int messageTimeout = 100;
		
		//Wait for window message to appear in the queue
		static DWORD retval = MsgWaitForMultipleObjects(0, nullptr, FALSE, messageTimeout, QS_ALLINPUT);
		if (retval != WAIT_TIMEOUT && retval != WAIT_FAILED)
		{
			if (GetMessage(&msg, hwnd, 0, 0))
			{
				TranslateMessage(&msg);
				if (msg.hwnd != 0)
					DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
				quitByWindow = true;
		}
		else if (retval == WAIT_FAILED)
		{
			ErrorMessage("MsgWaitForMultipleObjects Failed!");
		}

		//Checking flags
	}

	//Calls quitting message CALLBACK, if it is from window
	if (quitByWindow)
	{

	}
}