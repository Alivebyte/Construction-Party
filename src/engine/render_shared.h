// Shared header for render subsystem
#ifndef RENDER_SHARED_H
#define RENDER_SHARED_H

#include <stdint.h>

#include <glm/glm.hpp>

typedef uint32_t index_t;

enum BufferType
{
	BT_VERTEX,
	BT_INDEX,
	
	BT_MAX
};

enum BufferAccess
{
	BA_READ_ONLY,
	BA_WRITE_ONLY,
	BA_READ_WRITE,

	BA_MAX
};

// texture format
enum PixelFormat
{
	PF_UNKNOWN,
	PF_R8G8B8,
	PF_R8G8B8A8,
	PF_R8G8B8F,
	PF_R8G8B8A8F,

	// Depth formats
	PF_DEPTH32F
};

// Blending functions
enum BlendFactor
{
	BF_ZERO,
	BF_ONE,
	BF_SRC_COLOR,
	BF_ONE_MINUS_SRC_COLOR,
	BF_DST_COLOR,
	BF_ONE_MINUS_DST_COLOR,
	BF_SRC_ALPHA,
	BF_ONE_MINUS_SRC_ALPHA,
	BF_DST_ALPHA,
	BF_ONE_MINUS_DST_ALPHA,
	BF_CONSTANT_COLOR,
	BF_ONE_MINUS_CONSTANT_COLOR,
	BF_CONSTANT_ALPHA,
	BF_ONE_MINUS_CONSTANT_ALPHA
};

enum DepthFunc
{
	DF_NEVER, // Never passes.
	DF_LESS, // Passes if the incoming depth value is less than the stored depth value.
	DF_EQUAL, // Passes if the incoming depth value is equal to the stored depth value.
	DF_LEQUAL, // Passes if the incoming depth value is less than or equal to the stored depth value.
	DF_GREATER, // Passes if the incoming depth value is greater than the stored depth value.
	DF_NOTEQUAL, // Passes if the incoming depth value is not equal to the stored depth value.
	DF_GEQUAL, // Passes if the incoming depth value is greater than or equal to the stored depth value.
	DF_ALWAYS, // Always passes.
	DF_MAX
};

enum TextureWrap
{
	TextureWrap_Repeat,
	TextureWrap_MirroredRepeat,
	TextureWrap_ClampToEdge,
	TextureWrap_ClampToBorder
};

enum TextureFilter
{
	TextureFilter_Nearest,
	TextureFilter_Linear,
	TextureFilter_NearestMipmapNearest,
	TextureFilter_LinearMipmapNearest,
	TextureFilter_NearestMipmapLinear,
	TextureFilter_LinearMipmapLinear
};

// pixel format convertion
uint32_t GetGLPF(PixelFormat pf);
uint32_t GetGLInternalPF(PixelFormat pf);
uint32_t GetGLTypePF(PixelFormat pf);

// Wrapping and Filter convertion
int getGlWrap(TextureWrap wrap);
int getGlTexFilter(TextureFilter filter);

uint32_t GetGLDepthFunc(DepthFunc func);

// Blending factor convertion
uint32_t GetGLBlendFactor(BlendFactor factor);

struct Viewport
{
	int x;
	int y;
	int width;
	int height;
};

// Base structure for render view (view and projection matrices, viewport settings)
struct View
{
	//glm::mat4 view;
	//glm::mat4 proj;

	int x, y;
	int width, height;

	float fov;
	bool ortho;
};

// Stretched picture vertex
struct StretchedVertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec4 color;
};

#define MAX_STRETCH_VX 12 * sizeof(StretchedVertex)

#endif