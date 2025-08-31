#ifndef ENTITY_H
#define ENTITY_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class IEntity;
class Model;

struct EntityRegistrationInfo
{
	IEntity* (*m_createEntityFn)();
	const char* m_classname;
};

class IEntity
{
public:
	virtual ~IEntity() {}

	virtual void Think() = 0;

	inline void SetClassname(const char* pClassname) { m_pClassname = pClassname; }
	inline const char* GetClassname() { return m_pClassname; }
	inline void SetOrigin(const glm::vec3& origin) { m_Origin = origin; }
	inline void SetOrient(const glm::quat& orient) { m_Orient = orient; }
	inline const glm::vec3& GetOrigin() { return m_Origin; }
	inline const glm::quat& GetOrient() { return m_Orient; }
	inline Model* GetModel() { return m_pModel; }
	inline glm::mat4 GetEntityMatrix();

protected:
	const char* m_pClassname;
	glm::vec3 m_Origin;
	glm::quat m_Orient;
	Model* m_pModel;
};

inline glm::mat4 IEntity::GetEntityMatrix()
{
	glm::mat4 matWorldTransform = glm::identity<glm::mat4>();
	glm::mat4 matRotation = glm::mat4_cast(m_Orient);
	matWorldTransform = glm::translate(matWorldTransform, m_Origin) * matRotation;
	return matWorldTransform;
}

class IEntityAPI
{
public:
	virtual void RegisterEntity(const EntityRegistrationInfo& regInfo) = 0;
	virtual IEntity* CreateEntity(const char* classname) = 0;
};

extern "C" __declspec(dllexport) IEntityAPI* GetEntityAPI();

class IClientGameAPI
{
public:
	virtual void SetViewOrigin(float x, float y, float z, float dirx, float diry, float dirz) = 0;
	virtual Model* LoadModel(const char* pFilename) = 0;
};

extern "C" __declspec(dllexport) IClientGameAPI* GetClientGameAPI();

class IServerGameAPI
{
public:
	virtual IEntity* CreateEntity(const char* pClassname) = 0;
	virtual void AddEntity(IEntity* pEntity) = 0;
	virtual IEntity* FindEntityByClassname(const char* classname) = 0;
};

extern "C" __declspec(dllexport) IServerGameAPI* GetServerGameAPI();

struct UserCmd
{
	uint8_t walkForward;
	uint8_t walkBackward;
	uint8_t strafeLeft;
	uint8_t strafeRight;
	uint16_t mouseX;
	uint16_t mouseY;
};

class IServerGame
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void SendUserCmd(const UserCmd* pUserCmd) = 0;
};

typedef IServerGame* (*pfnServerMain)();

typedef union SDL_Event SDL_Event;

class IClientGame
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual void OnEvent(const SDL_Event* pEvent) = 0;

	virtual void Render() = 0;
};

typedef IClientGame* (*pfnClientMain)();

#endif // !ENTITY_H