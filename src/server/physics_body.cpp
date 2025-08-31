#include "physics_body.h"

PhysicsRigidbody::PhysicsRigidbody(JPH::BodyCreationSettings settings, PhysicsWorld* world)
{
	m_bcsettings = settings;
	m_bodyinterface = world->GetBodyInterface();
}

void PhysicsRigidbody::CreateBody()
{
	m_body = m_bodyinterface->CreateBody(m_bcsettings);
}

void PhysicsRigidbody::AddBody(JPH::EActivation state)
{
	m_bodyinterface->AddBody(m_body->GetID(), state);
}

void PhysicsRigidbody::RemoveBody()
{
	m_bodyinterface->RemoveBody(m_body->GetID());
}

void PhysicsRigidbody::DestroyBody()
{
	m_bodyinterface->DestroyBody(m_body->GetID());
}