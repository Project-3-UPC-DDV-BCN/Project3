#include "ComponentRectTransform.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCanvas.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "DebugDraw.h"
#include "ComponentScript.h"

ComponentRectTransform::ComponentRectTransform(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("RectTransform");
	SetType(ComponentType::CompRectTransform);
	SetGameObject(attached_gameobject);

	c_transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

	layer = 0;
	interactable = true;
	pos = float2::zero;
	z_pos = 0;
	size = float2::zero;
	anchor = float2(0.5f, 0.5f);
	scale = 1;
	snap_up = true;
	snap_down = true;
	snap_left = true;
	snap_right = true;
	on_click = false;
	on_click_down = false;
	on_click_up = false;
	on_mouse_enter = false;
	on_mouse_over = false;
	on_mouse_out = false;
	fixed_aspect_ratio = false;
	aspect_ratio = 1.0f;

	c_transform->SetPosition(float3(0, 0, 0));
	UpdateTransform();
}

ComponentRectTransform::~ComponentRectTransform()
{
}

bool ComponentRectTransform::Update()
{
	bool ret = true;

	bool is_canvas;

	ComponentCanvas* c_canvas = GetCanvas(is_canvas);
	if (c_canvas != nullptr && !is_canvas && scale != c_canvas->GetScale())
	{
		scale = c_canvas->GetScale();
		UpdateTransformAndChilds();
	}

	if (GetGameObject()->IsSelected() || is_canvas)
	{
		float4x4 global_anchor_trans = GetAnchorTransform(); global_anchor_trans.RemoveScale();
		float4x4 global_origin_trans = GetOriginMatrix(); global_origin_trans.RemoveScale();

		if (!is_canvas)
			App->renderer3D->GetDebugDraw()->Quad(GetMatrix(), GetScaledSize(), float4(0.0f, 0.5f, 1.0f, 1.0f));
		else
			App->renderer3D->GetDebugDraw()->Quad(GetMatrix(), GetScaledSize(), float4(1.0f, 1.0f, 1.0f, 1.0f));

		App->renderer3D->GetDebugDraw()->Circle(global_anchor_trans, 8, float4(0.0f, 0.8f, 0.0f, 1.0f));
		App->renderer3D->GetDebugDraw()->Circle(global_origin_trans, 1, float4(1, 0.0f, 0.0f, 1.0f), 5);
		App->renderer3D->GetDebugDraw()->Line(GetAnchorGlobalPos(), GetGlobalPos(), float4(0.0f, 0.8f, 0.0f, 1.0f));
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

	float4x4 pos_matrix = rect_trans->GetOriginMatrix() * rect_trans->GetMatrix().Inverted();

	float3 pos;
	Quat rot;
	float3 scal;
	pos_matrix.Decompose(pos, rot, scal);

	float4x4 matrix = GetMatrix();

	ret = matrix * canvas_matrix.Inverted();
	ret[0][3] -= pos.x;
	ret[1][3] += pos.y;

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

void ComponentRectTransform::AddPos(const float2 & add)
{
	pos += add;

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

	ret *= scale;

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

void ComponentRectTransform::SetZPos(float set)
{
	if (set != z_pos)
	{
		z_pos = set;
		UpdateTransform();
	}
}

float ComponentRectTransform::GetZPos()
{
	return z_pos;
}

void ComponentRectTransform::SetRotation(const float3 & _rotation)
{
	c_transform->SetRotation(_rotation);

	UpdateTransform();
}

float3 ComponentRectTransform::GetLocalRotation() const
{
	return c_transform->GetLocalRotation();
}

void ComponentRectTransform::SetSize(float2 _size, bool use_fixed_ratio)
{
	if (fixed_aspect_ratio && use_fixed_ratio)
	{
		if (_size.x != size.x)
		{
			_size.y = _size.x / aspect_ratio;
		}
		else if (_size.y != size.y || _size.x == size.x)
		{
			_size.x = _size.y * aspect_ratio;
		}
	}

	float2 scaled_pos = GetScaledPos();

	size = _size;

	if (size.x < 0)
		size.x = 0;

	if (size.y < 0)
		size.y = 0;

	float2 new_scaled_pos = GetScaledPos();

	pos += scaled_pos - new_scaled_pos;

	UpdateTransformAndChilds();
}

void ComponentRectTransform::SetLeftSize(const float & left)
{
	float difference = size.x - left;

	size.x = left;

	pos.x += difference / 2;
}

void ComponentRectTransform::SetRightSize(const float & right)
{
	float difference = size.x - right;

	size.x = right;

	pos.x -= difference / 2;
}

void ComponentRectTransform::SetUpSize(const float & up)
{
	float difference = size.y - up;

	size.y = up;

	pos.y += difference / 2;
}

void ComponentRectTransform::SetDownSize(const float & down)
{
	float difference = size.y - down;

	size.y = down;

	pos.y -= difference / 2;
}

float2 ComponentRectTransform::GetSize() const
{
	return size;
}

float2 ComponentRectTransform::GetScaledSize() const
{
	float2 ret = size;

	ret.x *= GetScaleAxis().x;
	ret.y *= GetScaleAxis().y;

	return ret;
}

void ComponentRectTransform::SetFixedAspectRatio(bool set)
{
	fixed_aspect_ratio = set;
}

bool ComponentRectTransform::GetFixedAspectRatio() const
{
	return fixed_aspect_ratio;
}

void ComponentRectTransform::SetAspectRatio(float _aspect_ratio)
{
	aspect_ratio = _aspect_ratio;
}

float ComponentRectTransform::GetAspectRatio() const
{
	return aspect_ratio;
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

float2 ComponentRectTransform::GetScaleAxis() const
{
	float2 ret = float2::one;

	if (snap_right || snap_left)
	{
		ret.x = scale;
	}
	if (snap_up || snap_down)
	{
		ret.y = scale;
	}

	return ret;
}

void ComponentRectTransform::SetSnapUp(bool set)
{
	if (!set && snap_up)
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_up = set;

		float2 new_scaled_size = GetScaledSize();
		float2 new_size = last_scaled_size - new_scaled_size + size;
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}
	else
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_up = set;

		float2 new_size = float2(last_scaled_size.x / GetScaleAxis().x, last_scaled_size.y / GetScaleAxis().y);
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}

	UpdateTransform();
}

void ComponentRectTransform::SetSnapDown(bool set)
{
	if (!set && snap_down)
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_down = set;

		float2 new_scaled_size = GetScaledSize();
		float2 new_size = last_scaled_size - new_scaled_size + size;
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}
	else
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_down = set;

		float2 new_size = float2(last_scaled_size.x / GetScaleAxis().x, last_scaled_size.y / GetScaleAxis().y);
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}

	UpdateTransform();
}

void ComponentRectTransform::SetSnapLeft(bool set)
{
	if (!set && snap_left)
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_left = set;

		float2 new_scaled_size = GetScaledSize();
		float2 new_size = last_scaled_size - new_scaled_size + size;
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}
	else
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_left = set;

		float2 new_size = float2(last_scaled_size.x / GetScaleAxis().x, last_scaled_size.y / GetScaleAxis().y);
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}

	UpdateTransform();
}

void ComponentRectTransform::SetSnapRight(bool set)
{
	if (!set && snap_right)
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_right = set;

		float2 new_scaled_size = GetScaledSize();
		float2 new_size = last_scaled_size - new_scaled_size + size;
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}
	else
	{
		float2 last_scaled_pos = GetScaledPos();
		float2 last_scaled_size = GetScaledSize();

		snap_right = set;

		float2 new_size = float2(last_scaled_size.x / GetScaleAxis().x, last_scaled_size.y / GetScaleAxis().y);
		SetSize(new_size, false);

		float2 new_scaled_pos = GetScaledPos();
		float2 new_pos = last_scaled_pos - new_scaled_pos + pos;
		SetPos(new_pos);
	}

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
	float2 scaled_pos = GetScaledPos();

	ret.x = anchor_pos.x + scaled_pos.x;
	ret.y = anchor_pos.y + scaled_pos.y;
	ret.z = z_pos;

	return ret;
}

void ComponentRectTransform::SetLayer(int layer)
{
}

int ComponentRectTransform::GetLayer()
{
	int ret = 0;

	ComponentCanvas* cv = GetCanvas();

	if (cv != nullptr)
	{
		if(cv->GetRenderMode() == CanvasRenderMode::RENDERMODE_SCREEN_SPACE)
		{
			ret = layer;
		}
		else
		{
			ret = 0;
		}
	}

	return ret;
}

void ComponentRectTransform::SetInteractable(bool set)
{
	interactable = set;
}

bool ComponentRectTransform::GetInteractable() const
{
	return interactable;
}

void ComponentRectTransform::SetOnClick(bool set)
{
	on_click = set;
}

void ComponentRectTransform::SetOnClickDown(bool set)
{
	on_click_down = set;
}

void ComponentRectTransform::SetOnClickUp(bool set)
{
	on_click_up = set;
}

void ComponentRectTransform::SetOnMouseEnter(bool set)
{
	on_mouse_enter = set;
}

void ComponentRectTransform::SetOnMouseOver(bool set)
{
	on_mouse_over = set;
}

void ComponentRectTransform::SetOnMouseOut(bool set)
{
	on_mouse_out = set;
}

bool ComponentRectTransform::GetOnClick()
{
	if (GetHasParentInactive())
		on_click = false;

	return on_click;
}

bool ComponentRectTransform::GetOnClickDown() 
{
	if (GetHasParentInactive())
		on_click_down = false;

	return on_click_down;
}

bool ComponentRectTransform::GetOnClickUp() 
{
	if (GetHasParentInactive())
		on_click_up = false;

	return on_click_up;
}

bool ComponentRectTransform::GetOnMouseEnter() 
{
	if (GetHasParentInactive())
		on_mouse_enter = false;

	return on_mouse_enter;
}

bool ComponentRectTransform::GetOnMouseOver() 
{
	if (GetHasParentInactive())
		on_mouse_over = false;

	return on_mouse_over;
}

bool ComponentRectTransform::GetOnMouseOut()
{
	if (GetHasParentInactive())
		on_mouse_out = false;

	return on_mouse_out;
}

void ComponentRectTransform::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddVector2("pos", pos);
	data.AddFloat("z_pos", z_pos);
	data.AddVector2("size", size);
	data.AddVector2("anchor", anchor);
	//data.AddBool("snap_up", snap_up);
	//data.AddBool("snap_down", snap_down);
	//data.AddBool("snap_left", snap_left);
	//data.AddBool("snap_right", snap_right);
	data.AddFloat("scale", 1.0f);
	data.AddBool("interactable", interactable);
	data.AddBool("fixed_aspect_ratio", fixed_aspect_ratio);
	data.AddFloat("fixed_ratio", aspect_ratio);
}

void ComponentRectTransform::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetFixedAspectRatio(data.GetBool("fixed_aspect_ratio"));
	SetAspectRatio(data.GetFloat("fixed_ratio"));
	SetPos(data.GetVector2("pos"));
	SetZPos(data.GetFloat("z_pos"));
	SetSize(data.GetVector2("size"));
	SetAnchor(data.GetVector2("anchor"));
	//SetSnapUp(data.GetBool("snap_up"));
	//SetSnapDown(data.GetBool("snap_down"));
	//SetSnapLeft(data.GetBool("snap_left"));
	//SetSnapRight(data.GetBool("snap_right"));
	SetScale(data.GetFloat("scale"));
	SetInteractable(data.GetBool("interactable"));
}

bool ComponentRectTransform::GetHasParent() const
{
	return GetGameObject()->GetParent() != nullptr;
}

bool ComponentRectTransform::GetHasParentInactive() const
{
	bool ret = false;

	GameObject* parent = GetGameObject()->GetParent();

	while (parent != nullptr)
	{
		if (!parent->IsActive())
		{
			ret = true;
			break;
		}

		parent = parent->GetParent();
	}


	return ret;
}

void ComponentRectTransform::UpdateTransform()
{
	float3 prefered_pos = GetPreferedPos();

	c_transform->SetPosition(float3(prefered_pos.x, prefered_pos.y, prefered_pos.z));
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
	float4x4 anchor_trans = GetAnchorTransform();
	float4x4 transform = GetMatrix();

	float4x4 local_trans = transform * anchor_trans.Inverted();

	float3 pos;
	Quat rot;
	float3 scale;
	local_trans.Decompose(pos, rot, scale);

	SetPos(float2(pos.x, pos.y));
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
