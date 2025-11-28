#ifndef WEAPON_H
#define WEAPON_H

#include "server_entity.h"

class Weapon : public ServerEntity
{
	typedef ServerEntity inherited;
public:
	Weapon();
	virtual ~Weapon();

	virtual void Think();

	virtual void MakeShot();

protected:
	int m_MaxAmmoAmount;
	int m_AmmoAmount;
	int m_PerShotDamage;
};


class WeaponRail : public Weapon
{
	typedef Weapon inherited;
public:
	WeaponRail();
	~WeaponRail();

	void Think();

	void MakeShot();

};
#endif // !WEAPON_H
