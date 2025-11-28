#include "weapon.h"
#include "player.h"
#include "ilogger.h"

Weapon::Weapon()
{
	m_MaxAmmoAmount = 0;
	m_AmmoAmount = 0;
	m_PerShotDamage = 0;
}

Weapon::~Weapon()
{

}

void Weapon::Think()
{
	inherited::Think();
}

void Weapon::MakeShot()
{
	glm::vec3 shotDir;
}

// Rail

REGISTER_ENTITY(WeaponRail, "weapon_rail");

WeaponRail::WeaponRail()
{
}

WeaponRail::~WeaponRail()
{

}

void WeaponRail::Think()
{
	inherited::Think();
}

extern IEntity* g_pPlayer;

void WeaponRail::MakeShot()
{
	Player* pPlayer = static_cast<Player*>(g_pPlayer);

	RayHitResult result;
	if (g_PhysicsWorld.TraceRay(pPlayer->GetOrigin(), 
		pPlayer->GetOrigin() + pPlayer->GetDirection() * 100.0f,
		result, 
		Layers::MOVING, 
		Layers::MOVING))
	{
		GetLogger()->Print("Pew!");
	}
}