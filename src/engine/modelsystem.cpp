#include <assert.h>
#include <vector>
#include <string>
#include "ifilesystem.h"
#include "ilogger.h"
#include "render.h"
#include "gpu_buffer.h"
#include "shader.h"
#include "shadersystem.h"
#include "renderdevice.h"
#include "modelsystem.h"
#include "texturesmanager.h"

ModelSystem* g_pModelSystem = nullptr;

static InputLayoutDesc_t g_staticVertexLayout[] = {
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION },
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_NORMAL },
	{ VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD },
};

static InputLayoutDesc_t g_skinnedVertexLayout[] = {
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_POSITION },
	{ VERTEXATTR_VEC3, SHADERSEMANTIC_NORMAL },
	{ VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD }
};

Shader* g_litShader = nullptr;

std::string getFileNameWithoutExtension(const std::string& filename)
{
	size_t lastindex = filename.find_last_of(".");
	if (lastindex != std::string::npos) {
		return filename.substr(0, lastindex);
	}

	return filename;
}

ModelSystem::ModelSystem()
{
}

ModelSystem::~ModelSystem()
{
}

void ModelSystem::Shutdown()
{
	for (int i = 0; i < m_models.size(); i++)
	{
		if (m_models[i].model)
		{
			delete m_models[i].model;
			m_models[i].model = nullptr;
		}
	}

	m_models.clear();
}

Model* ModelSystem::LoadModel(const char* filename)
{
	auto it = std::find_if(m_models.begin(), m_models.end(), [=](const ModelEntry& entry) { return strcmp(entry.filename, filename) == 0; });
	if (it != m_models.end())
	{
		return it->model;
	}

	if (!GetFileSystem()->IsExist(filename))
	{
		GetLogger()->Print("ModelSystem::LoadModel: File '%s' not exist.", filename);
		return nullptr;
	}

	Model* model = new Model();

	if (strstr(filename, ".obj"))
	{
		model->LoadObj(filename);
	}
	else
	{
		// Find file extension
		size_t stringLength = strlen(filename);

		for (int i = stringLength; i > 0; --i)
		{
			if (filename[i] == '.')
			{
				stringLength = i;
				break;
			}
		}

		const char* fileExtension = &filename[stringLength];

		GetLogger()->Print("ModelSystem::LoadModel: Unknowed file format '%s'",
			filename, fileExtension);

		return nullptr;
	}

	ModelEntry entry = {};
	strcpy(entry.filename, filename);
	entry.model = model;

	m_models.push_back(entry);

	GetLogger()->Print("Loaded Model '%s'", filename);

	return model;
}

Model::Model()
{
	m_data.vb = nullptr;
	m_data.ib = nullptr;
	m_AlbedoTexture = nullptr;

	//m_boundingBox.min = glm::vec3(0.0f);
	//m_boundingBox.max = glm::vec3(0.0f);
}

Model::~Model()
{
	m_AlbedoTexture = nullptr;

	if (m_data.vb)
	{
		delete m_data.vb;
		m_data.vb = nullptr;
	}
}

void Model::LoadObj(const char* filename)
{
	GetLogger()->Print("Loading OBJ file %s...", filename);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::vector<glm::vec3> out_vertices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		GetLogger()->Print("Model::LoadObj: Impossible to open the file !");
		return;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				GetLogger()->Print("Model::LoadObj: File can't be read by our simple parser :-( Try exporting with other options");
				fclose(file);
				return;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	fclose(file);

	// Combine in to the one array
	std::vector<StaticMeshVertex>  vertices;
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		StaticMeshVertex vtx = {};
		vtx.position = vertex;
		vtx.normal = normal;
		vtx.texcoord = uv;
		vertices.push_back(vtx);

		//m_boundingBox.min = glm::min(m_boundingBox.min, vertex);
		//m_boundingBox.max = glm::max(m_boundingBox.max, vertex);
	}

	m_data.vb = g_pRenderDevice->CreateVertexBuffer(vertices.data(), (int)sizeof(StaticMeshVertex) * (int)vertices.size());
	m_data.vbcount = vertices.size();

	std::string mtlfilename = getFileNameWithoutExtension(filename);
	mtlfilename += ".mtl";
	LoadMtl(mtlfilename.c_str());
}

void Model::LoadMtl(const char* filename)
{
	GetLogger()->Print("Loading MTL file %s...", filename);

	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		GetLogger()->Print("Model::LoadObj: Impossible to open the file !");
		return;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "map_Kd") == 0) {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);

			const char* textureFilename = stupidBuffer + 1;
			m_AlbedoTexture = g_pTexturesManager->LoadTexture2D(textureFilename, true);
		}

		//if (strcmp(lineHeader, "v") == 0) {
		//	glm::vec3 vertex;
		//	fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		//	temp_vertices.push_back(vertex);
		//}
		//else {
		//	// Probably a comment, eat up the rest of the line
		//	char stupidBuffer[1000];
		//	fgets(stupidBuffer, 1000, file);
		//}

	}

	fclose(file);
}

void Model::Draw(const glm::mat4& model, bool isTransparent /*= false*/)
{
	if (!g_litShader)
	{
		g_litShader = g_pShaderSystem->CreateShader("lit_generic", "data/shaders/lit_generic.vs", "data/shaders/lit_generic.ps",
			g_staticVertexLayout, sizeof(g_staticVertexLayout) / sizeof(g_staticVertexLayout[0]));
	}

	glFrontFace(GL_CCW);

	// ...hack hack hack...
	glDepthFunc(GL_ALWAYS);

	g_pRenderDevice->SetCullFace(true);
	g_pRenderDevice->SetDepthTest(true);
	g_pRenderDevice->SetDepthWrite(true);

	if (isTransparent)
	{
		// Enable blending
		g_pRenderDevice->SetBlending(true);
		g_pRenderDevice->SetBlendingFunction(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);

		glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, .5f);
		g_pShaderSystem->SetUniformFloat4(g_litShader, UNIFORM_CUSTOM_COLOR, glm::value_ptr(color));
	}
	else
	{
		g_pRenderDevice->SetBlending(false);

		//glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f);
		//g_pShaderSystem->SetUniformFloat4(g_litShader, UNIFORM_CUSTOM_COLOR, glm::value_ptr(color));
	}

	g_pRenderDevice->SetVerticesBuffer(m_data.vb);

	g_pShaderSystem->SetShader(g_litShader);
	g_pShaderSystem->SetUniformMatrix(g_litShader, UNIFORM_MODEL_MATRIX, &model[0]);

	//static float test = 0.0f;
	//test += g_systemTimer->GetDelta() * 6.0f;

	//glm::mat4 model = glm::mat4(1.0f);

	//int32_t x = 0, y = 0;
	//g_inputSystem->GetMousePosition(&x, &y);
	//
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, test, glm::vec3(0.0f, 1.0f, 0.0f));
	//
	//float realY = (float)g_renderView.height - (float)y - 1;
	//
	//glm::vec4 viewport = glm::vec4(0.0f, 0.0f, (float)g_renderView.width, (float)g_renderView.height);
	//glm::vec3 pos = glm::unProject(
	//	glm::vec3((float)x, realY, 0.0f),
	//	model,
	//	g_renderView.proj,
	//	viewport);
	//
	//pos *= 50.0f;
	//
	//model = glm::translate(model, pos);

	glm::mat4 mvp = glm::identity<glm::mat4>();
	mvp = g_pRender->GetProjectionMatrix() * g_pRender->GetViewMatrix() * model;
	g_pShaderSystem->SetUniformMatrix(g_litShader, UNIFORM_MVP_MATRIX, &mvp[0]);
	
	g_pTexturesManager->SetTexture(0, m_AlbedoTexture);
	g_pShaderSystem->SetUniformSampler(g_litShader, SAMPLER_ALBEDO, 0);

	g_pRenderDevice->DrawArrays(PT_TRIANGLES, 0, m_data.vbcount);

#if 0
	glm::mat4 mvp = glm::identity<glm::mat4>();
	mvp = g_renderView.proj * g_renderView.view * model;
	g_shaderSystem->SetUniformMatrix(g_litShader, UNIFORM_MVP_MATRIX, &mvp[0]);

	g_pTexturesManager->SetTexture(0, m_AlbedoTexture);
	g_shaderSystem->SetUniformSampler(g_litShader, SAMPLER_ALBEDO, 0);

	g_renderDevice->DrawArrays(PT_TRIANGLES, 0, m_data.vbcount);

	BoundingBox bbox = m_boundingBox;
	TransformBoundingBox(bbox, model);
	g_debugRender->DrawBoundingBox(bbox, glm::vec3(1.0f));
#endif
}

void ReleaseModelData(ModelData_t& data)
{
	if (data.ib)
	{
		delete data.ib;
		data.ib = nullptr;
	}

	if (data.vb)
	{
		delete data.vb;
		data.vb = nullptr;
	}
}
