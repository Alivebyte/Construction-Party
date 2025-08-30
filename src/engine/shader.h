#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <string>
#include "render_shared.h"
#include "gl_shared.h"

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

	const std::string& GetName() const { return m_name; }

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
