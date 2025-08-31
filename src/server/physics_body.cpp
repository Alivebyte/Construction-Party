#include "physics_body.h"

PhysicsRigidbody::PhysicsRigidbody(const JPH::BodyCreationSettings& settings, PhysicsWorld* world) :
	m_body(nullptr)
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

void PhysicsRigidbody::SetPosition(const glm::vec3& pos)
{
	m_bodyinterface->SetPosition(m_body->GetID(), ToJPH(pos), JPH::EActivation::Activate);
}

glm::vec3 PhysicsRigidbody::GetPosition()
{
	JPH::RVec3 pps = m_bodyinterface->GetPosition(m_body->GetID());
	return ToGLM(pps);
}

void PhysicsRigidbody::DebugDraw()
{
	JPH::RMat44 transform = m_body->GetWorldTransform();
	m_body->GetShape()->Draw(JPH::DebugRenderer::sInstance, transform, JPH::Vec3::sReplicate(1.0f), JPH::Color::sGreen, false, false);
}