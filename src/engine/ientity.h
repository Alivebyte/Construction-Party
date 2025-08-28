#ifndef ENTITY_H
#define ENTITY_H

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
};

class IEntityAPI
{
public:
	virtual void RegisterEntity(const EntityRegistrationInfo& regInfo) = 0;
	virtual IEntity* CreateEntity(const char* classname) = 0;
};

class IModelAPI
{
public:
	virtual Model* LoadModel(const char* pFilename) = 0;
};

extern "C" __declspec(dllexport) IModelAPI* GetModelAPI();

class IServerGame
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	
};

typedef IServerGame* (*pfnServerMain)();

extern "C" __declspec(dllexport) IEntityAPI* GetEntityAPI();

#endif // !ENTITY_H