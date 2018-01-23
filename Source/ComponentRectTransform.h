#ifndef _H_COMPONENT_RECT_TRANSFORM__
#define _H_COMPONENT_RECT_TRANSFORM__

#include "Component.h"

// TODO: REMEMBER TO SET IS UI TO FALSE WHEN DESTROYING ALL UI COMPONENTS OF A GO

class ComponentTransform;
class ComponentCanvas;

class ComponentRectTransform : public Component
{
public:
	ComponentRectTransform(GameObject* attached_gameobject);
	virtual ~ComponentRectTransform();

	void UpdateRectTransform();

	bool Update();

	void SetPos(const float2& size);
	float2 GetPos() const;
	void SetSize(const float2& size);
	float2 GetSize() const;
	void SetAnchor(const float2& anchor);
	float2 GetAnchor() const;
	float4x4 GetAnchorTransform() const;

	bool GetHasCanvas() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	void LookForCanvas();
	void UpdateTransform();

private:
	float2				pos;
	float2				size;
	float2			    anchor;
	ComponentTransform* c_transform = nullptr;
	ComponentCanvas*    c_canvas = nullptr;
};

#endif // !_H_COMPONENT_RECT_TRANSFORM__