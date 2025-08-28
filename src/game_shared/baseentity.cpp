#include "baseentity.h"

BaseEntity::BaseEntity()
{
}

BaseEntity::~BaseEntity()
{
}

void BaseEntity::Spawn()
{
}

void BaseEntity::Think()
{
}

// Entity Registrator

EntityRegistrator* EntityRegistrator::ms_entityRegistrators = nullptr;

EntityRegistrator::EntityRegistrator(EntityRegistrationInfo& regInfo) :
	m_entityRegInfo(regInfo)
{
	m_nextEntityRegistrator = ms_entityRegistrators;
	ms_entityRegistrators = this;
}
