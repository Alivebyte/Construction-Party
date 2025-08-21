#include "shadersystem.h"
#include "shader.h"
#include "gl_shared.h"

static const char* g_uniformNameTable[UNIFORM_MAX] =
{
	"u_modelMatrix",
	"u_viewMatrix",
	"u_projectionMatrix",
	"u_modelViewProjection",
	"u_customColor",
	"u_sunDirection",
	"u_sunColor",
	"u_sunAmbientColor",
};

static const char* g_samplersNameTable[SAMPLER_MAX] =
{
	"u_albedoTexture",
	"u_normalTexture",
	"u_lightmapTexture",
};

static const char* g_attributeNameTable[SHADERSEMANTIC_MAX] =
{
	"a_position",//SHADERSEMANTIC_POSITION,
	"a_color", //SHADERSEMANTIC_COLOR
	"a_texcoord",//SHADERSEMANTIC_TEXCOORD,
	"a_texcoord0", //SHADERSEMANTIC_TEXCOORD0,
	"a_texcoord1",//SHADERSEMANTIC_TEXCOORD1,
	"a_normal",//SHADERSEMANTIC_NORMAL,
	"a_tangent",//SHADERSEMANTIC_TANGENT,
	"a_bitangent",//SHADERSEMANTIC_BITANGENT,
	"a_boneIds",//SHADERSEMANTIC_BONEIDS,
	"a_weights"//SHADERSEMANTIC_WEIGHTS,
};

static size_t g_attributeSizeTable[VERTEXATTR_MAX] =
{
	sizeof(float) * 2,//VERTEXATTR_VEC2,
	sizeof(float) * 3,//VERTEXATTR_VEC3,
	sizeof(float) * 4,//VERTEXATTR_VEC4,
};

size_t g_vertexAttribsSizeTable[VERTEXATTR_MAX] =
{
	2, // VERTEXATTR_VEC2
	3, // VERTEXATTR_VEC3
	4, // VERTEXATTR_VEC4
};

size_t g_vertexAttribsRealSizeTable[VERTEXATTR_MAX] =
{
	8, // VERTEXATTR_VEC2
	12, // VERTEXATTR_VEC3
	16, // VERTEXATTR_VEC4
};

ShaderSystem* g_pShaderSystem = nullptr;

ShaderSystem::ShaderSystem()
{
}

ShaderSystem::~ShaderSystem()
{
}

void ShaderSystem::Init()
{
}

void ShaderSystem::Shutdown()
{
	for (int i = 0; i < m_shaders.size(); i++)
	{
		if (m_shaders[i])
		{
			delete m_shaders[i];
			m_shaders[i] = nullptr;
		}
	}

	m_shaders.clear();
}

Shader* ShaderSystem::CreateShader(const char* name, const char* vsfilepath, const char* psfilepath, InputLayoutDesc_t* inputLayout /*= nullptr*/, int inputLayoutCount/* = 0*/)
{
	auto it = std::find_if(m_shaders.begin(), m_shaders.end(), [=](const Shader* shader) { return shader->GetName() == name; });
	if ( it != m_shaders.end() )
	{
		return *it;
	}

	Shader* pShader = new Shader();
	
	if ( inputLayout && inputLayoutCount > 0 )
	{
		memcpy( pShader->m_layouts, inputLayout, inputLayoutCount * sizeof( InputLayoutDesc_t ) );
		pShader->m_layoutCount = inputLayoutCount;
	}

	pShader->Create( name, vsfilepath, psfilepath );

	m_shaders.push_back(pShader);

	return pShader;
}

void ShaderSystem::SetShader(const Shader* shader)
{
	SDL_assert( shader );
	glUseProgram( shader->m_program );
	
	// apply input layout

	size_t appliedOffset = 0;
	for (int i = 0; i < shader->m_layoutCount; i++)
	{
		const InputLayoutDesc_t* layoutEntry = &shader->m_layouts[i];

		glEnableVertexAttribArray(GLuint(i));

		glVertexAttribPointer(GLuint(i), GLint(g_vertexAttribsSizeTable[layoutEntry->attribute]),
			GL_FLOAT, GL_FALSE, static_cast<GLsizei>(shader->m_stride),
			(appliedOffset > 0) ? (void*)(appliedOffset * sizeof(float)) : (void*)0);

		appliedOffset += g_vertexAttribsSizeTable[layoutEntry->attribute];
	}
}

void ShaderSystem::SetUniformSampler(const Shader* shader, ShaderSamplers_t sampler, int index)
{
	glUniform1i( shader->m_glSamplerLocation[ sampler ], static_cast< GLint >(index) );
}

void ShaderSystem::SetUniformFloat4(const Shader* shader, ShaderUniform_t uniform, const void* data)
{	
	glUniform4fv( shader->m_glUniformLocation[ uniform ], 1, ( const GLfloat* )data );
}

void ShaderSystem::SetUniformMatrix( const Shader* shader, ShaderUniform_t uniform, const void* data )
{
	glUniformMatrix4fv( shader->m_glUniformLocation[ uniform ], 1, GL_FALSE, ( const GLfloat* )data );
}

void Shader::AllocateAttributes()
{
	// Allocate input layout
	for ( int i = 0; i < m_layoutCount; i++ )
	{
		const InputLayoutDesc_t& layout = m_layouts[ i ];

		glBindAttribLocation( m_program, i, g_attributeNameTable[ layout.semantic ] );

		GL_CHECK_ERROR();

		// add element size to stride
		m_stride += (int)g_attributeSizeTable[ layout.attribute ];
	}
}

void Shader::AllocateUniforms()
{
	// parse shader uniforms
	for (int i = 0; i < UNIFORM_MAX; i++)
	{
		m_glUniformLocation[i] = glGetUniformLocation(m_program, g_uniformNameTable[i]);
	}

	// parse shader samplers
	for (int i = 0; i < SAMPLER_MAX; i++)
	{
		m_glSamplerLocation[i] = glGetUniformLocation(m_program, g_samplersNameTable[i]);
	}
}
