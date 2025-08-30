#include "engine_main.h"
#include "engine.h"

IEngine* g_Engine = new Engine;

extern "C" int __declspec(dllexport) Engine_Main(int argc, char** argv)
{
	
	if (g_Engine)
	{
		g_Engine->Init();
		return 0;
	}
	else return 1;
}

IEngine* GetEngine()
{
	return g_Engine;
}
