#pragma once

#include "WindowManager.hpp"
#include "WindowHandle.hpp"
#include "SceneGraph.hpp"
#include "Loader.hpp"
#include "Error.hpp"
#include "CommnadLineParser.hpp"


class Application : public Error
{
public:
	//Runs the app
	//Receives command line parameters
	//Returns false if something went wrong (error & stuff)
	bool Run(int argc, char* argv[]);

	Application();
	~Application();

private:
	Loader*				_loader;
	WindowHandle*		_windowHandle;
	WindowManager*		_windowManager;
	CommandLineParser*	_cmdParser;
	
	SceneGraph*			_sceneGraph;

	//MessageQueue
	//

	//Callbacks for window manager
	void onMouse();
	void onKeyboard();
	void onSystem();
};