#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include <vector>
#include <iostream>
#include "Component.h"
#include "MathGeoLib\Math\float4x4.h"

class Mesh;
class ComponentRectTransform;
class ComponentCamera;

class CanvasDrawElement
{
public:
	CanvasDrawElement();

	void SetSize(float2 size);
	void SetTransform(float4x4 trans);
	void SetTextureId(uint id);

	float4x4 GetTransform();
	uint GetTextureId();

private:
	Mesh*    plane = nullptr;
	float2   size;
	float4x4 transform;
	uint	 texture_id;
};

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

	ComponentRectTransform* GetCompRectTransform() const;

	void SetRenderMode(CanvasRenderMode mode);
	CanvasRenderMode GetRenderMode();

	void SetSize(const float2& size);
	float2 GetSize() const;

	void AddDrawElement(CanvasDrawElement de);
	void ClearDrawElements();
	std::vector<CanvasDrawElement> GetDrawElements();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	void UpdateSize();

private:
	ComponentRectTransform* c_rect_trans = nullptr;

	ComponentCamera*			   render_camera = nullptr;
	std::vector<CanvasDrawElement> draws;

	float2 size;
	float2 last_size;
	CanvasRenderMode render_mode;
};

#endif // !_H_COMPONENT_CANVAS__