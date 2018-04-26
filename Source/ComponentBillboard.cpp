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

	//Get the rotation of the current camera & construct the matrix
	float3x3 camera_rot = reference->GetFrustum().ViewMatrix().RotatePart(); 
	float4x4 new_object_matrix = float4x4::FromTRS(object_transform->GetGlobalPosition(), camera_rot, object_transform->GetGlobalScale());
	
	//Apply it into the plane
	object_transform->SetMatrix(new_object_matrix);


	//float3 object_z = { 0,0,1 }; 

	////Get the director vector which the object/particle should be pointing at 
	//float3 direction = object_transform->GetGlobalPosition() - reference->camera_frustum.Pos();
	//direction.Normalize(); 

	//Quat rot = Quat::RotateFromTo(object_z, direction);

	//float3 angles = rot.ToEulerXYZ()*RADTODEG; 

	//object_transform->SetRotation({ 0,0,0 }); 

	//if (reference->camera_frustum.Pos().z > object_transform->GetGlobalPosition().z &&
	//	reference->camera_frustum.Pos().x < object_transform->GetGlobalPosition().x)
	//	angles.y = 90 + (90 - angles.y); 

	//else if(reference->camera_frustum.Pos().z > object_transform->GetGlobalPosition().z &&
	//	reference->camera_frustum.Pos().x > object_transform->GetGlobalPosition().x)
	//	angles.y = -90 - (90 + angles.y);

	//object_transform->SetRotation({ 0, angles.y, 0 });

	return ret; 
}

void ComponentBillboard::Save(Data & data) const
{
	data.AddInt("Type", (int)GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.CreateSection("Billboard");
;
	data.AddInt("Active", IsActive());

	data.CloseSection();
}

void ComponentBillboard::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));

	data.EnterSection("Billboard");

	bool is_active = data.GetBool("Active"); 
	SetActive(is_active);

	data.LeaveSection();
}

ComponentBillboard::~ComponentBillboard()
{
}


