#include "shader.h"
#include "ifilesystem.h"
#include "ilogger.h"

#include <string>

GLuint CreateShader(GLenum shaderType, const char* filename)
{
	FileHandle_t file = GetFileSystem()->OpenFile(filename, "rb");
	if (file == INVALID_FILE_HANDLE_VALUE)
	{
		GetLogger()->Print("CreateShader: failed to open file %s", filename);
	}

	size_t length = GetFileSystem()->GetFileLength(file);

	std::string content;
	content.resize(length + 1);
	GetFileSystem()->ReadFile(file, (void*)content.data(), length);

	content[length] = '\0';

	GetFileSystem()->CloseFile(file);

	const char* contentCStr = content.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &contentCStr, NULL);
	glCompileShader(shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		GetLogger()->Print("Failed to compile shader %s\n%s", filename, infoLog);
	}

	GetLogger()->Print("created shader from file %s", filename);
	return shader;
}

Shader::Shader() :
	m_name(nullptr),
	m_stride(0),
	m_layoutCount(0)
{

}

Shader::~Shader()
{
	Destroy();
}

void Shader::Create(const char* name, const char* vsfilepath, const char* psfilepath)
{
	m_name = name;

	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vsfilepath);
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, psfilepath);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	AllocateAttributes();
	glLinkProgram(m_program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int  success;
	char infoLog[512];
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_program, 512, NULL, infoLog);
		GetLogger()->Print("Failed to link program %s", infoLog);
	}

	AllocateUniforms();
}

void Shader::Destroy()
{
	glDeleteProgram(m_program);
}
