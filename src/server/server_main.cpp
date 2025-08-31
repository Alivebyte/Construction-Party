// server_main.cpp - Server game initialization
#include "iengine.h"
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
	BaseEntity* pSceneEntity1 = new BaseEntity();
	pSceneEntity1->SetModel("data/models/scene_table.obj");
	pSceneEntity1->Spawn();
	GetServerGameAPI()->AddEntity(pSceneEntity1);

	BaseEntity* pSceneEntity2 = new BaseEntity();
	pSceneEntity2->SetModel("data/models/scene_walls.obj");
	pSceneEntity2->Spawn();
	GetServerGameAPI()->AddEntity(pSceneEntity2);
}

void ServerGame::Update()
{
	Simulate();
}

void ServerGame::Simulate()
{
	// TODO: Simulate physics
	float deltaTime = GetEngine()->GetDeltaTime();

	g_PhysicsWorld.Update(deltaTime);
	
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
