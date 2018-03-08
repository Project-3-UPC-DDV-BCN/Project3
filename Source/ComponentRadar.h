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
	Texture* marker_texture = nullptr;
	std::string marker_name;
};

struct RadarEntity
{
	GameObject* go = nullptr;
	RadarMarker marker;
	bool has_marker = false;
};

class ComponentRadar: public Component
{
public:
	ComponentRadar(GameObject* attached_gameobject);
	virtual ~ComponentRadar();

	bool Update();

	void SetBackgroundTexture(Texture* tex);
	Texture* GetBackgroundTexture() const;

	void SetCenter(GameObject* go);
	GameObject* GetCenter() const;

	void CreateMarker(const char* name, Texture* texture);
	void DeleteMarker(const char* name);

	void AddEntity(GameObject* go);
	void AddMarkerToEntity(int entity_index, RadarMarker marker);

	void RemoveEntity(GameObject* go);

	void SetMaxDistance(float distance);
	float GetMaxDistance() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();

public:
	std::vector<RadarMarker> markers;
	std::vector<RadarEntity> entities;

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	Texture* background_texture;

	GameObject* center_go;

	float max_distance;
};

#endif // !_H_COMPONENT_RADAR__