#ifndef IENGINE_H
#define IENGINE_H

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

struct SDL_Window;
class IServerGame;

class IEngine
{
public:
	virtual void Init() = 0;
	virtual void RunLoop() = 0;
	virtual void Loop() = 0;
	virtual void Shutdown() = 0;
	virtual SDL_Window* GetWindow() = 0;
	virtual IServerGame* GetServerGame() = 0;
	virtual float GetDeltaTime() = 0;
};

extern "C" __declspec(dllexport) IEngine* GetEngine();

#endif