#include "pch.h"

#include "application.h"

int main()
{
	// Application settings
	Application::Settings settings{};
	settings.windowWidth    = 800;
	settings.windowHeight   = 600;
	settings.framesInFlight = 2;
	settings.vSync          = false;
	settings.cpuRaytracing  = false;
	settings.useRtx         = false;

	// Initialize application
	Application app;
	app.init(settings);
  
	// Run application
	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
