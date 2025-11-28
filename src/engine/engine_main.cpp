#include "engine_main.h"
#include "engine.h"

extern "C" int __declspec(dllexport) Engine_Main(int argc, char** argv)
{
	bool initSound = true;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-nosound") == 0)
			initSound = false;
	}

	GetEngine()->Init(initSound);
	GetEngine()->Shutdown();
	return 0;
}

IEngine* GetEngine()
{
	static Engine s_engine;
	return &s_engine;
}
