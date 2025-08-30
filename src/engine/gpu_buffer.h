#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include "render_shared.h"
#include "irender.h"

class RenderDevice;

class GPUBuffer : public IGPUBuffer
{
	friend class RenderDevice;
public:
	~GPUBuffer();

	void Bind();

	void* MapBuffer(BufferAccess access);
	void  UnmapBuffer();

	void UpdateBuffer(void* data, size_t size);

private:
	GPUBuffer(BufferType type, void* data, size_t size, bool isStream = false);

	BufferType m_type;
	uint32_t m_target;
	uint32_t m_buffer;
	size_t m_size;

	bool m_isStream;
};

#endif // !GPU_BUFFER_H
