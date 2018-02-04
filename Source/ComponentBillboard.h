#pragma once

#include "Component.h"

class ComponentTransform; 
class ComponentCamera; 

enum BillboardingType
{
	BILLBOARD_X,
	BILLBOARD_Y,
	BILLBOARD_ALL,
	BILLBOARD_NONE,
};

class ComponentBillboard: public Component
{
public:

	ComponentBillboard(GameObject* attached_gameobject);

	bool RotateObject(); 

	void Save(Data& data) const;
	void Load(Data& data);

	~ComponentBillboard(); 

	void AttachToCamera(ComponentCamera* new_reference);
	void SetBillboardType(BillboardingType new_type); 

private:

	BillboardingType billboarding_type; 
	
	ComponentCamera* reference; 

};