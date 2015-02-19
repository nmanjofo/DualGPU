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

//Handles system messages
void Application::_onSystem(void* arg)
{
	switch (((SystemMessage*)arg)->msg)
	{
	case SysMsg::KILLFOCUS:
		std::cout << "Stracam focus\n";
		break;

	case SysMsg::SETFOCUS:
		std::cout << "Dostavam focus\n";
		break;

	case SysMsg::MAXIMIZE:
		std::cout << "Maximalizacia\n";
		break;

	case SysMsg::MINIMIZE:
		std::cout << "Minimalizacia\n";
		break;

	case SysMsg::RESTORE:
		std::cout << "Obnovenie\n";
		break;

	case SysMsg::QUIT:
		_isExit = true;
		break;

	default:
		break;
	}
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
		Sleep(100);
	}

	Clean();

	return true;
}

void Application::Clean()
{
	if (_windowManager)
		delete _windowManager;
}