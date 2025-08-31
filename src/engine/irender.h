// irender.h - Interface to a renderer.
#ifndef IRENDER_H
#define IRENDER_H

#include "render_shared.h"

class IShader;
class Model;

class ITexture2D
{
public:
	virtual ~ITexture2D() {}

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
};

class IGPUBuffer
{
public:
	virtual ~IGPUBuffer() {}

	virtual void* MapBuffer(BufferAccess access) = 0;
	virtual void UnmapBuffer() = 0;
	virtual void UpdateBuffer(void* data, size_t size) = 0;
};

class IRender
{
public:
	virtual IShader* CreateShader(const char* name, const char* vsfilepath, const char* psfilepath, InputLayoutDesc_t* inputLayout = nullptr, int inputLayoutCount = 0) = 0;
	virtual void SetShader(const IShader* shader) = 0;
	virtual void SetUniformSampler(const IShader* shader, ShaderSamplers_t sampler, int index) = 0;
	virtual void SetUniformFloat4(const IShader* shader, ShaderUniform_t uniform, const void* data) = 0;
	virtual void SetUniformMatrix(const IShader* shader, ShaderUniform_t uniform, const void* data) = 0;

	virtual IGPUBuffer* CreateVertexBuffer(void* pData, int size, bool dynamic) = 0;
	virtual IGPUBuffer* CreateIndexBuffer(void* pData, int size, bool dynamic) = 0;

	virtual void SetVertexBuffer(IGPUBuffer* buffer) = 0;
	virtual void SetIndexBuffer(IGPUBuffer* buffer) = 0;

	virtual ITexture2D* LoadTexture2D(const char* texturename, bool useMipmaps = false) = 0;
	virtual ITexture2D* CreateTexture2D(const char* name, uint32_t width, uint32_t height, PixelFormat format, void* pData = nullptr, bool useAsRenderTarget = false) = 0;
	virtual void SetTexture2D(int slot, ITexture2D* texture) = 0;

	virtual void SetDepthTest(bool enable) = 0;
	virtual void SetDepthWrite(bool enable) = 0;
	virtual void SetStencilTest(bool enable) = 0;
	virtual void SetScissorTest(bool enable) = 0;
	virtual void SetCullFace(bool enable) = 0;
	virtual void SetBlending(bool value) = 0;
	virtual void SetBlendingFunction(BlendFactor srcFactor, BlendFactor destFactor) = 0;
	virtual void SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) = 0;
	virtual void SetBlendEquation(BlendEquation equation) = 0;

	virtual void SetScissor(int x, int y, int width, int height) = 0;
	virtual void SetViewport(int x, int y, int w, int h) = 0;

	virtual void SetViewMatrix(const glm::mat4& matView) = 0;
	virtual void SetProjectionMatrix(const glm::mat4& matProjection) = 0;

	virtual const StaticMeshVertex* GetModelVertices(Model* pModel, int* pNumVertices) = 0;

	virtual void DrawIndexed(PrimitiveType mode, uint32_t count, bool is16bitIndices, const void* indices) = 0;
};

extern "C" __declspec(dllexport) IRender* GetRender();

#endif // !IRENDER_H
