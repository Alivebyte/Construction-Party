#include "baseentity.h"

// Register base entity
REGISTER_ENTITY(BaseEntity, "entity");

BaseEntity::BaseEntity()
{
	m_pClassname = nullptr;
	m_Origin = glm::vec3(0.0f);
	m_Orient = glm::quat(glm::identity<glm::quat>());
	m_pModel = nullptr;
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

void BaseEntity::SetModel(const char* filename)
{
	m_pModel = GetClientGameAPI()->LoadModel(filename);
}

// useful point entities

class InfoPlayerStart : public BaseEntity
{
public:
	InfoPlayerStart();
	~InfoPlayerStart();
};

REGISTER_ENTITY(InfoPlayerStart, "info_player_start");

InfoPlayerStart::InfoPlayerStart()
{
}

InfoPlayerStart::~InfoPlayerStart()
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
