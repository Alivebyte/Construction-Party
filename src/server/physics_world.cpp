#include "ilogger.h"
#include "physics_world.h"
#include "debugrender.h"

#include <Jolt/Renderer/DebugRenderer.h>

// Callback for traces
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	GetLogger()->Print("JPH: %s", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	// Format the message
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%s:%d: ( %s ) (%s)", inFile, inLine, inExpression, inMessage != nullptr ? inMessage : "");

	// Print to the TTY
	GetLogger()->Print("%s", buffer);

	// Breakpoint
	return true;
};
#endif // JPH_ENABLE_ASSERTS

// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual JPH::uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// An example contact listener
class MyContactListener : public JPH::ContactListener
{
public:
	// See: ContactListener
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override
	{
		GetLogger()->Print("Contact validate callback");

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
	{
		GetLogger()->Print("A contact was added");
	}

	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
	{
		GetLogger()->Print("A contact was persisted");
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
	{
		GetLogger()->Print("A contact was removed");
	}
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		GetLogger()->Print("A body got activated");
	}

	virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		GetLogger()->Print("A body went to sleep");
	}
};

class MyDebugRenderer : public JPH::DebugRenderer
{
	// Inherited via DebugRenderer
	void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override
	{
		g_pDebugRender->DrawLine(ToGLM(inFrom), ToGLM(inTo), ToGLM(inColor));
	}

	void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override
	{
		glm::vec3 color = ToGLM(inColor);
		g_pDebugRender->DrawLine(ToGLM(inV1), ToGLM(inV2), color);
		g_pDebugRender->DrawLine(ToGLM(inV2), ToGLM(inV3), color);
		g_pDebugRender->DrawLine(ToGLM(inV3), ToGLM(inV1), color);
	}

	// Inherited via DebugRenderer
	Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override
	{
		return Batch();
	}

	Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override
	{
		return Batch();
	}

	void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override
	{
	}

	void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override
	{
	}

};

// JPH Globals
static JPH::TempAllocatorImpl* g_pTempAllocator = nullptr;
static JPH::JobSystemThreadPool* g_pJobSystemThreadPool = nullptr;

// JPH Physics System
JPH::PhysicsSystem g_JPHPhysicsSystem;

// Our JPH object globals
static ObjectLayerPairFilterImpl g_ObjectLayerPairFilterImpl;
static BPLayerInterfaceImpl g_BPLayerInterfaceImpl;
static ObjectVsBroadPhaseLayerFilterImpl g_ObjectVsBroadPhaseLayerFilterImpl;
static MyContactListener g_MyContactListener;
static MyBodyActivationListener g_MyBodyActivationListener;
static MyDebugRenderer g_MyDebugRenderer;

// The Physics World Global
PhysicsWorld g_PhysicsWorld;

PhysicsWorld::PhysicsWorld()
{
}

PhysicsWorld::~PhysicsWorld()
{
}

void PhysicsWorld::Init()
{
	JPH::RegisterDefaultAllocator();

	// Install trace and assert callbacks
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	// Create a factory
	JPH::Factory::sInstance = new JPH::Factory();

	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	JPH::RegisterTypes();

	// Set a our debug renderer to Jolt
	JPH::DebugRenderer::sInstance = &g_MyDebugRenderer;
	
	// We need a temp allocator for temporary allocations during the physics update. We're
	// pre-allocating 10 MB to avoid having to do allocations during the physics update.
	g_pTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

	// We need a job system that will execute physics jobs on multiple threads.
	g_pJobSystemThreadPool = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	// Setup physics limits
	phys_limits_t phys_limits;
	phys_limits.maxBodies = 1024;
	phys_limits.numBodyMutexes = 0;
	phys_limits.maxContactConstraints = 1024;
	phys_limits.maxBodyPairs = 1024;

	// Now we can create the physics system.
	g_JPHPhysicsSystem.Init(phys_limits.maxBodies, phys_limits.numBodyMutexes, phys_limits.maxBodyPairs, phys_limits.maxContactConstraints, 
		g_BPLayerInterfaceImpl, g_ObjectVsBroadPhaseLayerFilterImpl, 
		g_ObjectLayerPairFilterImpl);

	// Registering a body activation listener
	g_JPHPhysicsSystem.SetBodyActivationListener(&g_MyBodyActivationListener);

	// Registering a contact listener
	g_JPHPhysicsSystem.SetContactListener(&g_MyContactListener);

	g_JPHPhysicsSystem.SetGravity(JPH::Vec3(0.0f, -2.0f, 0.0f));
}

void PhysicsWorld::Update(float deltaTime, int collisionSteps)
{
	g_JPHPhysicsSystem.Update(deltaTime, collisionSteps, g_pTempAllocator, g_pJobSystemThreadPool);

	DebugDraw();
}

void PhysicsWorld::Shutdown()
{
	// Free a job system.
	if (g_pJobSystemThreadPool)
	{
		delete g_pJobSystemThreadPool;
		g_pJobSystemThreadPool = nullptr;
	}

	// Free a temp allocator.
	if (g_pTempAllocator)
	{
		delete g_pTempAllocator;
		g_pTempAllocator = nullptr;
	}

	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void PhysicsWorld::DebugDraw()
{
	JPH::BodyManager::DrawSettings settings;
	//g_JPHPhysicsSystem.DrawBodies(settings, JPH::DebugRenderer::sInstance);
	//g_JPHPhysicsSystem.DrawConstraints(JPH::DebugRenderer::sInstance);
}
