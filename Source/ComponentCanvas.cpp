#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentRectTransform.h"
#include "ComponentTransform.h"
#include "GameWindow.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"

ComponentCanvas::ComponentCanvas(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Canvas");
	SetType(ComponentType::CompCanvas);
	SetGameObject(attached_gameobject);

	size = float2(1080, 720);
	last_size = size;
	render_mode = CanvasRenderMode::RENDERMODE_SCREEN_SPACE;
}

ComponentCanvas::~ComponentCanvas()
{
}

bool ComponentCanvas::Update()
{
	bool ret = true;

	UpdateSize();

	return ret;
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
		UpdateRectTransforms();

		last_size = size;
	}
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

void ComponentCanvas::AddDrawElement(CanvasDrawElement de)
{
	draws.push_back(de);
}

std::vector<CanvasDrawElement> ComponentCanvas::GetDrawElements()
{
	return draws;
}

void ComponentCanvas::ClearDrawElements()
{
	draws.clear();
}


void ComponentCanvas::Save(Data & data) const
{
}

void ComponentCanvas::Load(Data & data)
{
}

void ComponentCanvas::UpdateRectTransforms()
{
	//std::vector<GameObject*> to_check;
	//to_check.push_back(GetGameObject());

	//// Iterate through all childs and get UI childs
	//while (!to_check.empty())
	//{
	//	GameObject* check = (*to_check.begin());
	//	to_check.erase(to_check.begin());

	//	for (std::list<GameObject*>::iterator it = check->childs.begin(); it != check->childs.end(); ++it)
	//	{
	//		if ((*it)->GetIsUI())
	//		{
	//			ComponentRectTransform* c_rect_tran = (ComponentRectTransform*)(*it)->GetComponent(Component::CompRectTransform);

	//			if (c_rect_tran != nullptr)
	//			{
	//				c_rect_tran->UpdateTransform();
	//			}
	//		}

	//		to_check.push_back(*it);
	//	}
	//}
}

CanvasDrawElement::CanvasDrawElement()
{
	plane = App->resources->GetMesh("PrimitivePlane");
	texture_id = 0;
	transform = float4x4::identity;
}

void CanvasDrawElement::SetTransform(float4x4 trans)
{
	transform = trans;
}

void CanvasDrawElement::SetTextureId(uint id)
{
	texture_id = id;
}

float4x4 CanvasDrawElement::GetTransform()
{
	return transform;
}
