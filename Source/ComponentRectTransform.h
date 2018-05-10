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

	void SetPos(const float2& pos);
	void AddPos(const float2& add);
	float2 GetPos() const;
	float2 GetScaledPos();
	float3 GetLocalPos() const;
	float3 GetGlobalPos() const;

	void SetZPos(float set);
	float GetZPos();

	void SetRotation(const float3& rotation);
	float3 GetLocalRotation() const;

	void SetSize(float2 size, bool use_fixed_ratio = true);
	void SetLeftSize(const float& left);
	void SetRightSize(const float& right);
	void SetUpSize(const float& up);
	void SetDownSize(const float& down);
	float2 GetSize() const;
	float2 GetScaledSize() const;

	void SetFixedAspectRatio(bool set);
	bool GetFixedAspectRatio() const;

	void SetAspectRatio(float aspect_ratio);
	float GetAspectRatio() const;

	void SetAnchor(const float2& anchor);
	float2 GetAnchor() const;
	float3 GetAnchorLocalPos();
	float3 GetAnchorGlobalPos();
	float4x4 GetAnchorTransform();

	void SetScale(const float& scale);
	float GetScale() const;
	float2 GetScaleAxis() const;

	void SetSnapUp(bool set);
	void SetSnapDown(bool set);
	void SetSnapLeft(bool set);
	void SetSnapRight(bool set);

	bool GetSnapUp();
	bool GetSnapDown();
	bool GetSnapLeft();
	bool GetSnapRight();

	float3 GetPreferedPos();

	void SetInteractable(bool set);
	bool GetInteractable() const;

	void SetOnClick(bool set);
	void SetOnClickDown(bool set);
	void SetOnClickUp(bool set);
	void SetOnMouseEnter(bool set);
	void SetOnMouseOver(bool set);
	void SetOnMouseOut(bool set);

	bool GetOnClick();
	bool GetOnClickDown();
	bool GetOnClickUp();
	bool GetOnMouseEnter();
	bool GetOnMouseOver();
	bool GetOnMouseOut();

	void Save(Data& data) const;
	void Load(Data& data);

	void SetID(uint new_id);
	uint GetID() const;

	void SetControllerAdmision(bool new_id);
	bool GetControllerAdmision() const;

	void SetControllerOrder(int new_order);
	int GetControllerOrder() const;

	void SetUsesLight(bool set);
	bool GetUsesLight() const;

private:
	bool GetHasParent() const;
	bool GetHasParentInactive() const;

private:
	ComponentTransform* c_transform = nullptr;

	float2				pos;
	float				z_pos;
	float2				size;
	float2			    anchor;
	float				scale;

	bool				fixed_aspect_ratio;
	float				aspect_ratio;

	uint				id;
	int					controler_order; 

	bool			    snap_up;
	bool				snap_down;
	bool				snap_left;
	bool				snap_right;

	bool				interactable;
	bool				controller_admision; 

	bool				on_click;
	bool				on_click_down;
	bool				on_click_up;
	bool				on_mouse_enter;
	bool				on_mouse_over;
	bool				on_mouse_out;

	bool				uses_light;
};

#endif // !_H_COMPONENT_RECT_TRANSFORM__