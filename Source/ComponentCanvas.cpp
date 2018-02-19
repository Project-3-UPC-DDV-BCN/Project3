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
}

ComponentCanvas::~ComponentCanvas()
{
}

bool ComponentCanvas::Update()
{
	bool ret = true;

	App->renderer3D->AddCanvasToDraw(this);

	UpdateSize();

	return ret;
}

bool ComponentCanvas::CleanUp()
{
	bool ret = true;


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
		float ratio_x = size.x / reference_size.x;
		float ratio_y = size.y / reference_size.y;

		ret = ratio_x;

		break;
	}

	return ret;
}

void ComponentCanvas::UpdateSize()
{
	if (GetSize().x != last_size.x || GetSize().y != last_size.y)
	{
		last_size = GetSize();
		c_rect_trans->SetSize(GetSize());
	}
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

void ComponentCanvas::Save(Data & data) const
{
}

void ComponentCanvas::Load(Data & data)
{
}

CanvasDrawElement::CanvasDrawElement()
{
	plane = App->resources->GetMesh("PrimitivePlane");
	texture_id = 0;
	transform = float4x4::identity;
	size = float2::zero;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	flip = false;
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

void CanvasDrawElement::SetFlip(const bool & _flip)
{
	flip = _flip;
}

float4x4 CanvasDrawElement::GetTransform()
{
	float4x4 ret = float4x4::identity;

	float4x4 size_trans = float4x4::identity;

	if (size.x == 0)
		size.x = 1;

	if (size.y == 0)
		size.y = 1;

	if(!flip)
		size_trans = ret.FromTRS(float3::zero, Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(-size.x, -1, -size.y));
	else
		size_trans = ret.FromTRS(float3::zero, Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(size.x, 1, size.y));

	ret = transform * size_trans;

	return ret;;
}

float4x4 CanvasDrawElement::GetOrtoTransform() const
{
	float4x4 ret = float4x4::identity;

	float4x4 size_trans = float4x4::identity;

	if (!flip)
		size_trans = ret.FromTRS(float3::zero, Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(-size.x, -1, -size.y));
	else
		size_trans = ret.FromTRS(float3::zero, Quat::FromEulerXYZ(90 * DEGTORAD, 0, 0), float3(size.x, 1, size.y));

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

AABB CanvasDrawElement::GetBBox()
{
	plane->box.Transform(GetTransform());
	return plane->box;
}
