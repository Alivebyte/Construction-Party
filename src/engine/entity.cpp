#include "entity.h"
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
		SDL_assert(strcmp(regInfo.m_classname, it->m_classname) == 0);
	}
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

