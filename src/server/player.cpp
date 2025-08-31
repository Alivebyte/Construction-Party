#include "player.h"
#include "iengine.h"
#include "physics_world.h"
#include "debugrender.h"
#include <imgui.h>

using namespace JPH;

REGISTER_ENTITY(Player, "player");

Body* gDragAnchor = nullptr;
BodyID gDragBody;
Ref<Constraint> gDragConstraint;

Player::Player() :
	m_RayPickMode(false)
{
}

Player::~Player()
{
}

void Player::Think()
{
	JPH::BodyInterface& bi = g_JPHPhysicsSystem.GetBodyInterface();

	//static Vec3Arg lastHitPos;
	if (gDragConstraint == nullptr)
	{

		

		//JPH::RayCast ray;
		//ray.mOrigin = ToJPH(m_Origin);
		//ray.mDirection = ToJPH(m_Direction);

		//// Create ray
		//JPH::RRayCast ray{ ToJPH(m_Origin), ToJPH(m_Direction) };

		//// Cast ray
		//JPH::RayCastResult hit;
		//had_hit = g_JPHPhysicsSystem.GetNarrowPhaseQuery().CastRay(ray, hit, JPH::SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING), JPH::SpecifiedObjectLayerFilter(Layers::MOVING));

		//g_pDebugRender->DrawLine(m_RayOrigin, m_RayOrigin + 200.0f * m_Direction, glm::vec3(1.0f));

		// Create ray
		RRayCast ray{ ToJPH(m_RayOrigin), ToJPH(m_RayOrigin + 200.0f * m_Direction) };

		// Cast ray
		RayCastResult hit;
		bool had_hit = g_JPHPhysicsSystem.GetNarrowPhaseQuery().CastRay(ray, hit, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING), SpecifiedObjectLayerFilter(Layers::MOVING));
		gDragBody = hit.mBodyID;

		Vec3Arg hitPosition = ray.GetPointOnRay(hit.mFraction);

		//g_pDebugRender->DrawAxis(ToGLM(hitPosition));
		if (had_hit)
		{
			

			if (m_RayPickMode)
			{
				// Target body must be dynamic
				BodyLockWrite lock(g_JPHPhysicsSystem.GetBodyLockInterface(), gDragBody);
				Body& drag_body = lock.GetBody();
				if (lock.Succeeded())
				{
					if (drag_body.IsDynamic())
					{
						// Create constraint to drag body
						DistanceConstraintSettings settings;
						settings.mPoint1 = settings.mPoint2 = hitPosition;
						settings.mLimitsSpringSettings.mFrequency = 2.0f / 1.0f;
						settings.mLimitsSpringSettings.mDamping = 1.0f;

						// Construct fixed body for the mouse constraint
						// Note that we don't add it to the world since we don't want anything to collide with it, we just
						// need an anchor for a constraint
						Body* drag_anchor = bi.CreateBody(BodyCreationSettings(new SphereShape(0.01f), hitPosition, Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING));
						gDragAnchor = drag_anchor;

						// Construct constraint that connects the drag anchor with the body that we want to drag
						gDragConstraint = settings.Create(*drag_anchor, drag_body);
						g_JPHPhysicsSystem.AddConstraint(gDragConstraint);
					}
				}
			}
		}
	}
	else
	{
		if (!m_RayPickMode)
		{
			// If key released, destroy constraint
			if (gDragConstraint != nullptr)
			{
				g_JPHPhysicsSystem.RemoveConstraint(gDragConstraint);
				gDragConstraint = nullptr;
			}

			// Destroy drag anchor
			if (gDragAnchor != nullptr)
			{
				bi.DestroyBody(gDragAnchor->GetID());
				gDragAnchor = nullptr;
			}

			// Forget the drag body
			gDragBody = BodyID();
		}
		else
		{
			if (gDragAnchor)
				bi.SetPosition(gDragAnchor->GetID(), ToJPH(m_RayOrigin), EActivation::DontActivate);

			// Activate other body
			bi.ActivateBody(gDragBody);
		}

	}
}

void Player::ParseUserCmd(const UserCmd* pUserCmd)
{
	if (!pUserCmd)
		return;

	if (pUserCmd->action)
		m_RayPickMode = true;
	else
		m_RayPickMode = false;

	m_RayOrigin = glm::vec3(pUserCmd->posx, pUserCmd->posy, pUserCmd->posz);
	m_Direction = glm::vec3(pUserCmd->dirx, pUserCmd->diry, pUserCmd->dirz);

	return;

	glm::vec3 direction = glm::vec3(pUserCmd->dirx, pUserCmd->diry, pUserCmd->dirz);

	// #TODO: !!!
	float dt = GetEngine()->GetDeltaTime();

	// movement
	if (pUserCmd->walkForward)
		m_Origin += direction * 100.5f * dt;
	if (pUserCmd->walkBackward)
		m_Origin -= direction * 100.5f * dt;
	if (pUserCmd->strafeLeft)
		m_Origin -= glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)) * 100.0f * dt;
	if (pUserCmd->strafeRight)
		m_Origin += glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)) * 100.0f * dt;
}
