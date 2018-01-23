#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include "Component.h"

class ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* attached_gameobject);
	virtual ~ComponentCanvas();

	float2 GetSize();
	float4x4 GetOrigin();

	bool Update();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	float2 size;
};

#endif // !_H_COMPONENT_CANVAS__