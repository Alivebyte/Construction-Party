#ifndef RENDER_H
#define RENDER_H

#include "irender.h"
#include "gl_shared.h"
#include "render_shared.h"

class GPUBuffer;

class Render : public IRender
{
public:
	Render();
	~Render();

	void Init(SDL_Window* pWindow);
	void Shutdown();

	void Present(bool vsync = false);

	void ResetStates();

	// Inherited via IRender
	IShader* CreateShader(const char* name, const char* vsfilepath, const char* psfilepath, InputLayoutDesc_t* inputLayout, int inputLayoutCount) override;
	void SetShader(const IShader* shader) override;
	void SetUniformSampler(const IShader* shader, ShaderSamplers_t sampler, int index) override;
	void SetUniformFloat4(const IShader* shader, ShaderUniform_t uniform, const void* data) override;
	void SetUniformMatrix(const IShader* shader, ShaderUniform_t uniform, const void* data) override;

	IGPUBuffer* CreateVertexBuffer(void* pData, int size, bool dynamic) override;
	IGPUBuffer* CreateIndexBuffer(void* pData, int size, bool dynamic) override;

	ITexture2D* CreateTexture2D(const char* name, uint32_t width, uint32_t height, PixelFormat format, void* pData, bool useAsRenderTarget) override;
	void SetTexture2D(int slot, ITexture2D* texture) override;

private:
	SDL_Window* m_pWindow;
	SDL_GLContext m_pGLContext;

	GPUBuffer* m_pStretchedPicVBuf;

	bool m_bUsingVAO;

	// Inherited via IRender
	void SetViewport(int x, int y, int w, int h) override;

	// Inherited via IRender
	void SetVertexBuffer(IGPUBuffer* buffer) override;
	void SetIndexBuffer(IGPUBuffer* buffer) override;

	// Inherited via IRender
	void SetScissor(int x, int y, int width, int height) override;

	// Inherited via IRender
	void DrawIndexed(PrimitiveType mode, uint32_t count, bool is16bitIndices, const void* indices) override;

	// Inherited via IRender
	void SetDepthTest(bool enable) override;
	void SetDepthWrite(bool enable) override;
	void SetStencilTest(bool enable) override;
	void SetScissorTest(bool enable) override;
	void SetBlending(bool value) override;
	void SetBlendingFunction(BlendFactor srcFactor, BlendFactor destFactor) override;
	void SetBlendEquation(BlendEquation equation) override;

	// Inherited via IRender
	void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) override;

	// Inherited via IRender
	void SetCullFace(bool enable) override;

	// Inherited via IRender
	ITexture2D* LoadTexture2D(const char* texturename, bool useMipmaps) override;
};

extern Render* g_pRender;

// TEMP
extern glm::vec3 g_viewOrigin;
extern glm::vec3 g_viewOrient;

#endif // !RENDER_H
