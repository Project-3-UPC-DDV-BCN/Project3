#include "ComponentRectTransform.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ComponentRectTransfrom::ComponentRectTransfrom(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("RectTransform");
	SetType(ComponentType::CompRectTransform);
	SetGameObject(attached_gameobject);

	c_transform = (ComponentTransform*)attached_gameobject->GetComponent(Component::CompTransform);

	rect = float4(0, 0, 0, 0);
	anchor = float2(0.0f, 0.0f);
}

ComponentRectTransfrom::~ComponentRectTransfrom()
{
}

bool ComponentRectTransfrom::Update()
{
	return false;
}

void ComponentRectTransfrom::SetRect(const float4 & _rect)
{
	rect = _rect;
}

float4 ComponentRectTransfrom::GetRect() const
{
	return rect;
}

void ComponentRectTransfrom::SetAnchor(const float2 & _anchor)
{
	anchor = _anchor;
}

float2 ComponentRectTransfrom::GetAnchor() const
{
	return anchor;
}

