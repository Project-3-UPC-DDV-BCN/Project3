#include "ComponentRectTransform.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCanvas.h"

ComponentRectTransform::ComponentRectTransform(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("RectTransform");
	SetType(ComponentType::CompRectTransform);
	SetGameObject(attached_gameobject);

	c_transform = (ComponentTransform*)attached_gameobject->GetComponent(Component::CompTransform);

	pos = float2::zero;
	size = float2::zero;
	anchor = float2::zero;
}

ComponentRectTransform::~ComponentRectTransform()
{
}

bool ComponentRectTransform::Update()
{
	return false;
}

void ComponentRectTransform::SetPos(const float2 & _pos)
{
	pos = _pos;

	UpdateTransform();
}

float2 ComponentRectTransform::GetPos() const
{
	return pos;
}

void ComponentRectTransform::SetSize(const float2 & _size)
{
	size = _size;

	UpdateTransform();
}

float2 ComponentRectTransform::GetSize() const
{
	return size;
}

void ComponentRectTransform::SetAnchor(const float2 & _anchor)
{
	anchor = _anchor;

	if (anchor.x < 0)
		anchor.x = 0;
	
	if (anchor.x > 1)
		anchor.x = 1;

	if (anchor.y < 0)
		anchor.y = 0;

	if (anchor.y > 1)
		anchor.y = 1;

	UpdateTransform();
}

float2 ComponentRectTransform::GetAnchor() const
{
	return anchor;
}

float4x4 ComponentRectTransform::GetAnchorTransform() const
{
	float4x4 anchor_trans = float4x4::identity;

	if (GetHasCanvas())
	{
		float2 canvas_size = c_canvas->GetSize();
		float4x4 canvas_origin = c_canvas->GetOrigin();

		anchor_trans[0][3] += (canvas_size.x * anchor.x);
		anchor_trans[0][3] += (canvas_size.y * anchor.y);
	}
	
	return anchor_trans;
}

bool ComponentRectTransform::GetHasCanvas() const
{
	return c_canvas != nullptr;
}

void ComponentRectTransform::Save(Data & data) const
{
}

void ComponentRectTransform::Load(Data & data)
{
}

void ComponentRectTransform::LookForCanvas()
{
	GameObject* parent = GetGameObject();

	while (parent != nullptr)
	{
		ComponentCanvas* canv = (ComponentCanvas*)parent->GetComponent(Component::CompCanvas);

		if (canv != nullptr)
		{
			c_canvas = canv;
			break;
		}
		else
		{
			parent = parent->GetParent();
		}
	}
}

void ComponentRectTransform::UpdateTransform()
{
	LookForCanvas();

	if (!GetHasCanvas())
		return;

	ComponentTransform* c_trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

	if (c_trans != nullptr)
	{
		float2 canvas_size = c_canvas->GetSize();
		float4x4 canvas_origin = c_canvas->GetOrigin();
		float4x4 anchor_trans = GetAnchorTransform();

		float4x4 final_trans = anchor_trans;
		anchor_trans[0][3] += pos.x;
		anchor_trans[1][3] += pos.y;

		c_trans->SetMatrix(final_trans);
	}
}

void ComponentRectTransform::UpdateRectTransform()
{
	LookForCanvas();

	if (!GetHasCanvas())
		return;

	ComponentTransform* c_trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

	if(c_trans != nullptr)
	{
		float4x4 anchor_trans = GetAnchorTransform();
		float4x4 game_object_trans = c_trans->GetMatrix();

		float2 new_pos = float2::zero;
		new_pos.x = game_object_trans[0][3] - anchor_trans[0][3];
		new_pos.y = game_object_trans[1][3] - anchor_trans[1][3];

		pos = new_pos;
	}
}
