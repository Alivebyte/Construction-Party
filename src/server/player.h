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

	const glm::vec3& GetDirection() { return m_Direction; }

private:
	glm::vec3 m_Direction;
	UserCmd m_userCmd;
};

#endif // !PLAYER_H
