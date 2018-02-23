#ifndef _H_COMPONENT_CANVAS__
#define _H_COMPONENT_CANVAS__

#include <vector>
#include <iostream>
#include "Component.h"
#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Geometry\AABB.h"

class Mesh;
class ComponentRectTransform;
class ComponentCamera;

class CanvasDrawElement
{
public:
	CanvasDrawElement();

	void SetPosition(const float2& pos);
	void SetSize(const float2& size);
	void SetTransform(const float4x4& trans);
	void SetOrtoTransform(const float4x4& trans);
	void SetTextureId(const uint& id);
	void SetColour(const float4& colour);
	void SetFlip(const bool& vertical_flip, const bool& horizontal_flip);

	float4x4 GetTransform();
	float4x4 GetOrtoTransform() const;
	uint GetTextureId() const;
	float4 GetColour() const;
	Mesh* GetPlane() const;
	AABB GetBBox();

private:
	Mesh*    plane = nullptr;
	float2   pos;
	float2   size;
	float4x4 transform;
	float4x4 orto_transform;
	uint	 texture_id;
	bool	 vertical_flip;
	bool	 horizontal_flip;
	float4   colour;
};

enum CanvasRenderMode
{
	RENDERMODE_SCREEN_SPACE,
	RENDERMODE_WORLD_SPACE,
};

enum CanvasScaleMode
{
	SCALEMODE_CONSTANT_SIZE,
	SCALEMODE_WITH_SCREEN_SIZE,
};

class ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* attached_gameobject);
	virtual ~ComponentCanvas();

	bool Update();
	bool CleanUp();

	ComponentRectTransform* GetCompRectTransform() const;

	void SetRenderCamera(ComponentCamera* render_camera);
	ComponentCamera* GetRenderCamera() const;

	void SetRenderMode(CanvasRenderMode mode);
	CanvasRenderMode GetRenderMode() const;

	void SetScaleMode(CanvasScaleMode scale_mode);
	CanvasScaleMode GetScaleMode() const;

	void SetSize(const float2& size);
	float2 GetSize() const;

	void SetReferenceSize(const float2& size);
	float2 GetReferenceSize() const;

	void SetScale(const float& scale);
	float GetScale() const;

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
	float2 reference_size;

	CanvasRenderMode render_mode;
	CanvasScaleMode scale_mode;

	float scale;
};

#endif // !_H_COMPONENT_CANVAS__