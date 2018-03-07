#include "ComponentRadar.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "UsefulFunctions.h"

ComponentRadar::ComponentRadar(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Radar");
	SetType(ComponentType::CompRadar);
	SetGameObject(attached_gameobject);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 100));

	c_rect_trans->SetFixedAspectRatio(true);

	background_texture = nullptr;
	center_go = nullptr;
	max_distance = 50;
}

ComponentRadar::~ComponentRadar()
{
}

bool ComponentRadar::Update()
{
	bool ret = true;

	ComponentCanvas* canvas = GetCanvas();

	if (canvas != nullptr)
	{
		CanvasDrawElement de(canvas, this);
		de.SetTransform(c_rect_trans->GetMatrix());
		de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
		de.SetSize(c_rect_trans->GetScaledSize());
		de.SetColour(float4(1.0f, 1.0f, 1.0f, 1.0f));
		de.SetFlip(false, false);

		if (background_texture != nullptr)
		{
			de.SetTextureId(background_texture->GetID());
		}

		canvas->AddDrawElement(de);
	}

	return ret;
}

void ComponentRadar::SetBackgroundTexture(Texture * tex)
{
	background_texture = tex;
}

Texture * ComponentRadar::GetBackgroundTexture() const
{
	return background_texture;
}

void ComponentRadar::SetCenter(GameObject * go)
{
	center_go = go;
}

GameObject * ComponentRadar::GetCenter() const
{
	return center_go;
}

void ComponentRadar::CreateMarker(const char * name, Texture * texture)
{
	bool exists = false;

	for (std::vector<RadarMarker>::iterator it = markers.begin(); it != markers.end(); ++it)
	{
		if (TextCmp(name, (*it).marker_name.c_str()))
		{
			exists = true;
			break;
		}
	}

	if (!exists)
	{
		RadarMarker marker;
		marker.marker_name = name;
		marker.marker_texture = texture;
		markers.push_back(marker);
	}
}

void ComponentRadar::DeleteMarker(const char * name)
{
	for (std::vector<RadarMarker>::iterator it = markers.begin(); it != markers.end(); ++it)
	{
		if (TextCmp(name, (*it).marker_name.c_str()))
		{
			markers.erase(it);
			break;
		}
	}
}

std::vector<RadarMarker> ComponentRadar::GetMarkers() const
{
	return markers;
}

void ComponentRadar::AddEntity(GameObject * go, RadarMarker marker)
{
	bool exists = false;

	for (std::vector<RadarEntity>::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		if (go == (*it).go)
		{
			exists = true;
			break;
		}
	}

	if (!exists)
	{
		RadarEntity entity;
		entity.go = go;
		entity.marker = marker;
		entities.push_back(entity);
	}
}

void ComponentRadar::RemoveEntity(GameObject * go)
{
	for (std::vector<RadarEntity>::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		if (go == (*it).go)
		{
			entities.erase(it);
			break;
		}
	}
}

std::vector<RadarEntity> ComponentRadar::GetEntities() const
{
	return entities;
}

void ComponentRadar::SetMaxDistance(float distance)
{
	max_distance = distance;

	if (max_distance < 0)
		max_distance = 0;
}

float ComponentRadar::GetMaxDistance() const
{
	return max_distance;
}

void ComponentRadar::Save(Data & data) const
{
}

void ComponentRadar::Load(Data & data)
{
}

ComponentCanvas * ComponentRadar::GetCanvas()
{
	ComponentCanvas* ret = nullptr;

	bool go_is_canvas;
	ret = GetRectTrans()->GetCanvas(go_is_canvas);

	return ret;
}

ComponentRectTransform * ComponentRadar::GetRectTrans()
{
	ComponentRectTransform* ret = nullptr;

	ret = (ComponentRectTransform*)GetGameObject()->GetComponent(Component::CompRectTransform);

	return ret;
}
