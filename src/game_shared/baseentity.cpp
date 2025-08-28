#include "baseentity.h"

// Register base entity
REGISTER_ENTITY(BaseEntity, "entity");

BaseEntity::BaseEntity() :
	m_Origin(0.0f),
	m_Orient(glm::identity<glm::quat>())
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


// useful point entities

class InfoPlayerStart : public BaseEntity
{
public:
	InfoPlayerStart();
	~InfoPlayerStart();
};

InfoPlayerStart::InfoPlayerStart()
{
}

InfoPlayerStart::~InfoPlayerStart()
{
}

REGISTER_ENTITY(InfoPlayerStart, "info_player_start");

// Entity Registrator

EntityRegistrator* EntityRegistrator::ms_entityRegistrators = nullptr;

EntityRegistrator::EntityRegistrator(EntityRegistrationInfo& regInfo) :
	m_entityRegInfo(regInfo)
{
	m_nextEntityRegistrator = ms_entityRegistrators;
	ms_entityRegistrators = this;
}
