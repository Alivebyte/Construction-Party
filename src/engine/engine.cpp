#include "engine.h"
#include "ifilesystem.h"
#include "ilogger.h"
#include "render.h"
#include "renderdevice.h"
#include "gpu_buffer.h"
#include "shader.h"
#include "shadersystem.h"
#include <stdio.h>

#define WINDOW_TITLE "TBN"

static bool s_bIsDedicated = false;

static GPUBuffer* s_pTriangleBuffer = nullptr;
static Shader* s_pShader = nullptr;

void Engine::Init()
{
	// Initialize logger
	GetLogger()->Init();

	// Initialize file system
	GetFileSystem()->Init();

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		GetLogger()->Print("Failed to intiailize SDL2. Error: %s", SDL_GetError());
		SDL_assert_always(0);
	}

	// Create render window
	m_pWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	
	// Initialize renderer
	if (!s_bIsDedicated)
	{
		g_pRender = new Render();
		g_pRender->Init(m_pWindow);

		// Create vertex buffer

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		s_pTriangleBuffer = g_pRenderDevice->CreateVertexBuffer(vertices, sizeof(vertices));

		// Create shader
	
		static InputLayoutDesc_t inputLayout[] =
		{
			{ VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION },
			{ VERTEXATTR_VEC4, SHADERSEMANTIC_COLOR },
		};

		s_pShader = g_pShaderSystem->CreateShader("test", "data/test.vs", "data/test.ps", inputLayout, sizeof(inputLayout) / sizeof(inputLayout[0]));
	}

	GetLogger()->Print("Engine started!\n");
	GetLogger()->Print("Starting engine loop.\n");

	RunLoop();
}

void Engine::RunLoop()
{
	while (!m_bExit)
	{
		Loop();
	}
}

void Engine::Loop()
{
	// TODO: Actual loop lol

	// Event handling
	if (m_pWindow)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: 
				m_bExit = true; 
				break;

			default:
				break;
			}
		}
	}

	// Rendering
	if (!s_bIsDedicated)
	{
		int windowSizeX = 0, windowSizeY = 0;
		SDL_GetWindowSize(m_pWindow, &windowSizeX, &windowSizeY);

		// install viewport
		g_pRenderDevice->SetViewport(0, 0, windowSizeX, windowSizeY);

		// Clear screen
		g_pRenderDevice->Clear(TST_COLOR | TST_DEPTH, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0xffffffff);

		// Draw triangle
		g_pRenderDevice->SetVerticesBuffer(s_pTriangleBuffer);
		g_pShaderSystem->SetShader(s_pShader);
		g_pRenderDevice->DrawArrays(PT_TRIANGLES, 0, 3);

		// Present the backbuffer to screen
		g_pRender->Present();
	}
}

void Engine::Shutdown()
{
	// TODO: Actually shutdown
	
	if (g_pRender)
	{
		g_pRender->Shutdown();

		delete g_pRender;
		g_pRender = nullptr;
	}

	GetFileSystem()->Shutdown();

	GetLogger()->Shutdown();
}

