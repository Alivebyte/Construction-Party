#ifndef WORLD_H
#define WORLD_H

#include "ientity.h"

#include <vector>

class World
{
public:
	IEntity* CreateEntity(const char* classname);
	void AddEntity(IEntity* pEntity);

	IEntity* FindEntityByClassname(const char* classname);

	void Update();
	void Render();

private:
	std::vector<IEntity*> m_Entities;
};

extern World g_World;

#endif // !WORLD_H
