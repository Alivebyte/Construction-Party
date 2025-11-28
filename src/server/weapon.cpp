#include "weapon.h"

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

void WeaponRail::MakeShot()
{
	inherited::MakeShot();
	//Msg("Pew!");
}