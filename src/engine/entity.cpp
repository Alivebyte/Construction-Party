#include "ientity.h"
#include <vector>
#include <SDL.h>

class EntityAPI : public IEntityAPI
{
public:
	// Inherited via IEntityAPI
	void RegisterEntity(const EntityRegistrationInfo& regInfo) override;
	IEntity* CreateEntity(const char* classname) override;

private:
	typedef std::vector<EntityRegistrationInfo>::iterator EntityRegInfoIt;
	std::vector<EntityRegistrationInfo> m_registeredInfo;
};

void EntityAPI::RegisterEntity(const EntityRegistrationInfo& regInfo)
{
	// Find already registered info
	for (EntityRegInfoIt it = m_registeredInfo.begin(); it != m_registeredInfo.end(); ++it)
	{
		SDL_assert(strcmp(regInfo.m_classname, it->m_classname) != 0);
	}

	m_registeredInfo.push_back(regInfo);
}

IEntity* EntityAPI::CreateEntity(const char* classname)
{
	// Find already registered info
	for (EntityRegInfoIt it = m_registeredInfo.begin(); it != m_registeredInfo.end(); ++it)
	{
		if (strcmp(it->m_classname, classname) == 0)
			return it->m_createEntityFn();
	}

	return nullptr;
}

IEntityAPI* GetEntityAPI()
{
	static EntityAPI entityAPI;
	return &entityAPI;
}

// TEMPORARY HERE: Client Game API

#include "render.h"
#include "engine.h"
#include "modelsystem.h"

class ClientGameAPI : public IClientGameAPI
{
public:
	// Inherited via IClientGameAPI
	void SetViewOrigin(float x, float y, float z, float dirx, float diry, float dirz) override;
	Model* LoadModel(const char* pFilename) override;
};

Model* ClientGameAPI::LoadModel(const char* pFilename)
{
	return g_pModelSystem->LoadModel(pFilename);
}

void ClientGameAPI::SetViewOrigin(float x, float y, float z, float dirx, float diry, float dirz)
{
	g_viewOrigin = glm::vec3(x, y, z);
	g_viewOrient = glm::vec3(dirx, diry, dirz);
}

IClientGameAPI* GetClientGameAPI()
{
	static ClientGameAPI clientGameAPI;
	return &clientGameAPI;
}

///////////////////////////////////////////////////////////

#include "world.h"

class ServerGameAPI : public IServerGameAPI
{
public:
	// Inherited via IServerGameAPI
	IEntity* CreateEntity(const char* pClassname) override;
	void AddEntity(IEntity* pEntity) override;
	IEntity* FindEntityByClassname(const char* classname) override;
};

IEntity* ServerGameAPI::CreateEntity(const char* pClassname)
{
	return g_World.CreateEntity(pClassname);
}

void ServerGameAPI::AddEntity(IEntity* pEntity)
{
	g_World.AddEntity(pEntity);
}

IEntity* ServerGameAPI::FindEntityByClassname(const char* classname)
{
	return g_World.FindEntityByClassname(classname);
}

IServerGameAPI* GetServerGameAPI()
{
	static ServerGameAPI serverGameAPI;
	return &serverGameAPI;
}
