#ifndef GL_SHARED_H
#define GL_SHARED_H

#include <SDL.h>
#include <glad/glad.h>

void GL_CheckError();
void GL_CheckErrorEx(const char* filename, int line);
void GL_CheckErrorFunction(const char* expression, const char* filename, int line);

#define GL_CHECK_ERROR() \
	GL_CheckErrorEx(__FILE__, __LINE__)

#define GL_CHECK_FUNC_ERROR(expr) \
	expr; \
	GL_CheckErrorFunction(#expr, __FILE__, __LINE__)

#endif // !GL_SHARED_H
