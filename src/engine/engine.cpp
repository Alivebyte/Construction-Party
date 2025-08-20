#include "engine.h"
#include <stdio.h>

void Engine::Init()
{
	printf("Engine started!\n");
	printf("Starting engine loop.\n");

	RunLoop();
}

void Engine::RunLoop()
{
	while (true)
	{
		Loop();
	}
}

void Engine::Loop()
{
	// TODO: Actual loop lol
}

void Engine::Shutdown()
{
	// TODO: Actually shutdown
}

