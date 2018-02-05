#pragma once

#include "Data.h"
#include <string>

class GameObject;

class Component
{
public:
	enum ComponentType {
		CompTransform, CompCamera, CompRigidBody, CompMeshRenderer, CompBoxCollider, CompSphereCollider, CompCapsuleCollider, CompMeshCollider, CompAudioSource, 
		CompAnimaton, CompScript, CompParticleSystem, CompFactory, CompFixedJoint, CompDistanceJoint, CompSphericalJoint, CompRevoluteJoint, CompPrismaticJoint, 
		CompD6Joint, CompBlastMeshRenderer, CompUnknown
	};

	Component();
	virtual ~Component();
	virtual void OnEnable();
	virtual void OnDisable();
	void SetActive(bool active);
	bool IsActive() const;
	void SetGameObject(GameObject* attached_gameobject);
	GameObject* GetGameObject() const;
	void SetType(ComponentType type);
	ComponentType GetType() const;
	void SetName(std::string name);
	std::string GetName() const;
	UID GetUID() const;
	void SetUID(UID uid);

	virtual bool Start();
	virtual bool Update();
	virtual bool CleanUp();

	virtual void Save(Data& data) const = 0;
	virtual void Load(Data& data) = 0;

private:
	ComponentType type;
	GameObject* gameobject;
	std::string name;
	bool active;
	UID uuid;
};

