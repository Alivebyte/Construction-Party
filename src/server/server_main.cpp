// server_main.cpp - Server game initialization
#include "ientity.h"
#include "baseentity.h"
#include "physics_world.h"
#include "player.h"

IEntity* g_pPlayer = nullptr;

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
	void Update() override;
	void SendUserCmd(const UserCmd* pUserCmd) override;
protected:
	void Simulate();
};

static ServerGame s_ServerGame;

void ServerGame::Init()
{
	// Initialize physics world
	g_PhysicsWorld.Init();

	// Create player
	g_pPlayer = GetServerGameAPI()->CreateEntity("player");
	g_pPlayer->SetOrigin(glm::vec3(-2.0f,1.0f, -2.0f));

	// Create an test entity
	BaseEntity* pTestEntity = new BaseEntity();
	pTestEntity->SetModel("data/models/test.obj");
	pTestEntity->Spawn();
	GetServerGameAPI()->AddEntity(pTestEntity);
}

void ServerGame::Update()
{
	Simulate();
}

void ServerGame::Simulate()
{
	// TODO: Simulate physics
}

void ServerGame::Shutdown()
{
	// Shutdown physics world
	g_PhysicsWorld.Shutdown();
}

void ServerGame::SendUserCmd(const UserCmd* pUserCmd)
{
	Player* pPlayer = dynamic_cast<Player*>(g_pPlayer);
	if (pPlayer)
	{
		pPlayer->ParseUserCmd(pUserCmd);
	}
}

// Server Export
extern "C" __declspec(dllexport) IServerGame* Server_Main()
{
	RegisterServerEntities();
	return &s_ServerGame;
}
