#include <SDL.h>
#include "world.h"
#include "modelsystem.h"

World g_World;

IEntity* World::CreateEntity(const char* classname)
{
	IEntity* pEntity = GetEntityAPI()->CreateEntity(classname);
	SDL_assert(pEntity);
	m_Entities.push_back(pEntity);
	return pEntity;
}

void World::AddEntity(IEntity* pEntity)
{
	SDL_assert(pEntity);
	m_Entities.push_back(pEntity);
}

IEntity* World::FindEntityByClassname(const char* classname)
{
	SDL_assert(classname);

	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i] && strcmp(m_Entities[i]->GetClassname(), classname) == 0)
			return m_Entities[i];
	}

	return nullptr;
}

void World::Update()
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i])
			m_Entities[i]->Think();
	}
}

void World::Render()
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i] && m_Entities[i]->GetModel())
		{
			m_Entities[i]->GetModel()->Draw(m_Entities[i]->GetEntityMatrix());
		}
	}
}