#ifndef RENDERDEVICE_H
#define RENDERDEVICE_H

#include <stdint.h>
#include "render_shared.h"

class GPUBuffer;
class VertexFormat;
class RenderTarget;

enum TextureSurfaceType
{
	TST_COLOR = 1 << 0,
	TST_DEPTH = 1 << 1,
	TST_STENCIL = 1 << 2,
};

enum PrimitiveType
{
	PT_POINTS,
	PT_LINES,
	PT_TRIANGLES
};

class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();

	GPUBuffer* CreateVertexBuffer(void* data, size_t size, bool isStream = false);
	GPUBuffer* CreateIndexBuffer(void* data, size_t size, bool isStream = false);

	void SetVerticesBuffer(GPUBuffer* buffer);
	void SetIndicesBuffer(GPUBuffer* buffer);

	void SetDepthTest(bool enable);
	void SetDepthWrite(bool enable);

	void SetBlending(bool value);
	void SetBlendingFunction(BlendFactor srcFactor, BlendFactor destFactor);

	void SetViewport(int x, int y, int w, int h);

	void Clear(uint32_t surfaces, float r, float g, float b, float a, float z, uint32_t stencil);

	// drawing
	void DrawArrays(PrimitiveType primType, uint32_t startOf, size_t verticesCount);
	void DrawElements(PrimitiveType primType, size_t elementsCount, bool is16bitIndices);

private:
	GPUBuffer* m_activeVB;
	GPUBuffer* m_activeIB;

	RenderTarget* m_activeReadRT;
	RenderTarget* m_activeWriteRT;

	bool m_blending;
	BlendFactor m_srcBlendFactor;
	BlendFactor m_destBlendFactor;
};

extern RenderDevice* g_pRenderDevice;

#endif // !RENDERDEVICE_H
