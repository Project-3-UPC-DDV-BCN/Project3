#pragma once

#include "Component.h"

class ComponentTransform; 
class ComponentCamera; 
class Particle; 

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
	ComponentBillboard(Particle* attached_particle); 

	bool RotateObject(); 

	void Save(Data& data) const;
	void Load(Data& data);

	~ComponentBillboard(); 

	void AttachToCamera(ComponentCamera* new_reference);
	void SetBillboardType(BillboardingType new_type); 
	BillboardingType GetBillboardType(); 

	void SetAttachedToParticle(bool attached); 
	bool GetAttachedToParticle() const; 

private:

	BillboardingType billboarding_type; 	
	ComponentCamera* reference; 

	bool attached_to_particle; 
	Particle* particle_attached; 

};