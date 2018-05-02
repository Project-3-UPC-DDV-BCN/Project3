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

	if (reference == nullptr || billboarding_type == BILLBOARD_NONE || IsActive() == false)
		return false;

	//Get the director vector which the object/particle is currently pointing at (Z axis)
	ComponentTransform* object_transform;

	float3 reference_axis = { 0,0,-1 };

	if (!attached_to_particle)
		object_transform = (ComponentTransform*)GetGameObject()->GetComponent(ComponentType::CompTransform);
	else
		object_transform = (ComponentTransform*)particle_attached->components.particle_transform;

	//Get the rotation of the current camera & construct the matrix
	float3 new_object_x; 
	float3 new_object_y;
	float3 new_object_z;

	float3 dir = object_transform->GetGlobalPosition() - reference->GetFrustum().Pos();
	float3 up = reference->GetFrustum().Up();
	dir = dir.Normalized();

	new_object_x = up.Cross(dir);
	new_object_x = new_object_x.Normalized();

	new_object_y = dir.Cross(new_object_x);
	new_object_y = new_object_y.Normalized();

	float3x3 matrix(new_object_x.x, new_object_y.x, dir.x, new_object_x.y, new_object_y.y, dir.y, new_object_x.z, new_object_y.z, dir.z);

	//Apply the matrix
	float4x4 new_mat = float4x4::FromTRS(object_transform->GetGlobalPosition(), matrix, object_transform->GetGlobalScale());
	
	//Apply it into the plane
	object_transform->SetMatrix(new_mat);

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
	data.AddInt("Billboard_Type", billboarding_type);

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
	billboarding_type = (BillboardingType)data.GetInt("Billboard_Type");

	data.LeaveSection();
}

ComponentBillboard::~ComponentBillboard()
{
}


