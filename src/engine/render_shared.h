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

enum BlendEquation
{
	BE_FUNC_ADD,
	BE_FUNC_SUBTRACT,
	BE_FUNC_REVERSE_SUBTRACT,
	BE_MIN,
	BE_MAX
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

// Blending equation convertion
uint32_t GetGLEquation(BlendEquation equation);

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

const int SHADERUNIFORM_MAX_COUNT = 16;
const int INPUT_LAYOUT_MAX_COUNT = 8;

enum VertexAttribute_t {
	VERTEXATTR_VEC2,
	VERTEXATTR_VEC3,
	VERTEXATTR_VEC4,
	VERTEXATTR_UINT,

	VERTEXATTR_MAX
};

enum ShaderSemantic_t {
	SHADERSEMANTIC_POSITION,
	SHADERSEMANTIC_COLOR,
	SHADERSEMANTIC_TEXCOORD,
	SHADERSEMANTIC_TEXCOORD0,
	SHADERSEMANTIC_TEXCOORD1,
	SHADERSEMANTIC_NORMAL,
	SHADERSEMANTIC_TANGENT,
	SHADERSEMANTIC_BITANGENT,
	SHADERSEMANTIC_BONEIDS,
	SHADERSEMANTIC_WEIGHTS,

	SHADERSEMANTIC_MAX
};

enum ShaderUniformType_t {
	SHADERUNIFORM_FLOAT,
	SHADERUNIFORM_VEC2,
	SHADERUNIFORM_VEC3,
	SHADERUNIFORM_VEC4,
	SHADERUNIFORM_MAT4,

	SHADERUNIFORM_MAX
};

enum ShaderUniform_t
{
	UNIFORM_MODEL_MATRIX,
	UNIFORM_VIEW_MATRIX,
	UNIFORM_PROJ_MATRIX,
	UNIFORM_MVP_MATRIX,
	UNIFORM_CUSTOM_COLOR,
	UNIFORM_SUN_DIRECTION,
	UNIFORM_SUN_COLOR,
	UNIFORM_SUN_AMBIENT,

	UNIFORM_MAX,
};

enum ShaderSamplers_t
{
	SAMPLER_ALBEDO,
	SAMPLER_NORMAL,
	SAMPLER_LIGHTMAP,

	SAMPLER_MAX
};

struct InputLayoutDesc_t {
	VertexAttribute_t attribute;
	ShaderSemantic_t semantic;
};

struct ShaderUniformDesc_t {
	ShaderUniformType_t type;
	const char* name;
	size_t size;
};

#endif