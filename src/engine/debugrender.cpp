#include "debugrender.h"
#include "render_shared.h"
#include "render.h"
#include "renderdevice.h"
#include "gpu_buffer.h"
#include "shadersystem.h"

#include "glad/glad.h"

struct DebugVertex
{
	glm::vec3 position;
	glm::vec3 color;
};

static InputLayoutDesc_t g_debugRenderLayout[] =
{
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION },
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_COLOR }
};

bool g_drawDebug = true;

ENGINE_API DebugRender* g_pDebugRender;

DebugRender::DebugRender()
{
	m_verticesBuffer = nullptr;
	m_shader = nullptr;
}

DebugRender::~DebugRender()
{
	m_verticesBuffer = nullptr;
	m_shader = nullptr;
}

void DebugRender::Initialize()
{
	float points[12];
	m_verticesBuffer = g_pRenderDevice->CreateVertexBuffer(points, sizeof(points), true);

	m_shader = g_pShaderSystem->CreateShader("debug_draw",
		"data/shaders/debug_draw.vs",
		"data/shaders/debug_draw.ps",
		g_debugRenderLayout,
		sizeof(g_debugRenderLayout) / sizeof(g_debugRenderLayout[0]));
}

void DebugRender::Shutdown()
{
	if (m_verticesBuffer) {
		delete m_verticesBuffer;
		m_verticesBuffer = nullptr;
	}
}

void DebugRender::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	if (!g_drawDebug)
		return;

	Line line;
	line.from = from;
	line.color0 = color;
	line.to = to;
	line.color1 = color;
	m_lines.push_back(line);
}

void DebugRender::RenderFrame()
{
	if (!g_drawDebug)
		return;

	// draw lines
	DrawLinesInternal();

	// and clear them
	m_lines.clear();
}

void DebugRender::DrawAxis(const glm::vec3& vec)
{
	const float length = 0.2f;
	DrawLine(vec, glm::vec3(vec.x + length, vec.y, vec.z), glm::vec3(1.0f, 0.0, 0.0f));
	DrawLine(vec, glm::vec3(vec.x, vec.y + length, vec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	DrawLine(vec, glm::vec3(vec.x, vec.y, vec.z + length), glm::vec3(0.0f, 1.0f, 0.0f));
}


void DebugRender::DrawLinesInternal()
{
	if (m_lines.empty())
		return;

	g_pRenderDevice->SetDepthTest(true);
	g_pRenderDevice->SetDepthWrite(true);

	g_pRenderDevice->SetVerticesBuffer(m_verticesBuffer);

	m_verticesBuffer->UpdateBuffer(m_lines.data(), m_lines.size() * sizeof(Line));

	// Bind our shader
	g_pShaderSystem->SetShader(m_shader);

	// #TODO: Fix stupid bug, when we get very far from wireframe and lines can start cliping
	glm::mat4 proj = g_pRender->GetProjectionMatrix();
	glm::mat4 view = g_pRender->GetViewMatrix();
	proj[2][3] -= 0.0001f;

	glm::mat4 mv = glm::mat4(1.0f);
	mv = proj * view;

	g_pShaderSystem->SetUniformMatrix(m_shader, UNIFORM_MVP_MATRIX, &mv[0]);

	// draw stuff
	g_pRenderDevice->DrawArrays(PT_LINES, 0, m_lines.size() * 2);

}
