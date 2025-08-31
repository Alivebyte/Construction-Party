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
	g_pPlayer->SetOrigin(glm::vec3(1.45480740f, 1.86708641f, -1.65813649f));

	// Create an test entity
	PhysicsEntity* pSceneEntity1 = new PhysicsEntity();
	pSceneEntity1->SetModel("data/models/scene_table.obj");
	pSceneEntity1->CreateFromModel();
	pSceneEntity1->Spawn();
	GetServerGameAPI()->AddEntity(pSceneEntity1);

	PhysicsEntity* pSceneEntity2 = new PhysicsEntity();
	pSceneEntity2->SetModel("data/models/scene_walls.obj");
	pSceneEntity2->CreateFromModel();
	pSceneEntity2->Spawn();
	GetServerGameAPI()->AddEntity(pSceneEntity2);

	const char* figures[] =
	{
		"data/models/figure_box.obj",
		//"data/models/figure_cone.obj", NO SUPPORTED :(
		"data/models/figure_sphere.obj",
		"data/models/figure_cylinder.obj",
	};

	float step = 0.0f;

	for (int i = 0; i < sizeof(figures) / sizeof(figures[0]); i++)
	{
		PhysicsEntity* pFigure = new PhysicsEntity();
		pFigure->SetOrigin(glm::vec3(step, 1.5f, 0.0f));
		pFigure->SetModel(figures[i]);
	
		if (strstr(figures[i], "box"))
			pFigure->CreateBox(0.2f);
		else if (strstr(figures[i], "sphere"))
			pFigure->CreateSphere(0.3f);
		else if (strstr(figures[i], "cylinder"))
			pFigure->CreateCylinder(0.2f, 0.1f);

		pFigure->Spawn();
		GetServerGameAPI()->AddEntity(pFigure);
		step += 0.4f;
	}

	//PhysicsEntity* pPhysicsEntity = new PhysicsEntity();
	//pPhysicsEntity->SetOrigin(glm::vec3(0.0f, 2.0f, 0.0f));
	//pPhysicsEntity->CreateBox(0.2f);
	//pPhysicsEntity->Spawn();
	//pPhysicsEntity->SetModel("data/models/figure_box.obj");
	//GetServerGameAPI()->AddEntity(pPhysicsEntity);
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
