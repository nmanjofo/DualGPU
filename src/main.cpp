#include "Application .hpp"


int main(int argc, char* argv[])
{
	Application* app = new Application;

	bool retval = app->Run(argc, argv);

	delete app;

	if (retval)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}