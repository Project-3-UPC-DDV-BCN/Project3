#pragma once
#include "ComponentCanvas.h"
#include "ComponentCamera.h"


class Texture;

class LensFlare {
public:
	LensFlare();
	virtual ~LensFlare();

	bool SetActiveCamera(ComponentCamera* active_camera);


private: 

	Texture*	test_tex = nullptr;
	bool		flip = false;
	float2		position = float2::zero;
	float2		size = float2::one;
	float		scale = 5.0;

	float4x4	transform = float4x4::identity;
	float4x4	orto_transform = float4x4::identity;

	// Artificial Canvas
	float4x4	canvas_transform = float4x4::identity;
};