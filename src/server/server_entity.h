// server_entity.h - Server-side entity
#ifndef SERVER_ENTITY_H
#define SERVER_ENTITY_H

#include "baseentity.h"
#include "physics_body.h"

class ServerEntity : public BaseEntity
{
public:
	ServerEntity();
	virtual ~ServerEntity();
};

class PhysicsEntity : public ServerEntity
{
public:
	PhysicsEntity();
	~PhysicsEntity();

	void Spawn() override;
	void Think() override;

	void CreateBox(float width);
	void CreateSphere(float radius);
	void CreateCylinder(float height, float radius);
	void CreateFromModel();

private:
	PhysicsRigidbody* m_pBody;
};

#endif // !SERVER_ENTITY_H
