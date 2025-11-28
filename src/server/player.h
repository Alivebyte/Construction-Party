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

private:
	glm::vec3 m_Direction;
	UserCmd m_userCmd;
};

#endif // !PLAYER_H
