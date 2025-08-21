#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <string>
#include "gl_shared.h"

const int SHADERUNIFORM_MAX_COUNT = 16;
const int INPUT_LAYOUT_MAX_COUNT = 8;

enum VertexAttribute_t {
	VERTEXATTR_VEC2,
	VERTEXATTR_VEC3,
	VERTEXATTR_VEC4,

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

class ShaderSystem;

class Shader
{
	friend class ShaderSystem;
public:
	Shader();
	~Shader();

	void Create(const char* name, const char* vsfilepath, const char* psfilepath);
	void Destroy();

	void AllocateAttributes();
	void AllocateUniforms();

	bool HasUniform(ShaderUniform_t uniform) { return m_glUniformLocation[uniform] != -1; }

private:
	// TEMP SOLUTION
	ShaderUniformDesc_t m_uniformDesc[SHADERUNIFORM_MAX_COUNT];
	size_t m_uniformCount;

	InputLayoutDesc_t m_layouts[INPUT_LAYOUT_MAX_COUNT];
	size_t m_layoutCount;

	GLint m_glUniformLocation[SHADERSEMANTIC_MAX];
	GLint m_glSamplerLocation[SAMPLER_MAX];

	GLuint m_glLayouts[INPUT_LAYOUT_MAX_COUNT];

	std::string m_name;

	int m_stride;

	GLuint m_program;
};

inline int GetVertexAttributeSize( VertexAttribute_t attrib )
{
	switch ( attrib )
	{
	case VERTEXATTR_VEC2:
		return 2;
	case VERTEXATTR_VEC3:
		return 3;
	case VERTEXATTR_VEC4:
		return 4;
	default:
		break;
	}

	return 0;
}

#endif // !SHADER_H
