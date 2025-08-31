#include "ilogger.h"
#include "render.h"
#include "renderdevice.h"
#include "texturesmanager.h"
#include "shadersystem.h"
#include "modelsystem.h"
#include "debugrender.h"

static GLuint g_VAO = 0;

// TEMP
glm::vec3 g_viewOrigin;
glm::vec3 g_viewOrient;

#ifdef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT                   0x92E0
#endif // GL_DEBUG_OUTPUT

#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB 0x8242
#endif // !GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB

#ifndef GL_DEBUG_TYPE_ERROR_ARB
#define GL_DEBUG_TYPE_ERROR_ARB 0x824C
#endif // !GL_DEBUG_TYPE_ERROR_ARB

#ifndef GL_ARB_debug_output
#define GL_ARB_debug_output 1
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLARBPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControlARB;
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTARBPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
PFNGLDEBUGMESSAGEINSERTARBPROC glDebugMessageInsertARB;
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKARBPROC)(GLDEBUGPROCARB callback, const void* userParam);
PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB;
typedef GLuint(APIENTRYP PFNGLGETDEBUGMESSAGELOGARBPROC)(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);
PFNGLGETDEBUGMESSAGELOGARBPROC glGetDebugMessageLogARB;
#endif

void APIENTRY GL_DebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// Nvidia spam too much about buffer mapping
	if (type == 0x8251)
		return;

	GetLogger()->Print("OpenGL: %stype = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR_ARB ? "** GL ERROR ** " : ""),
		type, severity, message);

	if (type == GL_DEBUG_TYPE_ERROR_ARB)
	{
		bool debug = true;
	}
}

Render* g_pRender = nullptr;

Render::Render() :
	m_pWindow(nullptr),
	m_pGLContext(nullptr),
	m_pStretchedPicVBuf(nullptr),
	m_bUsingVAO(false)
{
	m_ViewMatrix = glm::identity<glm::mat4>();
	m_ProjectionMatrix = glm::identity<glm::mat4>();
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
		GetLogger()->Error("Failed to load OpenGL");
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

	// Load extension
	glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)SDL_GL_GetProcAddress("glDebugMessageCallbackARB");

	// Enable debug output
	if (glDebugMessageCallbackARB)
	{
		GetLogger()->Print("...found GL_ARB_debug_output");

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(GL_DebugOutput, NULL);
	}


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

	// Create debug render
	g_pDebugRender = new DebugRender();
	g_pDebugRender->Initialize();

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

	g_pDebugRender->Shutdown();
	delete g_pDebugRender;
	g_pDebugRender = nullptr;

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

void Render::ResetStates()
{
	g_pRenderDevice->SetDepthTest(false);
	g_pRenderDevice->SetDepthWrite(false);
	g_pRenderDevice->SetStencilTest(false);
	g_pRenderDevice->SetScissorTest(false);
	g_pRenderDevice->SetCullFace(false);
	g_pRenderDevice->SetBlending(false);
}

IShader* Render::CreateShader(const char* name, const char* vsfilepath, const char* psfilepath, InputLayoutDesc_t* inputLayout, int inputLayoutCount)
{
	return (IShader*)g_pShaderSystem->CreateShader(name, vsfilepath, psfilepath, inputLayout, inputLayoutCount);
}

IGPUBuffer* Render::CreateVertexBuffer(void* pData, int size, bool dynamic)
{
	return (IGPUBuffer*)g_pRenderDevice->CreateVertexBuffer(pData, size, dynamic);
}

IGPUBuffer* Render::CreateIndexBuffer(void* pData, int size, bool dynamic)
{
	return (IGPUBuffer*)g_pRenderDevice->CreateIndexBuffer(pData, size, dynamic);
}

ITexture2D* Render::CreateTexture2D(const char* name, uint32_t width, uint32_t height, PixelFormat format, void* pData, bool useAsRenderTarget)
{
	return (ITexture2D*)g_pTexturesManager->CreateManual2D(name, width, height, format, pData, useAsRenderTarget);
}

void Render::SetTexture2D(int slot, ITexture2D* texture)
{
	g_pTexturesManager->SetTexture(slot, (Texture2D*)texture);
}

void Render::SetViewport(int x, int y, int w, int h)
{
	g_pRenderDevice->SetViewport(x, y, w, h);
}

void Render::SetVertexBuffer(IGPUBuffer* buffer)
{
	g_pRenderDevice->SetVerticesBuffer((GPUBuffer*)buffer);
}

void Render::SetIndexBuffer(IGPUBuffer* buffer)
{
	g_pRenderDevice->SetIndicesBuffer((GPUBuffer*)buffer);
}

void Render::SetScissor(int x, int y, int width, int height)
{
	glScissor(x, y, width, height);
}

void Render::DrawIndexed(PrimitiveType mode, uint32_t count, bool is16bitIndices, const void* indices)
{
	glDrawElements(GL_TRIANGLES, count, is16bitIndices ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, indices);

	GL_CHECK_ERROR();
}

void Render::SetDepthTest(bool enable)
{
	g_pRenderDevice->SetDepthTest(enable);
}

void Render::SetDepthWrite(bool enable)
{
	g_pRenderDevice->SetDepthWrite(enable);
}

void Render::SetStencilTest(bool enable)
{
	g_pRenderDevice->SetStencilTest(enable);
}

void Render::SetScissorTest(bool enable)
{
	g_pRenderDevice->SetScissorTest(enable);
}

void Render::SetBlending(bool value)
{
	g_pRenderDevice->SetBlending(value);
}

void Render::SetBlendingFunction(BlendFactor srcFactor, BlendFactor destFactor)
{
	g_pRenderDevice->SetBlendingFunction(srcFactor, destFactor);
}

void Render::SetBlendEquation(BlendEquation equation)
{
	g_pRenderDevice->SetBlendEquation(equation);
}

void Render::SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha)
{
	g_pRenderDevice->SetBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void Render::SetCullFace(bool enable)
{
	g_pRenderDevice->SetCullFace(enable);
}

ITexture2D* Render::LoadTexture2D(const char* texturename, bool useMipmaps)
{
	return (ITexture2D*)g_pTexturesManager->LoadTexture2D(texturename, useMipmaps);
}

void Render::SetViewMatrix(const glm::mat4& matView)
{
	m_ViewMatrix = matView;
}

void Render::SetProjectionMatrix(const glm::mat4& matProjection)
{
	m_ProjectionMatrix = matProjection;
}

void Render::SetShader(const IShader* shader)
{
	g_pShaderSystem->SetShader((const Shader*)shader);
}

void Render::SetUniformSampler(const IShader* shader, ShaderSamplers_t sampler, int index)
{
	g_pShaderSystem->SetUniformSampler((const Shader*)shader, sampler, index);
}

void Render::SetUniformFloat4(const IShader* shader, ShaderUniform_t uniform, const void* data)
{
	g_pShaderSystem->SetUniformFloat4((const Shader*)shader, uniform, data);
}

void Render::SetUniformMatrix(const IShader* shader, ShaderUniform_t uniform, const void* data)
{
	g_pShaderSystem->SetUniformMatrix((const Shader*)shader, uniform, data);
}

IRender* GetRender()
{
	return g_pRender;
}