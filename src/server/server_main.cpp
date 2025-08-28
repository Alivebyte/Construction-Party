// server_main.cpp - Server game initialization
#include "ientity.h"
#include "baseentity.h"
#include "physics_world.h"

void RegisterServerEntities()
{
	for (EntityRegistrator* entityReg = EntityRegistrator::ms_entityRegistrators;
		entityReg != nullptr;
		entityReg = entityReg->m_nextEntityRegistrator)
	{
		GetEntityAPI()->RegisterEntity(entityReg->m_entityRegInfo);
	}
}

class ServerGame : public IServerGame
{
public:
	// Inherited via IServerGame
	void Init() override;
	void Shutdown() override;
};

static ServerGame s_ServerGame;

void ServerGame::Init()
{
	// Initialize physics world
	g_PhysicsWorld.Init();
}

void ServerGame::Shutdown()
{
	// Shutdown physics world
	g_PhysicsWorld.Shutdown();
}

// Server Export
extern "C" __declspec(dllexport) IServerGame* Server_Main()
{
	RegisterServerEntities();
	return &s_ServerGame;
}
