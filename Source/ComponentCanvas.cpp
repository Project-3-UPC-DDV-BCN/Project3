#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentRectTransform.h"
#include "ComponentTransform.h"
#include "GameWindow.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "Mesh.h"
#include "MathGeoLib\LineSegment.h"

ComponentCanvas::ComponentCanvas(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Canvas");
	SetType(ComponentType::CompCanvas);
	SetGameObject(attached_gameobject);

	c_rect_trans = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	size = float2(1280, 720);
	last_size = size;
	reference_size = size;

	render_mode = CanvasRenderMode::RENDERMODE_SCREEN_SPACE;
	scale_mode = CanvasScaleMode::SCALEMODE_CONSTANT_SIZE;
	scale = 1.0f;
	
	c_rect_trans->SetInteractable(false);

	controller_selection = false;
	controller_id = 0; 
}

ComponentCanvas::~ComponentCanvas()
{
}

bool ComponentCanvas::Update()
{
	bool ret = true;

	BROFILER_CATEGORY("Component - Canvas - Update", Profiler::Color::Beige);

	App->renderer3D->AddCanvasToDraw(this);

	UpdateSize();

	return ret;
}

bool ComponentCanvas::CleanUp()
{
	bool ret = true;

	App->renderer3D->RemoveCanvasToDraw(this);

	return ret;
}

ComponentRectTransform * ComponentCanvas::GetCompRectTransform() const
{
	return c_rect_trans;
}

void ComponentCanvas::SetRenderCamera(ComponentCamera * _render_camera)
{
	render_camera = _render_camera;
}

ComponentCamera * ComponentCanvas::GetRenderCamera() const
{
	return render_camera;
}

void ComponentCanvas::SetRenderMode(CanvasRenderMode mode)
{
	render_mode = mode;
}

CanvasRenderMode ComponentCanvas::GetRenderMode() const
{
	return render_mode;
}

void ComponentCanvas::SetScaleMode(CanvasScaleMode _scale_mode)
{
	scale_mode = _scale_mode;
}

CanvasScaleMode ComponentCanvas::GetScaleMode() const
{
	return scale_mode;
}

void ComponentCanvas::SetSize(const float2 & _size)
{
	if (size.x < 0)
		size.x = 0;

	if (size.y < 0)
		size.y = 0;

	size = _size;
}

float2 ComponentCanvas::GetSize() const
{
	float2 ret = float2::zero;

	switch (render_mode)
	{
	case CanvasRenderMode::RENDERMODE_SCREEN_SPACE:
		ret = App->editor->game_window->GetSize();
		break;

	case CanvasRenderMode::RENDERMODE_WORLD_SPACE:
		ret = size;
		break;
	}

	return ret;
}

void ComponentCanvas::SetReferenceSize(const float2 & _reference_size)
{
	reference_size = _reference_size;
}

float2 ComponentCanvas::GetReferenceSize() const
{
	return reference_size;
}

void ComponentCanvas::SetScale(const float & _scale)
{
	scale = _scale;

	if (scale < 0)
	{
		scale = 0;
	}

}

float ComponentCanvas::GetScale() const
{
	float ret = 0;

	switch (scale_mode)
	{
	case CanvasScaleMode::SCALEMODE_CONSTANT_SIZE:
		ret = scale;
		break;
	case CanvasScaleMode::SCALEMODE_WITH_SCREEN_SIZE:
		float ratio_x = GetSize().x / reference_size.x;
		float ratio_y = GetSize().y / reference_size.y;

		ret = ratio_x;

		break;
	}

	return ret;
}

void ComponentCanvas::UpdateSize()
{
	last_size = GetSize();
	c_rect_trans->SetSize(GetSize());
}

void ComponentCanvas::AddDrawElement(CanvasDrawElement de)
{
	draws.push_back(de);
}

void ComponentCanvas::ClearDrawElements()
{
	draws.clear();
}

std::vector<CanvasDrawElement> ComponentCanvas::GetDrawElements()
{
	return draws;
}

void ComponentCanvas::SetLastRectTransform(ComponentRectTransform * last)
{
	last_rect_trans = last;
}

ComponentRectTransform * ComponentCanvas::GetLastRectTransform() const
{
	return last_rect_trans;
}

void ComponentCanvas::SetLastLastRectTransform(ComponentRectTransform * last)
{
	last_last_rect_trans = last;
}

ComponentRectTransform * ComponentCanvas::GetLastLastRectTransform() const
{
	return last_last_rect_trans;
}

void ComponentCanvas::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddVector2("size", size);
	data.AddVector2("reference_size", reference_size);
	data.AddInt("render_mode", render_mode);
	data.AddInt("scale_mode", scale_mode);
	data.AddFloat("scale", scale);

}

void ComponentCanvas::Load(Data & data)
{
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetSize(data.GetVector2("size"));
	SetReferenceSize(data.GetVector2("reference_size"));
	SetRenderMode(static_cast<CanvasRenderMode>(data.GetInt("render_mode")));
	SetScaleMode(static_cast<CanvasScaleMode>(data.GetInt("scale_mode")));
	SetScale(data.GetFloat("scale"));
}


ComponentRectTransform * ComponentCanvas::GetSelectedRect()
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).second->GetID() == controller_id)
			return (*it).second;
	}

	return nullptr; 
}

uint ComponentCanvas::GetCurrentID()
{
	return controller_id;
}

void ComponentCanvas::SetCurrentID(uint new_id)
{
	controller_id = new_id; 
}

bool ComponentCanvas::IsOrderRepeated(int order_to_test)
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).first == order_to_test)
			return true; 
	}
	return false; 
}

bool ComponentCanvas::IsIDInController(int id)
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).second->GetID() == id)
			return true;
	}
	return false;
}

void ComponentCanvas::AdvanceCursor()
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).second->GetID() == controller_id)
		{
			it++;

			if (it == controler_elements.end())
			{
				it = controler_elements.begin();
			}		
			

			if ((*it).second == nullptr)
			{
				controller_id = controler_elements[0]->GetID(); 
			}
			else
			{
				controller_id = (*it).second->GetID();
				break;
			}
			
		}			
	}
}

void ComponentCanvas::RegressCursor()
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).second->GetID() == controller_id)
		{
			if (it == controler_elements.begin())
			{
				it = controler_elements.end();
			}

			it--; 
			
			if ((*it).second == nullptr)
			{
				int count = controler_elements.size(); 
				controller_id = controler_elements[count - 1]->GetID();
			}
			else if (false) // Is Locked
			{

			}
			else
			{
				controller_id = (*it).second->GetID();
				break;
			}
		}
	}
}

void ComponentCanvas::PrintCursor()
{
	for (std::map<int, ComponentRectTransform*>::iterator it = controler_elements.begin(); it != controler_elements.end(); it++)
	{
		if ((*it).second->GetID() == controller_id)
		{
			(*it).second->SetOnMouseOver(true); 
		}
		else
		{
			(*it).second->SetOnMouseOver(false);
		}
			
	}
}

CanvasDrawElement::CanvasDrawElement(ComponentCanvas* _canvas, Component* _cmp)
{
	canvas = _canvas;
	cmp = _cmp;

	plane = App->resources->GetMesh("PrimitivePlane");
	texture_id = 0;
	transform = float4x4::identity;
	size = float2::zero;
	pos = float3::zero;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	vertical_flip = false;
	horizontal_flip = false;

	plane->box.SetNegativeInfinity();
	plane->box.Enclose((vec*)plane->vertices, plane->num_vertices);
}

void CanvasDrawElement::SetLayer(int _layer)
{
	layer = _layer;
}

void CanvasDrawElement::SetPosition(const float2& _pos)
{
	pos = float3(_pos.x, _pos.y, 0);
}

void CanvasDrawElement::SetPosition(const float3 & _pos)
{
	pos = _pos;
}

void CanvasDrawElement::SetSize(const float2& _size)
{
	size = _size;
}

void CanvasDrawElement::SetTransform(const float4x4& trans)
{
	transform = trans;
}

void CanvasDrawElement::SetOrtoTransform(const float4x4 & trans)
{
	orto_transform = trans;
}

void CanvasDrawElement::SetTextureId(const uint& id)
{
	texture_id = id;
}

void CanvasDrawElement::SetColour(const float4 & _colour)
{
	colour = _colour;
}

void CanvasDrawElement::SetFlip(const bool& _vertical_flip, const bool& _horizontal_flip)
{
	vertical_flip = _vertical_flip;
	horizontal_flip = _horizontal_flip;
}

int CanvasDrawElement::GetLayer()
{
	return layer;
}

Component * CanvasDrawElement::GetComponent()
{
	return cmp;
}

float4x4 CanvasDrawElement::GetTransform()
{
	float4x4 ret = float4x4::identity;

	float4x4 size_trans = float4x4::identity;

	if (size.x == 0)
		size.x = 1;

	if (size.y == 0)
		size.y = 1;

	float2 flip_multiplicator = float2(1, 1);

	if (horizontal_flip)
		flip_multiplicator.x = -1;

	if (vertical_flip)
		flip_multiplicator.y = -1;

	size_trans = ret.FromTRS(float3(pos.x, pos.y, pos.z), Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(size.x * flip_multiplicator.x, 1, -size.y * flip_multiplicator.y));

	ret = transform * size_trans;

	return ret;;
}

float4x4 CanvasDrawElement::GetOrtoTransform() const
{
	float4x4 ret = float4x4::identity;

	float4x4 size_trans = float4x4::identity;

	float2 flip_multiplicator = float2(1, 1);

	if (horizontal_flip)
		flip_multiplicator.x = -1;

	if (vertical_flip)
		flip_multiplicator.y = -1;

	size_trans = ret.FromTRS(float3(pos.x, pos.y, pos.z), Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(size.x * flip_multiplicator.x, 1, -size.y * flip_multiplicator.y));

	ret = orto_transform * size_trans;

	return ret;
}

uint CanvasDrawElement::GetTextureId() const
{
	return texture_id;
}

float4 CanvasDrawElement::GetColour() const
{
	return colour;
}

Mesh * CanvasDrawElement::GetPlane() const
{
	return plane;
}

bool CanvasDrawElement::CheckRay(LineSegment ray, CanvasRenderMode mode)
{
	bool ret = false;

	AABB box;

	if(mode == CanvasRenderMode::RENDERMODE_WORLD_SPACE)
	{
		float4x4 trans = GetTransform();
		trans[0][3] += 3;
		trans[1][3] -= 3;

		box = GetBBox();
		ray.Transform(trans.Inverted());
	}
	else 
	{
		float4x4 canvas_matrix = canvas->GetCompRectTransform()->GetMatrix();

		float3 pos;
		Quat rot;
		float3 scal;
		canvas_matrix.Decompose(pos, rot, scal);

		scal.x *= canvas->GetSize().x;
		scal.y *= canvas->GetSize().y;

		scal.x /= 2;
		scal.y /= 2;

		float4x4 trans = GetTransform();

		float4x4 ortho_trans = GetOrtoTransform();
		ortho_trans[0][3] = -ortho_trans[0][3] + scal.x - 7;
		ortho_trans[1][3] += scal.y - 7;
		
		box = GetOrthoBBox();
		ray.Transform(ortho_trans.Inverted());
	}

	if (ray.Intersects(box))
	{
		ret = true;
	}

	return ret;
}

AABB CanvasDrawElement::GetBBox()
{
	plane->box.Transform(GetTransform());
	return plane->box;
}

AABB CanvasDrawElement::GetOrthoBBox()
{
	plane->box.Transform(GetOrtoTransform());
	return plane->box;
}
