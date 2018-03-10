#include "ComponentRadar.h"
#include "GameObject.h"
#include "ComponentCanvas.h"
#include "ComponentRectTransform.h"
#include "Texture.h"
#include "UsefulFunctions.h"
#include "ComponentTransform.h"

ComponentRadar::ComponentRadar(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Radar");
	SetType(ComponentType::CompRadar);
	SetGameObject(attached_gameobject);

	c_rect_trans = GetRectTrans();

	c_rect_trans->SetSize(float2(100, 100));

	c_rect_trans->SetFixedAspectRatio(true);

	c_rect_trans->SetInteractable(false);

	background_texture = nullptr;
	transparent = false;
	center_go = nullptr;
	center_texture = nullptr;
	max_distance = 50;

	markers_size = 0.2f;
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
		de.SetFlip(false, false);

		if (!transparent)
		{
			de.SetColour(float4(1.0f, 1.0f, 1.0f, 1.0f));
			if (background_texture != nullptr)
			{
				de.SetTextureId(background_texture->GetID());
			}
		}
		else
		{
			de.SetColour(float4(1.0f, 1.0f, 1.0f, 0.0f));
		}

		canvas->AddDrawElement(de);

		ComponentTransform* center_trans = nullptr;
		if (center_go != nullptr)
		{
			center_trans = (ComponentTransform*)center_go->GetComponent(Component::CompTransform);

			CanvasDrawElement de(canvas, this);
			de.SetTransform(c_rect_trans->GetMatrix());
			de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
			de.SetSize(c_rect_trans->GetScaledSize() * markers_size);
			de.SetColour(float4(0.0f, 1.0f, 1.0f, 1.0f));
			de.SetFlip(false, false);

			if (center_texture != nullptr)
			{
				de.SetTextureId(center_texture->GetID());
			}

			canvas->AddDrawElement(de);

			for (std::vector<RadarEntity>::iterator it = entities.begin(); it != entities.end(); ++it)
			{
				if ((*it).go != nullptr)
				{
					ComponentTransform* entity_trans = (ComponentTransform*)(*it).go->GetComponent(Component::CompTransform);

					float3 center_rot = center_trans->GetGlobalRotation();
					float3 center_pos = center_trans->GetGlobalPosition();
					float3 entity_pos = entity_trans->GetGlobalPosition();

					float3 distance = center_pos - entity_pos;
					float distance_mag = center_pos.Distance(entity_pos);
					float angle_x_z = AngleFromTwoPoints(center_pos.x, center_pos.z, entity_pos.x, entity_pos.z);
					angle_x_z += center_rot.y;

					float scaled_size = c_rect_trans->GetScaledSize().x;

					// if max_distance -> scaled_size
					// distance = -> ?

					if (max_distance > 0)
					{
						float scaled_distance_x = (scaled_size * distance.x) / max_distance;
						float scaled_distance_y = (scaled_size * distance.y) / max_distance;
						float scaled_distance_z = (scaled_size * distance.z) / max_distance;
						
						float scaled_distance_mag = (scaled_size * distance_mag) / max_distance;

						CanvasDrawElement de(canvas, this);
						de.SetTransform(c_rect_trans->GetMatrix());
						de.SetOrtoTransform(c_rect_trans->GetOrtoMatrix());
						de.SetSize(c_rect_trans->GetScaledSize() * markers_size);
						de.SetColour(float4(1.0f, 0.0f, 1.0f, 1.0f));
						de.SetFlip(false, false);
						de.SetPosition(float3(-scaled_distance_mag * cos((DEGTORAD*angle_x_z)), scaled_distance_mag * sin(DEGTORAD*angle_x_z), -scaled_distance_y));

						if ((*it).marker != nullptr)
						{
							if((*it).marker->marker_texture != nullptr)
								de.SetTextureId((*it).marker->marker_texture->GetID());
						}

						canvas->AddDrawElement(de);
					}

				}
			}
		}
	}

	return ret;
}

bool ComponentRadar::CleanUp()
{
	for (std::vector<RadarMarker*>::iterator it = markers.begin(); it != markers.end();)
	{
		RELEASE(*it);
		it = markers.erase(it);
	}

	return false;
}

void ComponentRadar::SetBackgroundTexture(Texture * tex)
{
	background_texture = tex;
}

Texture * ComponentRadar::GetBackgroundTexture() const
{
	return background_texture;
}

void ComponentRadar::SetTransparent(bool _transparent)
{
	transparent = _transparent;
}

bool ComponentRadar::GetTransparent() const
{
	return transparent;
}

void ComponentRadar::SetCenter(GameObject * go)
{
	center_go = go;
}

GameObject * ComponentRadar::GetCenter() const
{
	return center_go;
}

void ComponentRadar::SetCenterTexture(Texture * tex)
{
	center_texture = tex;
}

Texture * ComponentRadar::GetCenterTexture() const
{
	return center_texture;
}

void ComponentRadar::CreateMarker(const char * name, Texture * texture)
{
	bool exists = false;

	for (std::vector<RadarMarker*>::iterator it = markers.begin(); it != markers.end(); ++it)
	{
		if (TextCmp(name, (*it)->marker_name.c_str()))
		{
			exists = true;
			break;
		}
	}

	if (!exists)
	{
		RadarMarker* marker = new RadarMarker();
		marker->marker_name = name;
		marker->marker_texture = texture;
		markers.push_back(marker);
	}
}

void ComponentRadar::DeleteMarker(RadarMarker* marker)
{
	for (std::vector<RadarMarker*>::iterator it = markers.begin(); it != markers.end(); ++it)
	{
		if (marker == (*it))
		{
			CleanEntitiesWithMarker((*it));
			RELEASE(*it);
			markers.erase(it);
			break;
		}
	}
}

RadarMarker * ComponentRadar::GetMarker(const char * name)
{
	RadarMarker* ret = nullptr;

	for (std::vector<RadarMarker*>::iterator it = markers.begin(); it != markers.end(); ++it)
	{
		if (TextCmp(name, (*it)->marker_name.c_str()))
		{
			ret = (*it);
			break;
		}
	}

	return ret;
}

void ComponentRadar::AddEntity(GameObject * go)
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
		entity.marker = nullptr;
		entities.push_back(entity);
	}
}

void ComponentRadar::AddMarkerToEntity(RadarEntity entity, RadarMarker* marker)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].go == entity.go)
		{
			entities[i].marker = marker;
			break;
		}
	}
}

void ComponentRadar::AddMarkerToEntity(GameObject * entity, RadarMarker * marker)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].go == entity)
		{
			entities[i].marker = marker;
			break;
		}
	}
}

void ComponentRadar::AddMarkerToEntity(int entity_index, RadarMarker * marker)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (i == entity_index)
		{
			entities[i].marker = marker;
			break;
		}
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

void ComponentRadar::RemoveAllEntities()
{
	entities.clear();
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

void ComponentRadar::SetMarkersSize(float size)
{
	markers_size = size;

	if (markers_size < 0)
		markers_size = 0;
}

float ComponentRadar::GetMarkersSize() const
{
	return markers_size;
}

void ComponentRadar::Save(Data & data) const
{
	data.AddFloat("markers_size", markers_size);
	data.AddFloat("max_distance", max_distance);
	data.AddBool("transparent", transparent);
	if (background_texture != nullptr)
		data.AddString("background_texture", background_texture->GetName().c_str());
	if (center_go != nullptr)
		data.AddUInt("center_go", center_go->GetUID());
	if(center_texture != nullptr)
		data.AddString("center_texture", center_texture->GetName().c_str());


//	std::vector<RadarMarker*> markers;
//	std::vector<RadarEntity> entities;
//
//private:
//	ComponentRectTransform * c_rect_trans = nullptr;
//
//	Texture* background_texture;
//	bool transparent;
//
//	GameObject* center_go;
//	Texture* center_texture;
//
//	float max_distance;
//
//	float markers_size;
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

void ComponentRadar::CleanEntitiesWithMarker(RadarMarker * marker)
{
	for (std::vector<RadarEntity>::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		if ((*it).marker == marker)
		{
			(*it).marker = nullptr;
		}
	}
}
