#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentRectTransform.h"
#include "ComponentTransform.h"
#include "GameWindow.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"

ComponentCanvas::ComponentCanvas(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Canvas");
	SetType(ComponentType::CompCanvas);
	SetGameObject(attached_gameobject);

	c_rect_trans = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	size = float2(1280, 720);
	last_size = size;
	render_mode = CanvasRenderMode::RENDERMODE_SCREEN_SPACE;
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

CanvasRenderMode ComponentCanvas::GetRenderMode()
{
	return render_mode;
}

void ComponentCanvas::SetSize(const float2 & _size)
{
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
}

void CanvasDrawElement::SetSize(const float2& _size)
{
	size = _size;
}

void CanvasDrawElement::SetTransform(const float4x4& trans)
{
	transform = trans;
}

void CanvasDrawElement::SetTextureId(const uint& id)
{
	texture_id = id;
}

void CanvasDrawElement::SetColour(const float4 & _colour)
{
	colour = _colour;
}

float4x4 CanvasDrawElement::GetTransform() const
{
	float4x4 ret = float4x4::identity;

	float4x4 size_trans = ret.FromTRS(float3::zero, Quat::FromEulerXYZ(90, 0, 0), float3(size.x, 1, size.y));

	ret = size_trans * transform;

	return ret;;
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
