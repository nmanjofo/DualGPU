#include "Application .hpp"

Application::Application()
{
	_loader = nullptr;
	_windowHandle = nullptr;
	_windowManager = nullptr;
	_cmdParser = nullptr;
	_sceneGraph = nullptr;

}

bool Application::Run(int argc, char* argv[])
{
	//Create objects
	_windowManager = new WindowManager;

	//Init
	bool retval = _windowManager->createWindow(800, 600, L"DGPU", L"Dual GPU Engine", 0);

	//Loop

	return true;
}