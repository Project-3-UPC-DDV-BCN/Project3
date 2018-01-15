#include "ComponentRectTransform.h"

ComponentRectTransfrom::ComponentRectTransfrom(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("RectTransform");
	SetType(ComponentType::CompRectTransform);
	SetGameObject(attached_gameobject);
}

ComponentRectTransfrom::~ComponentRectTransfrom()
{
}

bool ComponentRectTransfrom::Update()
{
	return false;
}
