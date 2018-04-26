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

	new_object_z = object_transform->GetGlobalPosition() - reference->GetFrustum().Pos();
	new_object_z.Normalize(); 
	new_object_z *= -1; 

	new_object_y = reference->GetFrustum().Up();
	new_object_y.Normalize();

	new_object_x = new_object_y.Cross(new_object_z);
	new_object_x.Normalize();

	//Calculate Matrix from axis
	float3 column1, column2, column3;

	column1.x = new_object_x.x; 
	column1.y = new_object_y.x;
	column1.z = new_object_z.x;

	column2.x = new_object_x.y;
	column2.y = new_object_y.y;
	column2.z = new_object_z.y;

	column3.x = new_object_x.z;
	column3.y = new_object_y.z;
	column3.z = new_object_z.z;

	float3x3 matrix = float3x3::identity; 

	matrix.SetCol(0, column1); 
	matrix.SetCol(1, column2);
	matrix.SetCol(2, column3);

	//Apply the matrix
	float4x4 new_mat = float4x4::FromTRS(object_transform->GetGlobalPosition(), matrix, object_transform->GetGlobalScale());
	
	//Apply it into the plane
	//object_transform->SetMatrix(new_mat);

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


