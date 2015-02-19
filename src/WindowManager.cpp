//Windows implementation of a window, MS WINDOWS

#include "WindowManager.hpp"
#include "KMInput.hpp"

#include <Windows.h>

WindowManager::WindowManager() : Thread()
{
	_isValid = false;
	_isInitComplete = false;
	_isVisible = false;
	_isHasFocus = false;
	_isKeySetPresent = false;

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

struct WMArray {
	UINT Code;
	char Message[100];
} waWMArray[] = {
	{ 0x0000, "WM_NULL" }, { 0x0001, "WM_CREATE" }, { 0x0002, "WM_DESTROY" }, { 0x0003, "WM_MOVE" },
	{ 0x0005, "WM_SIZE" }, { 0x0006, "WM_ACTIVATE" }, { 0x0007, "WM_SETFOCUS" }, { 0x0008, "WM_KILLFOCUS" },
	{ 0x000A, "WM_ENABLE" }, { 0x000B, "WM_SETREDRAW" }, { 0x000C, "WM_SETTEXT" }, { 0x000D, "WM_GETTEXT" },
	{ 0x000E, "WM_GETTEXTLENGTH" }, { 0x000F, "WM_PAINT" }, { 0x0010, "WM_CLOSE" }, { 0x0011, "WM_QUERYENDSESSION" },
	{ 0x0012, "WM_QUIT" }, { 0x0013, "WM_QUERYOPEN" }, { 0x0014, "WM_ERASEBKGND" }, { 0x0015, "WM_SYSCOLORCHANGE" },
	{ 0x0016, "WM_ENDSESSION" }, { 0x0018, "WM_SHOWWINDOW" }, { 0x001A, "WM_WININICHANGE" }, { WM_WININICHANGE, "WM_SETTINGCHANGE" },
	{ 0x001B, "WM_DEVMODECHANGE" }, { 0x001C, "WM_ACTIVATEAPP" }, { 0x001D, "WM_FONTCHANGE" }, { 0x001E, "WM_TIMECHANGE" },
	{ 0x001F, "WM_CANCELMODE" }, { 0x0020, "WM_SETCURSOR" }, { 0x0021, "WM_MOUSEACTIVATE" }, { 0x0022, "WM_CHILDACTIVATE" },
	{ 0x0023, "WM_QUEUESYNC" }, { 0x0024, "WM_GETMINMAXINFO" }, { 0x0026, "WM_PAINTICON" }, { 0x0027, "WM_ICONERASEBKGND" },
	{ 0x0028, "WM_NEXTDLGCTL" }, { 0x002A, "WM_SPOOLERSTATUS" }, { 0x002B, "WM_DRAWITEM" }, { 0x002C, "WM_MEASUREITEM" },
	{ 0x002D, "WM_DELETEITEM" }, { 0x002E, "WM_VKEYTOITEM" }, { 0x002F, "WM_CHARTOITEM" }, { 0x0030, "WM_SETFONT" },
	{ 0x0031, "WM_GETFONT" }, { 0x0032, "WM_SETHOTKEY" }, { 0x0033, "WM_GETHOTKEY" }, { 0x0037, "WM_QUERYDRAGICON" },
	{ 0x0039, "WM_COMPAREITEM" }, { 0x003D, "WM_GETOBJECT" }, { 0x0041, "WM_COMPACTING" }, { 0x0044, "WM_COMMNOTIFY" },
	{ 0x0046, "WM_WINDOWPOSCHANGING" }, { 0x0047, "WM_WINDOWPOSCHANGED" }, { 0x0048, "WM_POWER" }, { 0x004A, "WM_COPYDATA" },
	{ 0x004B, "WM_CANCELJOURNAL" }, { 0x004E, "WM_NOTIFY" }, { 0x0050, "WM_INPUTLANGCHANGEREQUEST" }, { 0x0051, "WM_INPUTLANGCHANGE" },
	{ 0x0052, "WM_TCARD" }, { 0x0053, "WM_HELP" }, { 0x0054, "WM_USERCHANGED" }, { 0x0055, "WM_NOTIFYFORMAT" },
	{ 0x007B, "WM_CONTEXTMENU" }, { 0x007C, "WM_STYLECHANGING" }, { 0x007D, "WM_STYLECHANGED" }, { 0x007E, "WM_DISPLAYCHANGE" },
	{ 0x007F, "WM_GETICON" }, { 0x0080, "WM_SETICON" }, { 0x0081, "WM_NCCREATE" }, { 0x0082, "WM_NCDESTROY" },
	{ 0x0083, "WM_NCCALCSIZE" }, { 0x0084, "WM_NCHITTEST" }, { 0x0085, "WM_NCPAINT" }, { 0x0086, "WM_NCACTIVATE" },
	{ 0x0087, "WM_GETDLGCODE" }, { 0x0088, "WM_SYNCPAINT" }, { 0x00A0, "WM_NCMOUSEMOVE" }, { 0x00A1, "WM_NCLBUTTONDOWN" },
	{ 0x00A2, "WM_NCLBUTTONUP" }, { 0x00A3, "WM_NCLBUTTONDBLCLK" }, { 0x00A4, "WM_NCRBUTTONDOWN" }, { 0x00A5, "WM_NCRBUTTONUP" },
	{ 0x00A6, "WM_NCRBUTTONDBLCLK" }, { 0x00A7, "WM_NCMBUTTONDOWN" }, { 0x00A8, "WM_NCMBUTTONUP" }, { 0x00A9, "WM_NCMBUTTONDBLCLK" },
	{ 0x0100, "WM_KEYFIRST" }, { 0x0100, "WM_KEYDOWN" }, { 0x0101, "WM_KEYUP" }, { 0x0102, "WM_CHAR" },
	{ 0x0103, "WM_DEADCHAR" }, { 0x0104, "WM_SYSKEYDOWN" }, { 0x0105, "WM_SYSKEYUP" }, { 0x0106, "WM_SYSCHAR" },
	{ 0x0107, "WM_SYSDEADCHAR" }, { 0x0108, "WM_KEYLAST" }, { 0x010D, "WM_IME_STARTCOMPOSITION" }, { 0x010E, "WM_IME_ENDCOMPOSITION" },
	{ 0x010F, "WM_IME_COMPOSITION" }, { 0x010F, "WM_IME_KEYLAST" }, { 0x0110, "WM_INITDIALOG" }, { 0x0111, "WM_COMMAND" },
	{ 0x0112, "WM_SYSCOMMAND" }, { 0x0113, "WM_TIMER" }, { 0x0114, "WM_HSCROLL" }, { 0x0115, "WM_VSCROLL" },
	{ 0x0116, "WM_INITMENU" }, { 0x0117, "WM_INITMENUPOPUP" }, { 0x011F, "WM_MENUSELECT" }, { 0x0120, "WM_MENUCHAR" },
	{ 0x0121, "WM_ENTERIDLE" }, { 0x0122, "WM_MENURBUTTONUP" }, { 0x0123, "WM_MENUDRAG" }, { 0x0124, "WM_MENUGETOBJECT" },
	{ 0x0125, "WM_UNINITMENUPOPUP" }, { 0x0126, "WM_MENUCOMMAND" }, { 0x0132, "WM_CTLCOLORMSGBOX" }, { 0x0133, "WM_CTLCOLOREDIT" },
	{ 0x0134, "WM_CTLCOLORLISTBOX" }, { 0x0135, "WM_CTLCOLORBTN" }, { 0x0136, "WM_CTLCOLORDLG" }, { 0x0137, "WM_CTLCOLORSCROLLBAR" },
	{ 0x0138, "WM_CTLCOLORSTATIC" }, { 0x0200, "WM_MOUSEFIRST" }, { 0x0200, "WM_MOUSEMOVE" }, { 0x0201, "WM_LBUTTONDOWN" },
	{ 0x0202, "WM_LBUTTONUP" }, { 0x0203, "WM_LBUTTONDBLCLK" }, { 0x0204, "WM_RBUTTONDOWN" }, { 0x0205, "WM_RBUTTONUP" },
	{ 0x0206, "WM_RBUTTONDBLCLK" }, { 0x0207, "WM_MBUTTONDOWN" }, { 0x0208, "WM_MBUTTONUP" }, { 0x0209, "WM_MBUTTONDBLCLK" },
	{ 0x020A, "WM_MOUSEWHEEL" }, { 0x020A, "WM_MOUSELAST" }, { 0x0209, "WM_MOUSELAST" }, { 0x0210, "WM_PARENTNOTIFY" },
	{ 0x0211, "WM_ENTERMENULOOP" }, { 0x0212, "WM_EXITMENULOOP" }, { 0x0213, "WM_NEXTMENU" }, { 0x0214, "WM_SIZING" },
	{ 0x0215, "WM_CAPTURECHANGED" }, { 0x0216, "WM_MOVING" }, { 0x0218, "WM_POWERBROADCAST" }, { 0x0219, "WM_DEVICECHANGE" },
	{ 0x0220, "WM_MDICREATE" }, { 0x0221, "WM_MDIDESTROY" }, { 0x0222, "WM_MDIACTIVATE" }, { 0x0223, "WM_MDIRESTORE" },
	{ 0x0224, "WM_MDINEXT" }, { 0x0225, "WM_MDIMAXIMIZE" }, { 0x0226, "WM_MDITILE" }, { 0x0227, "WM_MDICASCADE" },
	{ 0x0228, "WM_MDIICONARRANGE" }, { 0x0229, "WM_MDIGETACTIVE" }, { 0x0230, "WM_MDISETMENU" }, { 0x0231, "WM_ENTERSIZEMOVE" },
	{ 0x0232, "WM_EXITSIZEMOVE" }, { 0x0233, "WM_DROPFILES" }, { 0x0234, "WM_MDIREFRESHMENU" }, { 0x0281, "WM_IME_SETCONTEXT" },
	{ 0x0282, "WM_IME_NOTIFY" }, { 0x0283, "WM_IME_CONTROL" }, { 0x0284, "WM_IME_COMPOSITIONFULL" }, { 0x0285, "WM_IME_SELECT" },
	{ 0x0286, "WM_IME_CHAR" }, { 0x0288, "WM_IME_REQUEST" }, { 0x0290, "WM_IME_KEYDOWN" }, { 0x0291, "WM_IME_KEYUP" },
	{ 0x02A1, "WM_MOUSEHOVER" }, { 0x02A3, "WM_MOUSELEAVE" }, { 0x0300, "WM_CUT" }, { 0x0301, "WM_COPY" },
	{ 0x0302, "WM_PASTE" }, { 0x0303, "WM_CLEAR" }, { 0x0304, "WM_UNDO" }, { 0x0305, "WM_RENDERFORMAT" },
	{ 0x0306, "WM_RENDERALLFORMATS" }, { 0x0307, "WM_DESTROYCLIPBOARD" }, { 0x0308, "WM_DRAWCLIPBOARD" }, { 0x0309, "WM_PAINTCLIPBOARD" },
	{ 0x030A, "WM_VSCROLLCLIPBOARD" }, { 0x030B, "WM_SIZECLIPBOARD" }, { 0x030C, "WM_ASKCBFORMATNAME" }, { 0x030D, "WM_CHANGECBCHAIN" },
	{ 0x030E, "WM_HSCROLLCLIPBOARD" }, { 0x030F, "WM_QUERYNEWPALETTE" }, { 0x0310, "WM_PALETTEISCHANGING" }, { 0x0311, "WM_PALETTECHANGED" },
	{ 0x0312, "WM_HOTKEY" }, { 0x0317, "WM_PRINT" }, { 0x0318, "WM_PRINTCLIENT" }, { 0x0358, "WM_HANDHELDFIRST" },
	{ 0x035F, "WM_HANDHELDLAST" }, { 0x0360, "WM_AFXFIRST" }, { 0x037F, "WM_AFXLAST" }, { 0x0380, "WM_PENWINFIRST" },
	{ 0x038F, "WM_PENWINLAST" }, { 0x8000, "WM_APP" }, { 0x0400, "WM_USER" }
};

std::string getMsgString(UINT msg)
{
	for (int i = 0; i < sizeof(waWMArray) / sizeof(WMArray); i++)
		if (waWMArray[i].Code == msg)
			return waWMArray[i].Message;

	return "UNKNOWN MESSAGE";
}

//Windows message callback function
LRESULT CALLBACK WindowManager::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WindowManager* manager = nullptr;

	//std::cout << getMsgString(message) << std::endl;

	if (message == WM_NCCREATE)
	{
		//Get manager pointer from lpCreateParams
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
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		SystemMessage msg;
		msg.msg = SysMsg::QUIT;
		_onSystem->call(&msg);

		PostQuitMessage(0);
		break;
	}
	case WM_SYSCOMMAND:
	{
		SystemMessage msg;
		if (wParam == SC_MINIMIZE)
		{
			_isVisible = false;
			msg.msg = SysMsg::MINIMIZE;
			_onSystem->call(&msg);
			break;
		}
		else if (wParam == SC_RESTORE)
		{
			_isVisible = true;
			msg.msg = SysMsg::RESTORE;
			_onSystem->call(&msg);
			break;
		}
		break;
	}
	case WM_SETFOCUS:
	{
		SystemMessage msg;
		msg.msg = SysMsg::SETFOCUS;
		_onSystem->call(&msg);

		_isHasFocus = true;
		break;
	}
	case WM_KILLFOCUS:
	{
		SystemMessage msg;
		msg.msg = SysMsg::KILLFOCUS;
		_onSystem->call(&msg);

		_isHasFocus = false;
		break;
	}
	case WM_INPUT:
	{
		if (!isWindowActive())
			break;

		const KMInput::KMInputMessage* msg = _km->processInputMessage(lParam);
		if (msg != nullptr)
		{
			if (msg->messageType == KMInput::KMINPUT_MESSAGE_KEYBOARD)
			{
				if (_isKeySetPresent)
				{
					if (_selectedKeysSet.find(msg->keyboard.key) == _selectedKeysSet.end())
						break;
				}

				static KeyboardMessage kbMSG;

				kbMSG.key = msg->keyboard.key;
				kbMSG.isPressed = msg->keyboard.state == KMInput::KMInputKeyState::KMINPUT_KEY_PRESSED;

				_onKeyboard->call(&kbMSG);
			}
			else if (msg->messageType == KMInput::KMINPUT_MESSAGE_MOUSE)
			{
				//Only valid when inside client area
				POINT point;
				RECT lpRect;
				GetCursorPos(&point);
				GetClientRect(*((HWND*)_handle), &lpRect);
				ScreenToClient(*((HWND*)_handle), &point);
				
				//Test if the cursor is inside the window
				if (PtInRect(&lpRect, point) == 0)
					break;

				static MouseMessage mouseMSG;
				
				//Je to nutne kopirovat?
				mouseMSG.deltaWheel = msg->mouse.deltaWheel;
				mouseMSG.deltaX = msg->mouse.deltaX;
				mouseMSG.deltaY = msg->mouse.deltaY;
				mouseMSG.isLeftPressed = msg->mouse.deltaWheel;
				mouseMSG.isMiddlePressed = msg->mouse.deltaWheel;
				mouseMSG.isRightPressed = msg->mouse.deltaWheel;
				_onMouse->call(&mouseMSG);
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

void WindowManager::setKeySet(std::set<enum Keys> keys)
{
	_selectedKeysSet = keys;
	_isKeySetPresent = true;
}

bool WindowManager::createWindow(unsigned int width, unsigned int height, const wchar_t* className, const wchar_t* title, unsigned int flags, Callback* onMouse, Callback* onKeyboard, Callback* onSystem)
{
	if (_handle == nullptr)
	{
		if (onMouse == nullptr || onKeyboard == nullptr || onSystem == nullptr)
		{
			ErrorMessage("One of the callbacks is invalid");
			return false;
		}

		_onKeyboard = onKeyboard;
		_onMouse = onMouse;
		_onSystem = onSystem;

		_width = width;
		_height = height;

		//Create window and spawn window thread function - message loop
		WindowCreateParams params;
		params.className = className;
		params.flags = flags;
		params.height = height;
		params.onKeyboard = onKeyboard;
		params.onMouse = onMouse;
		params.onSystem = onSystem;
		params.title = title;
		params.width = width;

		run(&params);

		//Wait for window to initialize
		while (!_isInitComplete)
			Sleep(10);

		return _isValid;
	}

	return true;
}

//RUNS IN SEPARATE THREAD!
//Creates window, then processes it's messages
void WindowManager::threadMain(void* arguments)
{
	WindowCreateParams* params = (WindowCreateParams*)arguments;

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
	wcex.lpszClassName = params->className;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex))
	{
		return;
	}

	RECT console_rect = { 0, 0, 0, 0 };
	GetWindowRect(GetConsoleWindow(), &console_rect);

	// Create window
	RECT rc = { 0, 0, params->width, params->height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWND = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		params->className,
		params->title,
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
		_isInitComplete = true;
		return;
	}

	ShowWindow(hWND, SW_SHOWNORMAL);
	_isVisible = true;

	//Handle
	_handle = new WindowHandle(&hWND);
	if (_handle == nullptr)
	{
		ErrorMessage("Failed to create WindowHandle!");
		_isInitComplete = true;
		return;
	}

	//Init keyboard and mouse input, keep it blocked for now
	_km = new KMInput(hWND);
	if (_km->isValid() == false)
	{
		ErrorMessage("Failed to init KMInput!");
		_isInitComplete = true;
		return;
	}
	_km->setKeyboardMode(KMInput::KMInputMode::KMIMPUT_KEYBOARD_MODE_SINGLE_DOWN);

	_isInitComplete = true;
	_isValid = true;

	//------Message Processing------
	bool quitByWindow = false;
	bool quitExternal = false;

	HWND hwnd = (HWND)(*((HWND*)_handle->getRawHandle()));

	while (!(quitByWindow || quitExternal))
	{
		static MSG msg = { 0 };
		static const unsigned int messageTimeoutMiliseconds = 50;
		
		//Wait for window message to appear in the queue
		static DWORD retval = MsgWaitForMultipleObjects(0, nullptr, FALSE, messageTimeoutMiliseconds, QS_ALLINPUT);
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
		if (isTimeToDie())
			quitExternal = true;
	}

	return;
}