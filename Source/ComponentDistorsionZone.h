#ifndef _DISTORSIONZONE_H_
#define _DISTORSIONZONE_H_

#define DEFAULT_RADIUS 10

#include "Component.h"
#include "Geomath.h"

class ComponentDistorsionZone : 
	public Component 
{
public:

	ComponentDistorsionZone(GameObject* attached_gameobject);
	~ComponentDistorsionZone();
	bool Update();
	void DebugDraw();
	bool CheckCollision(AABB target);

	void Save(Data& data) const;
	void Load(Data& data);

public:

	std::string bus = "";
	float distorsion_value = 0.f;
	math::OBB zone;
	float radius;
};





#endif
