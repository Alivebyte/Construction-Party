#ifndef RENDER_H
#define RENDER_H

#include "gl_shared.h"
#include "render_shared.h"

class GPUBuffer;

class Render
{
public:
	Render();
	~Render();

	void Init(SDL_Window* pWindow);
	void Shutdown();

	void Present(bool vsync = false);

private:
	SDL_Window* m_pWindow;
	SDL_GLContext m_pGLContext;

	GPUBuffer* m_pStretchedPicVBuf;

	bool m_bUsingVAO;
};

extern Render* g_pRender;

// TEMP
extern glm::vec3 g_viewOrigin;
extern glm::vec3 g_viewOrient;

#endif // !RENDER_H
