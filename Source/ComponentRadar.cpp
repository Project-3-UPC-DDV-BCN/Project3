#include "ComponentRadar.h"

ComponentRadar::ComponentRadar(GameObject * attached_gameobject)
{
}

ComponentRadar::~ComponentRadar()
{
}

bool ComponentRadar::Update()
{
	return false;
}

void ComponentRadar::Save(Data & data) const
{
}

void ComponentRadar::Load(Data & data)
{
}

ComponentCanvas * ComponentRadar::GetCanvas()
{
	return nullptr;
}

ComponentRectTransform * ComponentRadar::GetRectTrans()
{
	return nullptr;
}
