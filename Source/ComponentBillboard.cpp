#include "ComponentBillboard.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"

ComponentBillboard::ComponentBillboard(GameObject* attached_gameobject)
{
	SetGameObject(attached_gameobject);
	SetActive(true);
	SetType(Component::CompBillboard);

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

bool ComponentBillboard::RotateObject()
{
	bool ret = true; 

	//Get the director vector which the object/particle is currently pointing at (Z axis)
	ComponentTransform* object_transform = (ComponentTransform*)GetGameObject()->GetComponent(ComponentType::CompTransform);

	//Get the director vector which the object/particle should be pointing at 

	//Get the difference angle which we should increment depending on the billboard type

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


