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

	bool Update();

	ComponentTransform* GetCompTransform() const;
	ComponentTransform* GetParentCompTransform() const;
	ComponentRectTransform* GetParentCompRectTransform() const;
	float4x4 GetMatrix() const;
	float2 GetGlobalOrigin();
	float4x4 GetOriginMatrix() const;

	ComponentCanvas* GetCanvas(bool& go_is_canvas);

	void UpdateTransform();
	void UpdateTransformAndChilds();

	void UpdateRectTransform();
	void UpdateRectTransformAndChilds();

	void SetPos(const float2& size);
	float2 GetPos() const;
	float2 GetGlobalPos() const;
	float4x4 GetPositionTransform();

	void SetSize(const float2& size);
	float2 GetSize() const;

	void SetAnchor(const float2& anchor);
	float2 GetAnchor() const;
	float2 GetGlobalAnchor();
	float4x4 GetAnchorTransform();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	bool GetHasParent() const;

private:
	ComponentTransform* c_transform = nullptr;

	float2				pos;
	float2				size;
	float2			    anchor;
};

#endif // !_H_COMPONENT_RECT_TRANSFORM__