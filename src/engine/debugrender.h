#ifndef DEBUGRENDER_H
#define DEBUGRENDER_H

#include <vector>
#include "iengine.h"
#include "render_shared.h"

class GPUBuffer;
class Shader;

class ENGINE_API DebugRender
{
public:
	DebugRender();
	~DebugRender();

	void Initialize();
	void Shutdown();

	void DrawAxis(const glm::vec3& vec);
	void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);

	void RenderFrame();

private:

	void DrawLinesInternal();

private:
	// Primitives
	struct Line
	{
		glm::vec3 from;
		glm::vec3 color0;

		glm::vec3 to;
		glm::vec3 color1;
	};

	std::vector<Line> m_lines;

private:
	GPUBuffer* m_verticesBuffer;
	Shader* m_shader;
};

extern ENGINE_API DebugRender* g_pDebugRender;

extern bool g_drawDebug;

#endif // !DEBUGRENDER_H
