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

	c_transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

	pos = float2::zero;
	size = float2::zero;
	anchor = float2(0.5f, 0.5f);

	UpdateTransform();
}

ComponentRectTransform::~ComponentRectTransform()
{
}

bool ComponentRectTransform::Update()
{
	bool ret = true;

	return ret;
}

ComponentTransform * ComponentRectTransform::GetCompTransform() const
{
	return c_transform;
}

ComponentTransform * ComponentRectTransform::GetParentCompTransform() const
{
	ComponentTransform* ret = nullptr;

	if(GetHasParent())
		ret = (ComponentTransform*)GetGameObject()->GetParent()->GetComponent(Component::CompTransform);
	
	return ret;
}

ComponentRectTransform * ComponentRectTransform::GetParentCompRectTransform() const
{
	ComponentRectTransform* ret = nullptr;

	if (GetHasParent())
		ret = (ComponentRectTransform*)GetGameObject()->GetParent()->GetComponent(Component::CompRectTransform);

	return ret;
}

float4x4 ComponentRectTransform::GetMatrix() const
{
	return c_transform->GetMatrix();
}

float4x4 ComponentRectTransform::GetOriginMatrix() const
{
	float4x4 ret = float4x4::identity;

	ret = c_transform->GetMatrix();
	ret[0][3] -= (size.x/2);
	ret[1][3] -= (size.y/2);

	return ret;
}

ComponentCanvas * ComponentRectTransform::GetCanvas(bool& go_is_canvas)
{
	ComponentCanvas* ret = (ComponentCanvas*)GetGameObject()->GetComponent(Component::CompCanvas);

	// Is the canvas
	if (ret != nullptr)
	{
		go_is_canvas = true;

		return ret;
	}

	// Has a parent canvas
	GameObject* parent = GetGameObject()->GetParent();

	while (parent != nullptr)
	{
		ret = (ComponentCanvas*)parent->GetComponent(Component::CompCanvas);

		if (ret != nullptr)
		{
			go_is_canvas = false;
			return ret;
		}
		else
		{
			parent = parent->GetParent();
		}
	}

	go_is_canvas = false;
	return ret;
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

float2 ComponentRectTransform::GetGlobalPos() const
{
	float2 ret = float2::zero;

	ComponentTransform* c_trans = GetCompTransform();

	if (c_trans != nullptr)
	{
		float4x4 transform = c_trans->GetMatrix();

		ret = float2(transform[0][3], transform[1][3]);
	}

	return ret;
}

float4x4 ComponentRectTransform::GetPositionTransform()
{
	float4x4 anchor_trans = GetAnchorTransform();

	float4x4 final_trans = anchor_trans;
	final_trans[0][3] += pos.x;
	final_trans[1][3] += pos.y;

	return final_trans;
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

float2 ComponentRectTransform::GetGlobalAnchor()
{
	float2 ret = float2::zero;

	float4x4 anchor_trans = GetAnchorTransform();

	ret = float2(anchor_trans[0][3], anchor_trans[1][3]);

	return ret;
}

float4x4 ComponentRectTransform::GetAnchorTransform()
{
	float4x4 anchor_trans = float4x4::identity;
	
	ComponentRectTransform* parent_c_rect_trans = GetParentCompRectTransform();

	if (GetHasParent() && parent_c_rect_trans != nullptr)
	{
		float4x4 parent_matrix_orig = parent_c_rect_trans->GetOriginMatrix();
		float2 parent_size = parent_c_rect_trans->GetSize();

		anchor_trans = parent_matrix_orig;
		anchor_trans[0][3] += (anchor.x * parent_size.x);
		anchor_trans[1][3] += (anchor.y * parent_size.y);
	}
	else
	{
		anchor_trans = c_transform->GetMatrix();
	}

	return anchor_trans;
}

void ComponentRectTransform::Save(Data & data) const
{
}

void ComponentRectTransform::Load(Data & data)
{
}

bool ComponentRectTransform::GetHasParent() const
{
	return GetGameObject()->GetParent() != nullptr;
}

void ComponentRectTransform::UpdateTransform()
{
	c_transform->SetMatrix(GetPositionTransform());
}

void ComponentRectTransform::UpdateRectTransform()
{
	std::list<GameObject*> to_change;

	to_change.push_back(GetGameObject());

	std::list<GameObject*>::iterator it = to_change.begin();

	while (to_change.size() > 0)
	{
		ComponentRectTransform* c_rect_trans = (ComponentRectTransform*)(*it)->GetComponent(Component::CompRectTransform);

		if (c_rect_trans != nullptr)
		{
			float4x4 anchor_trans = c_rect_trans->GetAnchorTransform();
			float4x4 transform = c_rect_trans->GetMatrix();

			float2 new_pos = float2::zero;
			new_pos.x = transform[0][3] - anchor_trans[0][3];
			new_pos.y = transform[1][3] - anchor_trans[1][3];

			c_rect_trans->SetPos(new_pos);
		}

		for (std::list<GameObject*>::iterator ch = (*it)->childs.begin(); ch != (*it)->childs.end(); ++ch)
		{
			to_change.push_back(*ch);
		}

		to_change.erase(it);

		it = to_change.begin();
	}
}
