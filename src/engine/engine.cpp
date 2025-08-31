#include "engine.h"
#include "ifilesystem.h"
#include "ilogger.h"
#include "ientity.h"
#include "render.h"
#include "renderdevice.h"
#include "gpu_buffer.h"
#include "shader.h"
#include "shadersystem.h"
#include "modelsystem.h"
#include "world.h"
#include <stdio.h>

#define WINDOW_TITLE "Construction Party"

static bool s_bIsDedicated = false;
static bool s_bCoreProfile = true;

static IClientGame* g_pClientGame = nullptr;
static IServerGame* g_pServerGame = nullptr;

int g_mousePoxX = 0;
int g_mousePoxY = 0;

void Engine::Init()
{
	// Initialize logger
	GetLogger()->Init();

	// Initialize file system
	GetFileSystem()->Init();

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		GetLogger()->Error("Failed to intiailize SDL2. Error: %s", SDL_GetError());
	}

	// Enable core profile for compability for new hardware
	if (s_bCoreProfile)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef _DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // _DEBUG

		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}

	// Create render window
	m_pWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	
	// Initialize renderer
	if (!s_bIsDedicated)
	{
		g_pRender = new Render();
		g_pRender->Init(m_pWindow);
	}

	// Load server dll
	InitServerDll();

	// Load client dll
	InitClientDll();

	// Initialize Server Game
	// #TODO: Should be on game start?
	g_pServerGame->Init();

	// Initialize Client Game
	// #TODO: Should be on game start?
	g_pClientGame->Init();

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
	// Get current ticks
	static Uint32 startTime = SDL_GetTicks(), endTime = SDL_GetTicks();

	startTime = SDL_GetTicks();
	m_fDeltaTime = ((float)startTime - (float)endTime) / 1000.0f;

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

			// Client event handling
			g_pClientGame->OnEvent(&event);

			// Server update handling
			g_pServerGame->Update();
		}
	}

	// Event Update
	// ????

	// Server world update
	g_World.Update();

	// Rendering
	if (!s_bIsDedicated)
	{
		int windowSizeX = 0, windowSizeY = 0;
		SDL_GetWindowSize(m_pWindow, &windowSizeX, &windowSizeY);

		// install viewport
		g_pRenderDevice->SetViewport(0, 0, windowSizeX, windowSizeY);

		g_pRenderDevice->Clear(TST_COLOR | TST_DEPTH, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0xffffffff);

		g_World.Render();

		g_pClientGame->Render();

		// Present the backbuffer to screen
		g_pRender->Present();

		g_pRender->ResetStates();
	}

	endTime = startTime;
}

void Engine::Shutdown()
{
	g_pClientGame->Shutdown();

	g_pServerGame->Shutdown();

	if (g_pRender)
	{
		g_pRender->Shutdown();

		delete g_pRender;
		g_pRender = nullptr;
	}

	GetFileSystem()->Shutdown();

	GetLogger()->Shutdown();
}

SDL_Window* Engine::GetWindow()
{
	return m_pWindow;
}

#if defined(WIN32)
#include <Windows.h>
#define LIB_CLIENT "client.dll"
#define LIB_SERVER "server.dll"
#elif defined(_LINUX)
#define LIB_CLIENT "libclient.so"
#define LIB_SERVER "libserver.so"
#endif

#define LIB_DIR "bin/"
#define LIB_CLIENT_PATH LIB_DIR LIB_CLIENT
#define LIB_SERVER_PATH LIB_DIR LIB_SERVER

static HINSTANCE g_hClientDll = NULL;
static HINSTANCE g_hServerDll = NULL;

static const char* GetErrorString()
{
#ifdef WIN32
	static char buf[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, sizeof(buf), NULL);
	return buf;
#else
	return dlerror();
#endif 
}

void Engine::InitClientDll()
{
	g_hClientDll = LoadLibrary(LIB_CLIENT_PATH);
	if (!g_hClientDll)
		GetLogger()->Error("Couldn't load client.dll!\n%s", GetErrorString());

	pfnClientMain getClient = (pfnClientMain)GetProcAddress(g_hClientDll, "Client_Main");
	if (!getClient)
	{
		GetLogger()->Error(LIB_CLIENT " doesn't have exported Client_Main.\n%s", GetErrorString());
	}

	g_pClientGame = getClient();
}

void Engine::InitServerDll()
{
	g_hServerDll = LoadLibrary(LIB_SERVER_PATH);
	if (!g_hServerDll)
		GetLogger()->Error("Couldn't load server.dll!\n%s", GetErrorString());

	pfnServerMain getServer = (pfnServerMain)GetProcAddress(g_hServerDll, "Server_Main");
	if (!getServer)
	{
		GetLogger()->Error(LIB_SERVER " doesn't have exported Server_Main.\n%s", GetErrorString());
	}

	g_pServerGame = getServer();
}

float Engine::GetDeltaTime()
{
	return m_fDeltaTime;
}

IServerGame* Engine::GetServerGame()
{
	return g_pServerGame;
}

//static GPUBuffer* s_pTriangleBuffer = nullptr;
//static Shader* s_pShader = nullptr;
//static Model* s_pTestModel = nullptr;

//// Create vertex buffer
//
//float vertices[] = {
//	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
//	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
//	 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
//};
//
//s_pTriangleBuffer = g_pRenderDevice->CreateVertexBuffer(vertices, sizeof(vertices));
//
//// Create shader
//
//static InputLayoutDesc_t inputLayout[] =
//{
//	{ VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION },
//	{ VERTEXATTR_VEC4, SHADERSEMANTIC_COLOR },
//};
//
//s_pShader = g_pShaderSystem->CreateShader("test", "data/shaders/test.vs", "data/shaders/test.ps", inputLayout, sizeof(inputLayout) / sizeof(inputLayout[0]));
//
//// Load example model
//s_pTestModel = g_pModelSystem->LoadModel("data/models/test.obj");

//	// Clear screen
//	g_pRenderDevice->Clear(TST_COLOR | TST_DEPTH, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0xffffffff);

//	// Draw model
//	
//	static glm::mat4 identityMatrix = glm::mat4(1.0f);
//	s_pTestModel->Draw(identityMatrix);

//	// Draw triangle
//	g_pRenderDevice->SetVerticesBuffer(s_pTriangleBuffer);
//	g_pShaderSystem->SetShader(s_pShader);
//	g_pRenderDevice->DrawArrays(PT_TRIANGLES, 0, 3);
