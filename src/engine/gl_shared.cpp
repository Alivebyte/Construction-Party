#include "gl_shared.h"
#include "ilogger.h"

const char *GL_ErrorString( int err )
{
	switch( err )
	{
	case GL_STACK_OVERFLOW:
		return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:
		return "GL_STACK_UNDERFLOW";
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	default:
		return "UNKNOWN ERROR";
	}
}

void GL_CheckError()
{
	GLenum err;
	if ( (err = glGetError()) != GL_NO_ERROR )
		GetLogger()->Print( "OpenGL Error: %s", GL_ErrorString( err ) );// Msg("OpenGL Error: %s [%s]\n", GL_ErrorString(err), GL_TargetToString(tex->target));
}

void GL_CheckErrorEx( const char* filename, int line )
{
	GLenum err;
	if ( (err = glGetError()) != GL_NO_ERROR )
		GetLogger()->Print( "OpenGL Error: %s at %s:%i", GL_ErrorString( err ), filename, line );// Msg("OpenGL Error: %s [%s]\n", GL_ErrorString(err), GL_TargetToString(tex->target));
}

void GL_CheckErrorFunction(const char* expression, const char* filename, int line)
{
	GLenum err;
	if ( (err = glGetError()) != GL_NO_ERROR )
		GetLogger()->Print( "OpenGL Error: %s (%s) at %s:%i", expression, GL_ErrorString( err ), filename, line );// Msg("OpenGL Error: %s [%s]\n", GL_ErrorString(err), GL_TargetToString(tex->target));
}

// Conversion functions

#include "render_shared.h"

uint32_t GetGLInternalPF(PixelFormat pf)
{
	switch (pf)
	{
	case PF_UNKNOWN:
		return 0;

	case PF_R8G8B8:
	case PF_R8G8B8F:
		return GL_RGB;

	case PF_R8G8B8A8:
	case PF_R8G8B8A8F:
		return GL_RGBA;
	}

	return 0;
}

uint32_t GetGLTypePF(PixelFormat pf)
{
	switch (pf)
	{
	case PF_UNKNOWN:
		return 0;

	case PF_R8G8B8:
	case PF_R8G8B8A8:
		return GL_UNSIGNED_BYTE;

	case PF_R8G8B8F:
	case PF_R8G8B8A8F:
		return GL_FLOAT;
	}

	return 0;
}

uint32_t GetGLBlendFactor(BlendFactor factor)
{
	switch (factor)
	{
	case BF_ZERO:
		return GL_ZERO;
	case BF_ONE:
		return GL_ONE;
	case BF_SRC_COLOR:
		return GL_SRC_COLOR;
	case BF_ONE_MINUS_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case BF_DST_COLOR:
		return GL_DST_COLOR;
	case BF_ONE_MINUS_DST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case BF_SRC_ALPHA:
		return GL_SRC_ALPHA;
	case BF_ONE_MINUS_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case BF_DST_ALPHA:
		return GL_DST_ALPHA;
	case BF_ONE_MINUS_DST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case BF_CONSTANT_COLOR:
		return GL_CONSTANT_COLOR;
	case BF_ONE_MINUS_CONSTANT_COLOR:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case BF_CONSTANT_ALPHA:
		return GL_CONSTANT_ALPHA;
	case BF_ONE_MINUS_CONSTANT_ALPHA:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	default:
		break;
	}

	return 0;
}

uint32_t GetGLDepthFunc(DepthFunc func)
{
	switch (func)
	{
	case DF_NEVER:
		return GL_NEVER;
	case DF_LESS:
		return GL_LESS;
	case DF_EQUAL:
		return GL_EQUAL;
	case DF_LEQUAL:
		return GL_LEQUAL;
	case DF_GREATER:
		return GL_GREATER;
	case DF_NOTEQUAL:
		return GL_NOTEQUAL;
	case DF_GEQUAL:
		return GL_GEQUAL;
	case DF_ALWAYS:
		return GL_ALWAYS;
	default:
		break;
	}

	SDL_assert(0 && "Should be never here");
	return 0;
}

int getGlWrap(TextureWrap wrap)
{
	int param = 0;

	if (wrap == TextureWrap_Repeat)
		param = GL_REPEAT;
	else if (wrap == TextureWrap_MirroredRepeat)
		param = GL_MIRRORED_REPEAT;
	else if (wrap == TextureWrap_ClampToEdge)
		param = GL_CLAMP_TO_EDGE;
	else if (wrap == TextureWrap_ClampToBorder)
		param = GL_CLAMP_TO_BORDER;

	return param;
}

int getGlTexFilter(TextureFilter filter)
{
	int param = 0;

	if (filter == TextureFilter_Linear)
		param = GL_LINEAR;
	else if (filter == TextureFilter_Nearest)
		param = GL_NEAREST;
	else if (filter == TextureFilter_LinearMipmapLinear)
		param = GL_LINEAR_MIPMAP_LINEAR;
	else if (filter == TextureFilter_LinearMipmapNearest)
		param = GL_LINEAR_MIPMAP_NEAREST;
	else if (filter == TextureFilter_NearestMipmapLinear)
		param = GL_NEAREST_MIPMAP_LINEAR;
	else if (filter == TextureFilter_NearestMipmapNearest)
		param = GL_NEAREST_MIPMAP_NEAREST;

	return param;
}
