#include "ComponentRectTransform.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCanvas.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "DebugDraw.h"

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
	rotation = float3::zero;
	scale = 1;
	snap_up = true;
	snap_down = true;
	snap_left = true;
	snap_right = true;

	c_transform->SetPosition(float3(0, 0, 0));
	UpdateTransform();
}

ComponentRectTransform::~ComponentRectTransform()
{
}

bool ComponentRectTransform::Update()
{
	bool ret = true;

	float4x4 global_anchor_trans = GetAnchorTransform(); global_anchor_trans.RemoveScale();
	float4x4 global_origin_trans = GetOriginMatrix(); global_origin_trans.RemoveScale();

	App->renderer3D->GetDebugDraw()->Quad(GetMatrix(), GetScaledSize(), float4(0.0f, 0.5f, 1.0f, 1.0f));
	App->renderer3D->GetDebugDraw()->Circle(global_anchor_trans, 8, float4(0.0f, 0.8f, 0.0f, 1.0f));
	App->renderer3D->GetDebugDraw()->Circle(global_origin_trans, 1, float4(1, 0.0f, 0.0f, 1.0f), 5);
	App->renderer3D->GetDebugDraw()->Line(GetAnchorGlobalPos(), GetGlobalPos(), float4(0.0f, 0.8f, 0.0f, 1.0f));

	bool is_canvas;
	ComponentCanvas* cv = GetCanvas(is_canvas);
	if (cv != nullptr && !is_canvas && scale != cv->GetScale())
	{
		scale = cv->GetScale();
		UpdateTransformAndChilds();
	}

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

ComponentCanvas * ComponentRectTransform::GetCanvas()
{
	bool is_canvas;
	return GetCanvas(is_canvas);
}

float4x4 ComponentRectTransform::GetMatrix() const
{
	return c_transform->GetMatrix();
}

float4x4 ComponentRectTransform::GetOrtoMatrix() 
{
	float4x4 ret = float4x4::identity;

	ComponentCanvas* canvas = GetCanvas();

	ComponentRectTransform* rect_trans = canvas->GetCompRectTransform();

	float4x4 canvas_matrix = float4x4::identity;

	if (rect_trans != nullptr)
	{
		canvas_matrix = rect_trans->GetMatrix();
	}

	float4x4 matrix = GetMatrix();

	ret = matrix * canvas_matrix.Inverted();

	return ret;
}

float3 ComponentRectTransform::GetOriginLocalPos()
{
	return float3(-GetScaledSize().x/2, -GetScaledSize().y/2, 0);
}

float3 ComponentRectTransform::GetOriginGlobalPos()
{
	float4x4 mat = GetOriginMatrix();

	float3 transform;
	Quat rot;
	float3 sca;
	mat.Decompose(transform, rot, sca);

	return transform;
}

float4x4 ComponentRectTransform::GetOriginMatrix() const
{
	float4x4 ret = float4x4::identity;

	ret = c_transform->GetMatrix();

	float4x4 movement = ret.FromTRS(float3(-(GetScaledSize().x / 2), -(GetScaledSize().y / 2), 0), Quat::identity, float3::one);

	ret = c_transform->GetMatrix() * movement;

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

float2 ComponentRectTransform::GetScaledPos()
{
	float2 ret = pos;
	float2 size_added = GetScaledSize() - size;

	if (snap_right)
	{
		ret.x += size_added.x / 2;
	}
	if (snap_left)
	{
		ret.x -= size_added.x / 2;
	}
	if (snap_up)
	{
		ret.y += size_added.y / 2;
	}
	if (snap_down)
	{
		ret.y -= size_added.y / 2;
	}

	return ret;
}

float3 ComponentRectTransform::GetLocalPos() const
{
	return c_transform->GetLocalPosition();
}

float3 ComponentRectTransform::GetGlobalPos() const
{
	return c_transform->GetGlobalPosition();
}

void ComponentRectTransform::SetRotation(const float3 & _rotation)
{
	rotation = _rotation;

	UpdateTransform();
}

float3 ComponentRectTransform::GetLocalRotation() const
{
	return c_transform->GetLocalRotation();
}

void ComponentRectTransform::SetSize(const float2 & _size)
{
	size = _size;

	if (size.x < 0)
		size.x = 0;

	if (size.y < 0)
		size.y = 0;

	UpdateTransformAndChilds();
}

float2 ComponentRectTransform::GetSize() const
{
	return size;
}

float2 ComponentRectTransform::GetScaledSize() const
{
	float2 ret = size;

	if (snap_right || snap_left)
	{
		ret.x *= scale;
	}
	if (snap_up || snap_down)
	{
		ret.y *= scale;
	}

	return ret;
}

void ComponentRectTransform::SetAnchor(const float2 & _anchor)
{
	bool is_canvas = false;
	GetCanvas(is_canvas);

	if (!is_canvas)
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
	else
	{
		anchor = float2(0.5f, 0.5f);
	}
}

float2 ComponentRectTransform::GetAnchor() const
{
	return anchor;
}

float3 ComponentRectTransform::GetAnchorLocalPos()
{
	float3 ret = float3::zero;

	ComponentRectTransform* parent_c_rect_trans = GetParentCompRectTransform();

	if (parent_c_rect_trans != nullptr)
	{
		float2 parent_size = parent_c_rect_trans->GetScaledSize();

		ret = parent_c_rect_trans->GetOriginLocalPos();

		ret.x += anchor.x * parent_size.x;
		ret.y += anchor.y * parent_size.y;
	}

	return ret;
}

float3 ComponentRectTransform::GetAnchorGlobalPos()
{
	float3 ret = float3::zero;

	float4x4 anchor_trans = GetAnchorTransform();

	float3 transform;
	Quat rot;
	float3 sca;
	anchor_trans.Decompose(transform, rot, sca);

	ret = transform;

	return ret;
}

float4x4 ComponentRectTransform::GetAnchorTransform()
{
	float4x4 anchor_trans = float4x4::identity;
	
	ComponentRectTransform* parent_c_rect_trans = GetParentCompRectTransform();

	if (GetHasParent() && parent_c_rect_trans != nullptr)
	{
		float4x4 parent_matrix_orig = parent_c_rect_trans->GetOriginMatrix();
		float2 parent_size = parent_c_rect_trans->GetScaledSize();

		anchor_trans = parent_matrix_orig;

		float4x4 movement = anchor_trans.FromTRS(float3((anchor.x * parent_size.x), (anchor.y * parent_size.y), 0), Quat::identity, float3::one);

		anchor_trans = parent_matrix_orig * movement;
	}
	else
	{
		anchor_trans = c_transform->GetMatrix();
	}

	return anchor_trans;
}

void ComponentRectTransform::SetScale(const float & _scale)
{
	scale = _scale;

	UpdateTransformAndChilds();
}

float ComponentRectTransform::GetScale() const
{
	return scale;
}

void ComponentRectTransform::SetSnapUp(bool set)
{
	snap_up = set;

	UpdateTransform();
}

void ComponentRectTransform::SetSnapDown(bool set)
{
	snap_down = set;

	UpdateTransform();
}

void ComponentRectTransform::SetSnapLeft(bool set)
{
	snap_left = set;

	UpdateTransform();
}

void ComponentRectTransform::SetSnapRight(bool set)
{
	snap_right = set;

	UpdateTransform();
}

bool ComponentRectTransform::GetSnapUp()
{
	return snap_up;
}

bool ComponentRectTransform::GetSnapDown()
{
	return snap_down;
}

bool ComponentRectTransform::GetSnapLeft()
{
	return snap_left;
}

bool ComponentRectTransform::GetSnapRight()
{
	return snap_right;
}

float3 ComponentRectTransform::GetPreferedPos()
{
	float3 ret = float3::zero;

	float3 anchor_pos = GetAnchorLocalPos();

	ret.x = anchor_pos.x + GetScaledPos().x;
	ret.y = anchor_pos.y + GetScaledPos().y;

	return ret;
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
	float3 prefered_pos = GetPreferedPos();

	c_transform->SetPosition(float3(prefered_pos.x, prefered_pos.y, prefered_pos.z));

	c_transform->SetRotation(rotation);
}

void ComponentRectTransform::UpdateTransformAndChilds()
{
	std::list<GameObject*> to_change;

	to_change.push_back(GetGameObject());

	for (std::list<GameObject*>::iterator ch = GetGameObject()->childs.begin(); ch != GetGameObject()->childs.end(); ++ch)
	{
		to_change.push_back(*ch);
	}

	std::list<GameObject*>::iterator it = to_change.begin();

	while (to_change.size() > 0)
	{
		ComponentRectTransform* c_rect_trans = (ComponentRectTransform*)(*it)->GetComponent(Component::CompRectTransform);

		if (c_rect_trans != nullptr)
		{
			c_rect_trans->UpdateTransform();
		}

		to_change.erase(it);

		it = to_change.begin();
	}
}

void ComponentRectTransform::UpdateRectTransform()
{
	//float4x4 anchor_trans = GetAnchorTransform();
	//float4x4 transform = GetMatrix();

	//float2 new_pos = float2::zero;
	//new_pos.x = transform[0][3] - anchor_trans[0][3];
	//new_pos.y = transform[1][3] - anchor_trans[1][3];

	//SetPos(new_pos);
}

void ComponentRectTransform::UpdateRectTransformAndChilds()
{
	std::list<GameObject*> to_change;

	to_change.push_back(GetGameObject());

	std::list<GameObject*>::iterator it = to_change.begin();

	while (to_change.size() > 0)
	{
		ComponentRectTransform* c_rect_trans = (ComponentRectTransform*)(*it)->GetComponent(Component::CompRectTransform);

		if (c_rect_trans != nullptr)
		{
			c_rect_trans->UpdateRectTransform();
		}

		for (std::list<GameObject*>::iterator ch = (*it)->childs.begin(); ch != (*it)->childs.end(); ++ch)
		{
			to_change.push_back(*ch);
		}

		to_change.erase(it);

		it = to_change.begin();
	}
}
