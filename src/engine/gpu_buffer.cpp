#include "ilogger.h"
#include "gl_shared.h"
#include "render_shared.h"
#include "gpu_buffer.h"

GLenum BufferTypeToTarget(BufferType type)
{
	switch (type)
	{
	case BT_VERTEX:
		return GL_ARRAY_BUFFER;

	case BT_INDEX:
		return GL_ELEMENT_ARRAY_BUFFER;
	
	default:
		break;
	};

	SDL_assert(0 && "Unknowed buffer type");
	return 0;
}

GPUBuffer::GPUBuffer(BufferType type, void* data, size_t size, bool isStream /*= false*/)
{
	m_type = type;
	m_target = BufferTypeToTarget(m_type);
	m_isStream = isStream;
	m_size = size;

	glGenBuffers(1, &m_buffer);
	glBindBuffer(m_target, m_buffer);
	glBufferData(m_target, size, data, isStream ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	GL_CHECK_ERROR();
	glBindBuffer(m_target, 0);
}

GPUBuffer::~GPUBuffer()
{
	glDeleteBuffers(1, &m_buffer);
}

void GPUBuffer::Bind()
{
	glBindBuffer(m_target, m_buffer);
}

void* GPUBuffer::MapBuffer(BufferAccess access)
{
	GLenum accessGl = 0;

	switch (access)
	{
	case BA_READ_ONLY:
		accessGl = GL_READ_ONLY;
		break;
	case BA_WRITE_ONLY:
		accessGl = GL_WRITE_ONLY;
		break;
	case BA_READ_WRITE:
		accessGl = GL_READ_WRITE;
		break;
	}

	Bind();

	void* ptr = glMapBuffer(m_target, accessGl);

	GL_CHECK_ERROR();

	return ptr;
}

void GPUBuffer::UnmapBuffer()
{
	Bind();

	glUnmapBuffer(m_target);
}

void GPUBuffer::UpdateBuffer(void* data, size_t size)
{
	SDL_assert(m_isStream && "Trying to update static buffer.");

	// Clear GL errors
	glGetError();

	// Bind buffer
	Bind();

	glBufferData(m_target, size, data, GL_DYNAMIC_DRAW);

	GLenum error = glGetError();
	if (error == GL_OUT_OF_MEMORY)
	{
		GetLogger()->Print("GPUBuffer::UpdateData: Out of memory. Couldn't allocate buffer with %u size", (unsigned)size);
		return;
	}

	m_size = size;
}
