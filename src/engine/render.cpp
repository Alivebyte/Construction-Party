#include "ilogger.h"
#include "render.h"
#include "renderdevice.h"
#include "texturesmanager.h"
#include "shadersystem.h"
#include "modelsystem.h"

static GLuint g_VAO = 0;

Render* g_pRender = nullptr;

Render::Render() :
	m_pWindow(nullptr),
	m_pGLContext(nullptr),
	m_pStretchedPicVBuf(nullptr),
	m_bUsingVAO(false)
{
}

Render::~Render()
{
}

void Render::Init(SDL_Window* pWindow)
{
	m_pWindow = pWindow;
	SDL_assert(m_pWindow);

	// Create OpenGL context
	m_pGLContext = SDL_GL_CreateContext(m_pWindow);
	SDL_GL_MakeCurrent(m_pWindow, m_pGLContext);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	if (GLVersion.major == 0 || GLVersion.minor == 0)
	{
		GetLogger()->Print("Failed to load OpenGL");
		SDL_assert(0);
	}

	// Core profile probably, should use VAO
	if (GLVersion.major >= 3 && GLVersion.minor >= 1)
		m_bUsingVAO = true;

	GetLogger()->Print("%s", (const char*)glGetString(GL_VENDOR));
	GetLogger()->Print("%s", (const char*)glGetString(GL_RENDERER));
	GetLogger()->Print("OpenGL ver. %s", (const char*)glGetString(GL_VERSION));
	GetLogger()->Print("Context created with OpenGL version %d.%d", GLVersion.major, GLVersion.minor);

	// Reset OpenGL error stack
	glGetError();

	// Create render device
	g_pRenderDevice = new RenderDevice();

	// Create texture manager
	g_pTexturesManager = new TexturesManager();
	g_pTexturesManager->Init();

	// Create shader system
	g_pShaderSystem = new ShaderSystem();
	g_pShaderSystem->Init();

	// Create model system
	g_pModelSystem = new ModelSystem();

	// Create stretched picture buffer
	m_pStretchedPicVBuf = g_pRenderDevice->CreateVertexBuffer(nullptr, MAX_STRETCH_VX, true);

	// Core profile require Vertex Array Object to render
	if (m_bUsingVAO)
	{
		// Create global vertex array
		glGenVertexArrays(1, &g_VAO);
		glBindVertexArray(g_VAO);
	}
}

void Render::Shutdown()
{
	if (m_bUsingVAO)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &g_VAO);
	}

	delete m_pStretchedPicVBuf;
	m_pStretchedPicVBuf = nullptr;

	g_pModelSystem->Shutdown();
	delete g_pModelSystem;
	g_pModelSystem = nullptr;

	g_pShaderSystem->Shutdown();
	delete g_pShaderSystem;
	g_pShaderSystem = nullptr;

	g_pTexturesManager->Shutdown();
	delete g_pTexturesManager;
	g_pTexturesManager = nullptr;

	delete g_pRenderDevice;
	g_pRenderDevice = nullptr;

	// Destroy OpenGL context
	SDL_GL_MakeCurrent(nullptr, nullptr);
	SDL_GL_DeleteContext(m_pGLContext);
	m_pGLContext = nullptr;
}

void Render::Present(bool vsync)
{
	SDL_GL_SwapWindow(m_pWindow);
}
