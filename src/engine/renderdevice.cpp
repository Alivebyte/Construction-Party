#include <assert.h>
#include <stdio.h>

// OpenGL
#include "gl_shared.h"

#include "gpu_buffer.h"
#include "renderdevice.h"

static GLenum g_glPrimitiveMode[PT_TRIANGLES + 1] = {
	GL_POINTS,
	GL_LINES,
	GL_TRIANGLES
};

// The render device instance.
RenderDevice* g_pRenderDevice = nullptr;

RenderDevice::RenderDevice()
{
	m_activeVB = nullptr;
	m_activeIB = nullptr;
	m_blending = false;
	m_activeReadRT = nullptr;
	m_activeWriteRT = nullptr;
}

RenderDevice::~RenderDevice()
{
}

GPUBuffer* RenderDevice::CreateVertexBuffer(void* data, size_t size, bool isStream)
{
	return new GPUBuffer(BT_VERTEX, data, size, isStream);
}

GPUBuffer* RenderDevice::CreateIndexBuffer(void* data, size_t size, bool isStream)
{
	return new GPUBuffer(BT_INDEX, data, size, isStream);
}

void RenderDevice::SetVerticesBuffer(GPUBuffer* buffer)
{
	if (buffer) {
		if (m_activeVB != buffer) {
			m_activeVB = buffer;
			m_activeVB->Bind();
		}
	} else { // unbind buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void RenderDevice::SetIndicesBuffer(GPUBuffer* buffer)
{
	if (buffer) {
		if (m_activeIB != buffer) {
			m_activeIB = buffer;
			m_activeIB->Bind();
		}
	} else { // unbind buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void RenderDevice::SetDepthTest(bool enable)
{
	enable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void RenderDevice::SetDepthWrite(bool enable)
{
	glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void RenderDevice::SetStencilTest(bool enable)
{
	enable ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
}

void RenderDevice::SetScissorTest(bool enable)
{
	enable ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}

void RenderDevice::SetCullFace(bool enable)
{
	enable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void RenderDevice::SetBlending(bool value)
{
	if (m_blending != value) {
		m_blending = value;

		value ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}
}

void RenderDevice::SetBlendingFunction(BlendFactor srcFactor, BlendFactor destFactor)
{
	// Switch state if one of two blending factors was changed
	if (srcFactor != m_srcBlendFactor || destFactor != m_destBlendFactor)
	{
		m_srcBlendFactor = srcFactor;
		m_destBlendFactor = destFactor;

		// push to gl state
		glBlendFunc(GetGLBlendFactor(srcFactor), GetGLBlendFactor(destFactor));
	}
}

void RenderDevice::SetBlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha)
{
	glBlendFuncSeparate(GetGLBlendFactor(srcRGB), GetGLBlendFactor(dstRGB),
		GetGLBlendFactor(srcAlpha), GetGLBlendFactor(dstAlpha));
}

void RenderDevice::SetBlendEquation(BlendEquation equation)
{
	glBlendEquation(GetGLEquation(equation));
}

void RenderDevice::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void RenderDevice::Clear(uint32_t surfaces, float r, float g, float b, float a, float z, uint32_t stencil)
{
	GLenum flags = 0;

	if (surfaces & TST_COLOR)
	{
		flags |= GL_COLOR_BUFFER_BIT;
		glClearColor(r, g, b, a);
	}

	if (surfaces & TST_DEPTH)
	{
		flags |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(z);
	}

	if (surfaces & TST_STENCIL)
	{
		flags |= GL_STENCIL_BUFFER_BIT;
		glClearStencil((GLint)stencil);
	}

	glClear(flags);
}

void RenderDevice::DrawArrays(PrimitiveType primType, uint32_t startOf, size_t verticesCount)
{
	glDrawArrays(g_glPrimitiveMode[primType], startOf, GLsizei(verticesCount));
}

void RenderDevice::DrawElements(PrimitiveType primType, size_t elementsCount, bool is16bitIndices)
{
	glDrawElements(g_glPrimitiveMode[primType], GLsizei(elementsCount), is16bitIndices ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, NULL);
}
