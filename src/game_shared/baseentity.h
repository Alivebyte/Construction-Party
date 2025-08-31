// baseentity.h - Base Entity class for both game sides.
#ifndef BASEENTITY_H
#define BASEENTITY_H

#include "ientity.h"

class BaseEntity : public IEntity
{
public:
	BaseEntity();
	virtual ~BaseEntity();

	virtual void Spawn();
	virtual void Think();

	void SetModel(const char* filename);
};

// Entity registration

class EntityRegistrator
{
public:
	EntityRegistrator(EntityRegistrationInfo& regInfo);

public:
	EntityRegistrationInfo m_entityRegInfo;

	EntityRegistrator* m_nextEntityRegistrator;
	static EntityRegistrator* ms_entityRegistrators;
};

#define REGISTER_ENTITY(CLASSNAME, NAME) \
	static IEntity* Create_##CLASSNAME##_Private() { IEntity* pEntity = new CLASSNAME(); pEntity->SetClassname(NAME); return pEntity; } \
	static EntityRegistrationInfo s_##CLASSNAME##RegisterInfo = { &Create_##CLASSNAME##_Private, NAME }; \
	static EntityRegistrator s_##CLASSNAME##Registrator(s_##CLASSNAME##RegisterInfo)

#endif // !BASEENTITY_H
