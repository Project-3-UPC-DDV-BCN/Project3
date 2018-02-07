#include "ComponentBillboard.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "Application.h"
#include "Particles.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"

ComponentBillboard::ComponentBillboard(GameObject* attached_gameobject)
{
	SetGameObject(attached_gameobject);
	SetActive(true);
	SetType(Component::CompBillboard);
	SetAttachedToParticle(false); 
	particle_attached = nullptr; 

	AttachToCamera(App->renderer3D->editor_camera);
}

ComponentBillboard::ComponentBillboard(Particle * attached_particle)
{
	SetGameObject(nullptr);
	SetActive(true);
	SetType(Component::CompBillboard);
	SetBillboardType(BILLBOARD_ALL); 
	SetAttachedToParticle(true);
	particle_attached = attached_particle; 

	AttachToCamera(App->renderer3D->editor_camera);
}

void ComponentBillboard::AttachToCamera(ComponentCamera* new_reference)
{
	reference = new_reference; 
}

void ComponentBillboard::SetBillboardType(BillboardingType new_type)
{
	billboarding_type = new_type; 
}

void ComponentBillboard::SetAttachedToParticle(bool attached)
{
	attached_to_particle = attached; 
}

bool ComponentBillboard::GetAttachedToParticle() const
{
	return attached_to_particle;
}

bool ComponentBillboard::RotateObject()
{
	bool ret = true; 

	if (reference == nullptr || billboarding_type == BILLBOARD_NONE)
		return false; 
	
	//Get the director vector which the object/particle is currently pointing at (Z axis)
	ComponentTransform* object_transform;
	if(!attached_to_particle)
		object_transform = (ComponentTransform*)GetGameObject()->GetComponent(ComponentType::CompTransform);
	else
		object_transform = (ComponentTransform*)particle_attached->components.particle_transform;

	float3 object_z = object_transform->GetMatrix().WorldZ();

	//Get the director vector which the object/particle should be pointing at 
	float3 direction = object_transform->GetGlobalPosition() - reference->camera_frustum.pos; 
	direction.Normalize(); 

	////Crete Y axis
	//float3 new_y = direction.Perpendicular(); 

	////Create X axis
	//float3 new_x = direction.Cross(new_y); 

	////Construct new mat
	//float4x4 new_mat = float4x4::FromT

	////Apply the mat
	//object_transform->SetMatrix(new_mat);

	//Get the difference angle which we should increment depending on the billboard type
	float angle_x; 
	float angle_y; 

	float3 prev_projection;
	float3 next_projection;

	float3 plane_ref = { 0,0,1 }; 

	angle_x = angle_y = 0.0f; 

	switch (billboarding_type)
	{
	case BILLBOARD_X:

		prev_projection = { 0, object_z.y, object_z.z };
		next_projection = { 0, direction.y, direction.z };

		angle_x = plane_ref.AngleBetween(next_projection);

		if (direction.y >= 0)
			angle_x *= -1;

		break; 

	case BILLBOARD_Y:
		prev_projection = { object_z.x, 0,  object_z.z };
		next_projection = { direction.x, 0, direction.z };

		angle_y = plane_ref.AngleBetween(next_projection);

		if (direction.x <= 0)
			angle_y *= -1; 

		break;

	case BILLBOARD_ALL:

		prev_projection = { 0, object_z.y, object_z.z };
		next_projection = { 0, direction.y, direction.z };

		angle_x = plane_ref.AngleBetween(next_projection);

		if (direction.y >= 0)
			angle_x *= -1;

		prev_projection = { object_z.x, 0,  object_z.z };
		next_projection = { direction.x, 0, direction.z };

		angle_y = plane_ref.AngleBetween(next_projection);

		if (direction.x <= 0)
			angle_y *= -1;

		break;
	}

	object_transform->SetRotation({ angle_x*RADTODEG, angle_y*RADTODEG, 0 });

	return ret;
}

void ComponentBillboard::Save(Data & data) const
{
}

void ComponentBillboard::Load(Data & data)
{
}

ComponentBillboard::~ComponentBillboard()
{
}


