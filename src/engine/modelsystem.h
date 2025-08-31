#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <vector>

#include "render_shared.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class GPUBuffer;
class Texture2D;

struct ModelData_t
{
	GPUBuffer* vb;
	GPUBuffer* ib;
	uint32_t vbcount;
	uint32_t ibcount;
};

void ReleaseModelData(ModelData_t& data);

class Render;

class Model
{
	friend class Render;
public:
	Model();
	~Model();

	void LoadObj(const char* filename);
	void LoadMtl(const char* filename);

	void Draw(const glm::mat4& model, bool isTransparent = false);

	//BoundingBox GetBoundingBox() { return m_boundingBox; } 

private:
	std::vector<StaticMeshVertex> m_Vertices;
	ModelData_t m_data;
	//BoundingBox m_boundingBox;
	Texture2D* m_AlbedoTexture;
};

class ModelSystem
{
public:
	ModelSystem();
	~ModelSystem();

	void Shutdown();

	Model* LoadModel(const char* filename);

private:
	struct ModelEntry
	{
		char filename[260];
		Model* model;
	};

	std::vector< ModelEntry > m_models;

};

extern ModelSystem* g_pModelSystem;

#endif // !MODELMANAGER_H
