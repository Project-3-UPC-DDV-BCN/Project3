#pragma once
#include "Component.h"
#include "MathGeoLib/Frustum.h"
#include "MathGeoLib/AABB.h"
#include "MathGeoLib/Rect.h"
#include "Color.h"
#include "Primitive.h"

class RenderTextureMSAA;

//MathGeoLib frustum info: http://clb.demon.fi/MathGeoLib/nightly/docs/Frustum_summary.php

class ComponentCamera :
	public Component
{
public:
	ComponentCamera(GameObject* attached_gameobject);
	~ComponentCamera();

	math::Frustum GetFrustum() const;
	bool ContainsGameObjectAABB(AABB& gameobject_bounding_box);
	bool IsPointInside(float3 world_position);
	void UpdatePosition();
	float* GetProjectionMatrix() const;
	float* GetViewMatrix();

	void SetAsMainCamera();
	void SetFOV(float fov);
	float GetFOV() const;
	void AddLayerToDraw(std::string layer);
	std::string GetLayerToDraw(int index) const;
	std::vector<std::string> GetAllLayersToDraw() const;
	void RemoveLayerToDraw(std::string layer);
	void SetBackgroundColor(Color color);
	Color GetBackgroundColor() const;
	void SetNearPlaneDistance(float distance);
	float GetNearPlaneDistance() const;
	void SetFarPlaneDistance(float distance);
	float GetFarPlanceDistance() const;
	void SetAspectRatio(float ratio);
	float GetAspectRatio() const;
	void SetViewport(Rect viewport);
	Rect GetViewport() const;
	void SetRenderOrder(int position);
	int GetRenderOrder() const;
	void SetTargetTexture(RenderTextureMSAA* texture);
	void SetViewPortTexture(RenderTextureMSAA* texture);
	RenderTextureMSAA* GetTargetTexture() const;
	RenderTextureMSAA* GetViewportTexture() const;

	void Save(Data& data) const;
	void Load(Data& data);

public:
	std::vector<std::string> layers_to_draw;
	Frustum camera_frustum;

private:
	RenderTextureMSAA* camera_viewport_texture;
	RenderTextureMSAA* camera_target_texture;
	
	Color background_color;
	Rect camera_viewport;
	int render_order;
	float aspect_ratio;
	float current_fov;
};

