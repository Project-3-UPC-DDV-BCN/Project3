#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentRectTransform.h"
#include "ComponentTransform.h"

ComponentCanvas::ComponentCanvas(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Canvas");
	SetType(ComponentType::CompCanvas);
	SetGameObject(attached_gameobject);
	size = float2::zero;
}

ComponentCanvas::~ComponentCanvas()
{
}

float2 ComponentCanvas::GetSize()
{
	return size;
}

float4x4 ComponentCanvas::GetOrigin()
{
	float4x4 origin = float4x4::identity;

	ComponentTransform* c_trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

	if (c_trans != nullptr)
	{
		float4x4 canvas_trans = c_trans->GetMatrix();
		float2 canvas_size = GetSize();

		origin = canvas_trans;
		origin[0][3] -= (canvas_size.x / 2);
		origin[1][3] -= (canvas_size.y / 2);
	}

	return origin;
}

bool ComponentCanvas::Update()
{
	bool ret = true;

	return ret;
}

void ComponentCanvas::Save(Data & data) const
{
}

void ComponentCanvas::Load(Data & data)
{
}

