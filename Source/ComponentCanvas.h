#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include <vector>
#include <iostream>
#include "Component.h"

enum CanvasRenderMode
{
	RENDERMODE_SCREEN_SPACE,
	RENDERMODE_WORLD_SPACE,
};

class ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* attached_gameobject);
	virtual ~ComponentCanvas();

	bool Update();

	void SetRenderMode(CanvasRenderMode mode);
	CanvasRenderMode GetRenderMode();

	void SetSize(const float2& size);
	float2 GetSize() const;

	float4x4 GetOrigin();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	void UpdateRectTransforms();

private:
	float2 size;
	float2 last_size;
	CanvasRenderMode render_mode;
};

#endif // !_H_COMPONENT_CANVAS__