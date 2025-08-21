#include "engine.h"
#include "ifilesystem.h"
#include <stdio.h>

void Engine::Init()
{
	GetFileSystem()->Init();
	
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
	
	GetFileSystem()->Shutdown();
}

