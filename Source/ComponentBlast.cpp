#include "ComponentBlast.h"
#include "Material.h"
#include "Application.h"
#include "ModuleResources.h"

ComponentBlast::ComponentBlast(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Blast");
	SetType(ComponentType::CompBlast);
	SetGameObject(attached_gameobject);
	interior_material = nullptr;
}

ComponentBlast::~ComponentBlast()
{

}

void ComponentBlast::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	if (interior_material)
	{
		data.CreateSection("IntMaterial");
		interior_material->Save(data);
		data.CloseSection();
	}
}

void ComponentBlast::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	if (data.EnterSection("IntMaterial"))
	{
		int mat_id = data.GetUInt("UUID");
		interior_material = App->resources->GetMaterial(mat_id);
	}
}
