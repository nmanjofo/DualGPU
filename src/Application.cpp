#include "Application .hpp"

Application::Application()
{
	_loader = nullptr;
	_windowHandle = nullptr;
	_windowManager = nullptr;
	_cmdParser = nullptr;
	_sceneGraph = nullptr;

	_isExit = false;
}

Application::~Application()
{

}

void Application::_onKeyboard(void* arg)
{

}

void Application::_onMouse(void* arg)
{

}


void Application::_onSystem(void* arg)
{

}


bool Application::Run(int argc, char* argv[])
{
	//Create objects
	_windowManager = new WindowManager;

	//Init
	CallbackFromInstance<Application> onMouse;
	CallbackFromInstance<Application> onKeyboard;
	CallbackFromInstance<Application> onSystem;

	onMouse.bind(&Application::_onMouse, this);
	onKeyboard.bind(&Application::_onKeyboard, this);
	onSystem.bind(&Application::_onSystem, this);

	bool retval = _windowManager->createWindow(800, 600, L"DGPU", L"Dual GPU Engine", 0, &onMouse, &onKeyboard, &onSystem );

	if (retval == false)
	{
		return false;
	}



	//Loop
	while (!_isExit)
	{

	}

	return true;
}