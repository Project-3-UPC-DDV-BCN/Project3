#ifndef _H_COMPONENT_RECT_TRANSFORM__
#define _H_COMPONENT_RECT_TRANSFORM__

#include "Component.h"

class ComponentTransform;

class ComponentRectTransfrom : public Component
{
public:
	ComponentRectTransfrom(GameObject* attached_gameobject);
	virtual ~ComponentRectTransfrom();

	bool Update();

	void SetRect(const float4& size);
	float4 GetRect() const;

	void SetAnchor(const float2& anchor);
	float2 GetAnchor() const;

	void SetRotation(float3 rotation);
	float3 GetGlobalRotation() const;
	float3 GetLocalRotation() const;
	void SetScale(float3 scale);
	float3 GetGlobalScale() const;
	float3 GetLocalScale() const;
	void UpdateGlobalMatrix();
	const float4x4 GetMatrix() const;
	const float* GetOpenGLMatrix() const;
	void SetMatrix(const float4x4 &matrix);

private:
	float4 rect;
	float2 anchor;
	ComponentTransform* c_transform = nullptr;
};

#endif // !_H_COMPONENT_RECT_TRANSFORM__