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
	std::vector<RadarMarker> GetMarkers() const;

	void AddEntity(GameObject* go, RadarMarker marker);
	void RemoveEntity(GameObject* go);
	std::vector<RadarEntity> GetEntities() const;

	void SetMaxDistance(float distance);
	float GetMaxDistance() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	ComponentCanvas * GetCanvas();
	ComponentRectTransform * GetRectTrans();

private:
	ComponentRectTransform * c_rect_trans = nullptr;

	Texture* background_texture;

	GameObject* center_go;
	std::vector<RadarMarker> markers;
	std::vector<RadarEntity> entities;

	float max_distance;
};

#endif // !_H_COMPONENT_RADAR__