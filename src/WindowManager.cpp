#include "WindowManager.h"

WindowManager::WindowManager()
{
	_isValid = false;
	_isVisible = false;
	_isHasFocus = false;
}

bool WindowManager::init(unsigned int width, unsigned int height, bool isResizable)
{
	//Register windows class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = program_instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = TEXT("DGPUClass");
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex))
		return false;

	RECT console_rect = { 0, 0, 0, 0 };
	GetWindowRect(console_hWnd, &console_rect);

	// Create window
	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		L"DGPUClass",
		L"DualGpuTestApp",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		console_rect.left + (console_rect.right - console_rect.left),
		console_rect.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		program_instance,
		nullptr);

	if (!g_hWnd)
		return false;

	ShowWindow(g_hWnd, SW_SHOWNORMAL);
}