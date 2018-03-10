#ifndef _H_COMPONENT_RADAR__
#define _H_COMPONENT_RADAR__

#include "Component.h"
#include <vector>
#include "Timer.h"

class ComponentRectTransform;
class ComponentTransform;
class ComponentCanvas;
class CanvasDrawElement;
class Texture;
class Timer;

struct RadarMarker
{
	RadarMarker() {};
	Texture* marker_texture = nullptr;
	std::string marker_name;
};

struct RadarEntity
{
	GameObject* go = nullptr;
	RadarMarker* marker = nullptr;
};

class ComponentRadar: public Component
{
public:
	ComponentRadar(GameObject* attached_gameobject);
	virtual ~ComponentRadar();

	bool Update();
	bool CleanUp();

	void SetBackgroundTexture(Texture* tex);
	Texture* GetBackgroundTexture() const;

	void SetTransparent(bool transparent);
	bool GetTransparent() const;

	void SetCenter(GameObject* go);
	GameObject* GetCenter() const;

	void SetCenterTexture(Texture* tex);
	Texture* GetCenterTexture() const;

	void CreateMarker(const char* name, Texture* texture);
	void DeleteMarker(RadarMarker* marker);
	RadarMarker* GetMarker(const char* name);

	void AddEntity(GameObject* go);
	void AddMarkerToEntity(RadarEntity entity, RadarMarker* marker);
	void AddMarkerToEntity(GameObject* entity, RadarMarker* marker);
	void AddMarkerToEntity(int entity_index, RadarMarker* marker);

	void RemoveEntity(GameObject* go);
	void RemoveAllEntities();

	void SetMaxDistance(float distance);
	float GetMaxDistance() const;

	void SetMarkersSize(float size);
	float GetMarkersSize() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();
	void CleanEntitiesWithMarker(RadarMarker* marker);

public:
	std::vector<RadarMarker*> markers;
	std::vector<RadarEntity> entities;

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	Texture* background_texture;
	bool transparent;

	GameObject* center_go;
	Texture* center_texture;

	float max_distance;

	float markers_size;
};

#endif // !_H_COMPONENT_RADAR__