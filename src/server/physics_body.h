#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "physics_world.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

class PhysicsRigidbody
{
public:
	PhysicsRigidbody(JPH::BodyCreationSettings settings, PhysicsWorld* world);
	void CreateBody();
	void DestroyBody();
	void RemoveBody();
	void AddBody(JPH::EActivation state);
	JPH::BodyID* GetBodyId() { if (m_body) { m_body->GetID(); } else { return NULL; } }
private:
	JPH::BodyCreationSettings m_bcsettings;
	JPH::BodyInterface* m_bodyinterface;
	JPH::Body* m_body;
};

#endif // !PHYSICS_BODY_H
