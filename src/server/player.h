#ifndef PLAYER_H
#define PLAYER_H

#include "server_entity.h"

class Player : public ServerEntity
{
public:
	Player();
	~Player();

	void Think() override;

	void ParseUserCmd(const UserCmd* pUserCmd);
};

#endif // !PLAYER_H
