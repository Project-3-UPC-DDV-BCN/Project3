#pragma once
#include "Component.h"

class ComponentRigidBody :
	public Component
{
public:
	ComponentRigidBody(GameObject* attached_gameobject);
	~ComponentRigidBody();


};

