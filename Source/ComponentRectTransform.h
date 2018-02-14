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
	ComponentCanvas* GetCanvas(bool& go_is_canvas);
	ComponentCanvas* GetCanvas();

	void UpdateTransform();
	void UpdateTransformAndChilds();

	void UpdateRectTransform();
	void UpdateRectTransformAndChilds();

	float4x4 GetMatrix() const;
	float4x4 GetOrtoMatrix();

	float3 GetOriginLocalPos();
	float3 GetOriginGlobalPos();
	float4x4 GetOriginMatrix() const;

	void SetPos(const float2& size);
	float2 GetPos() const;
	float2 GetScaledPos();
	float3 GetLocalPos() const;
	float3 GetGlobalPos() const;

	void SetRotation(const float3& rotation);
	float3 GetLocalRotation() const;

	void SetSize(const float2& size);
	float2 GetSize() const;
	float2 GetScaledSize() const;

	void SetAnchor(const float2& anchor);
	float2 GetAnchor() const;
	float3 GetAnchorLocalPos();
	float3 GetAnchorGlobalPos();
	float4x4 GetAnchorTransform();

	void SetScale(const float& scale);
	float GetScale() const;

	void SetSnapUp(bool set);
	void SetSnapDown(bool set);
	void SetSnapLeft(bool set);
	void SetSnapRight(bool set);

	bool GetSnapUp();
	bool GetSnapDown();
	bool GetSnapLeft();
	bool GetSnapRight();

	float3 GetPreferedPos();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	bool GetHasParent() const;

private:
	ComponentTransform* c_transform = nullptr;

	float2				pos;
	float2				size;
	float2			    anchor;
	float3				rotation;
	float				scale;

	bool			    snap_up;
	bool				snap_down;
	bool				snap_left;
	bool				snap_right;
};

#endif // !_H_COMPONENT_RECT_TRANSFORM__