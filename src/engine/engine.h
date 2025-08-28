#ifndef ENGINE_H
#define ENGINE_H

#include "iengine.h"

#include <SDL.h>

class Engine : public IEngine
{
public:
	void Init() override;
	void RunLoop() override;
	void Loop() override;
	void Shutdown() override;

private:
	void InitServerDll();

private:
	SDL_Window* m_pWindow = nullptr;
	bool m_bExit = false;
};

extern int g_mousePoxX;
extern int g_mousePoxY;

#endif