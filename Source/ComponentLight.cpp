#include "ComponentLight.h"
#include "GameObject.h"

ComponentLight::ComponentLight(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Light");
	SetType(ComponentType::CompLight);
	SetGameObject(attached_gameobject);
}

ComponentLight::~ComponentLight()
{
}

void ComponentLight::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
}

void ComponentLight::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
}
