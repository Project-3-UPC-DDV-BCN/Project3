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

	AttachToCamera(App->renderer3D->game_camera);
}

void ComponentBillboard::AttachToCamera(ComponentCamera* new_reference)
{
	reference = new_reference; 
}

void ComponentBillboard::SetBillboardType(BillboardingType new_type)
{
	billboarding_type = new_type; 
}

BillboardingType ComponentBillboard::GetBillboardType()
{
	return billboarding_type;
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

	BROFILER_CATEGORY("Component - Billboard - RotateObject", Profiler::Color::Beige);

	if (reference == nullptr || billboarding_type == BILLBOARD_NONE)
		return false;

	//Get the director vector which the object/particle is currently pointing at (Z axis)
	ComponentTransform* object_transform;

	float3 reference_axis = { 0,0,-1 };

	if (!attached_to_particle)
		object_transform = (ComponentTransform*)GetGameObject()->GetComponent(ComponentType::CompTransform);
	else
		object_transform = (ComponentTransform*)particle_attached->components.particle_transform;

	float3 object_z = { 0,0,1 }; 

	//Get the director vector which the object/particle should be pointing at 
	float3 direction = object_transform->GetGlobalPosition() - reference->camera_frustum.Pos();
	direction.Normalize(); 

	Quat rot = Quat::RotateFromTo(object_z, direction);

	float3 angles = rot.ToEulerXYZ()*RADTODEG; 

	object_transform->SetRotation({ 0,0,0 }); 

	if (reference->camera_frustum.Pos().z > object_transform->GetGlobalPosition().z &&
		reference->camera_frustum.Pos().x < object_transform->GetGlobalPosition().x)
		angles.y = 90 + (90 - angles.y); 

	else if(reference->camera_frustum.Pos().z > object_transform->GetGlobalPosition().z &&
		reference->camera_frustum.Pos().x > object_transform->GetGlobalPosition().x)
		angles.y = -90 - (90 + angles.y);

	object_transform->SetRotation({ 0, angles.y, 0 });

	CONSOLE_LOG("%f", angles.y); 

	//if (billboarding_type == BILLBOARD_Y)
	//	direction.y = 0;

	//else if (billboarding_type == BILLBOARD_X)
	//	direction.x = 0;

	//else if (billboarding_type == BILLBOARD_ALL)
	//{
	//	direction = { 0,0,1 };
	//	object_transform->SetRotation({ 0, 0, 0 });
	//	object_z = {0,0,1};
	//}
	//	

	//direction.Normalize();
	//direction *= -1;

	////Get the angle between where the object is pointing at and where the object should be pointing at in XZ plane
	//float3 desired_pos_projection_xz = { direction.x, 0, direction.z };
	//float3 projection_z_on_xz = { object_z.x, 0, object_z.z };

	//float angle_xz = desired_pos_projection_xz.AngleBetweenNorm(reference_axis)*RADTODEG;

	////Get the angle between where the object is pointing at and where the object should be pointing at in XY plane
	//float3 desired_pos_projection_xy = { 0, direction.y, direction.z };
	//float angle_xy = desired_pos_projection_xy.AngleBetweenNorm(reference_axis)*RADTODEG;

	//if (reference->camera_frustum.Pos().x > 0)
	//	angle_xz *= -1;

	//if (reference->camera_frustum.Pos().y < 0)
	//	angle_xy *= -1;

	//switch (billboarding_type)
	//{
	//case BILLBOARD_X:
	//	object_transform->SetRotation({ angle_xy, object_transform->GetGlobalRotation().y, object_transform->GetGlobalRotation().z });
	//	break;

	//case BILLBOARD_Y:
	//	object_transform->SetRotation({ object_transform->GetGlobalRotation().x, angle_xz, object_transform->GetGlobalRotation().z });
	//	break;

	//case BILLBOARD_ALL:
	//	billboarding_type = BILLBOARD_Y; 
	//	RotateObject(); 

	//	billboarding_type = BILLBOARD_X;
	//	RotateObject();

	//	billboarding_type = BILLBOARD_ALL; 

	//	break;
	//}
	//	
	//return ret;
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


