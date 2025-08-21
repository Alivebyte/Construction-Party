#ifndef SHADERSYSTEM_H
#define SHADERSYSTEM_H

#include <string>
#include <vector>

#include "shader.h"

class ShaderSystem
{
public:
	ShaderSystem();
	~ShaderSystem();

	void Init();
	void Shutdown();

	Shader* CreateShader(const char* name, const char* vsfilepath, const char* psfilepath, InputLayoutDesc_t* inputLayout = nullptr, int inputLayoutCount = 0);

	void SetShader(const Shader* shader);

	void SetUniformSampler( const Shader* shader, ShaderSamplers_t sampler, int index );
	void SetUniformFloat4( const Shader* shader, ShaderUniform_t uniform, const void* data );
	void SetUniformMatrix( const Shader* shader, ShaderUniform_t uniform, const void* data );

private:
	struct ShaderData
	{
		std::string shadername;
		Shader* shader;
	};

	std::vector<ShaderData> m_shaders;
};

extern ShaderSystem* g_pShaderSystem;

#endif // !SHADERSYSTEM_H
