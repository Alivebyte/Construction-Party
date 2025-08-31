#include "server_entity.h"
#include "physics_world.h"
#include "irender.h"

ServerEntity::ServerEntity()
{
}

ServerEntity::~ServerEntity()
{
}

/////////////////////////////////////////////////

REGISTER_ENTITY(PhysicsEntity, "physics_prop");

using namespace JPH;

PhysicsEntity::PhysicsEntity() :
	m_pBody(nullptr)
{
}

PhysicsEntity::~PhysicsEntity()
{
	if (m_pBody)
	{
		delete m_pBody;
		m_pBody = nullptr;
	}
}

void PhysicsEntity::Spawn()
{
	if (m_pBody)
		m_pBody->AddBody(EActivation::Activate);
}

void PhysicsEntity::Think()
{
	if (m_pBody)
	{
		//m_pBody->DebugDraw();

		m_Origin = m_pBody->GetPosition();

	}
}

void PhysicsEntity::CreateBox(float width)
{
	BoxShapeSettings boxSettings(Vec3(width, width, width));
	RefConst<Shape> pBoxShape = boxSettings.Create().Get();

	BodyCreationSettings settings(pBoxShape, ToJPH(m_Origin), Quat::sIdentity(),
		EMotionType::Dynamic, Layers::MOVING);

	m_pBody = new PhysicsRigidbody(settings, &g_PhysicsWorld);
	m_pBody->CreateBody();
}

void PhysicsEntity::CreateSphere(float radius)
{
	SphereShapeSettings shpereSettings(radius);
	RefConst<Shape> pShape = shpereSettings.Create().Get();

	BodyCreationSettings settings(pShape, ToJPH(m_Origin), Quat::sIdentity(),
		EMotionType::Dynamic, Layers::MOVING);

	m_pBody = new PhysicsRigidbody(settings, &g_PhysicsWorld);
	m_pBody->CreateBody();
}

void PhysicsEntity::CreateCylinder(float height, float radius)
{
	CylinderShapeSettings shpereSettings(height, radius);
	RefConst<Shape> pShape = shpereSettings.Create().Get();

	BodyCreationSettings settings(pShape, ToJPH(m_Origin), Quat::sIdentity(),
		EMotionType::Dynamic, Layers::MOVING);

	m_pBody = new PhysicsRigidbody(settings, &g_PhysicsWorld);
	m_pBody->CreateBody();
}

void PhysicsEntity::CreateFromModel()
{
	// no luck today
	if (!m_pModel)
		return;

	int numVertices = 0;
	const StaticMeshVertex* pVertices = GetRender()->GetModelVertices(m_pModel, &numVertices);

	Array<JPH::Vec3> vertices;
	vertices.reserve(numVertices);

	for (int i = 0; i < numVertices; i++)
		vertices.push_back(ToJPH(pVertices[i].position));

	JPH::TriangleList triangles;
	triangles.reserve(numVertices / 3);

	for (int i = 0; i < numVertices; i += 3)
	{
		JPH::Triangle triangle;
		triangle.mV[0] = JPH::Float3{ pVertices[i].position.x,  pVertices[i].position.y,  pVertices[i].position.z };
		triangle.mV[1] = JPH::Float3{ pVertices[i + 1].position.x, pVertices[i + 1].position.y, pVertices[i + 1].position.z };
		triangle.mV[2] = JPH::Float3{ pVertices[i + 2].position.x, pVertices[i + 2].position.y, pVertices[i + 2].position.z };
		triangles.push_back(triangle);
	}

	JPH::MeshShapeSettings meshSettings(triangles);
	meshSettings.Sanitize();

	RefConst<Shape> pShape = meshSettings.Create().Get();

	BodyCreationSettings settings(pShape, ToJPH(m_Origin), Quat::sIdentity(),
		EMotionType::Static, Layers::NON_MOVING);

	m_pBody = new PhysicsRigidbody(settings, &g_PhysicsWorld);
	m_pBody->CreateBody();
}
