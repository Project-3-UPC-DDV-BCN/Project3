#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleEditor.h"
#include "MathGeoLib\MathGeoLib.h"

class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleCamera3D();

	bool Init(Data* editor_config);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateEditorCamera();

	void LookAt(const float3 &spot);
	void OrbitAt(const float3 &spot);
	void FocusOnObject(AABB& box);
	float* GetViewMatrix();
	void SetOrbital(bool is_orbital);
	bool IsOrbital() const;
	math::float3 GetPosition() const;
	void SetPosition(math::float3 position);
	ComponentCamera* GetCamera() const;
	void SetCameraSensitivity(float sensivity);
	float GetCameraSensitivity() const;
	void MousePickRay(int mouse_x, int mouse_y);
	void SaveData(Data* data);

	LineSegment GetUIMouseRay(ComponentCanvas* cv);

private:
	bool isPlaying = false;
	bool camera_is_orbital;
	ComponentCamera* editor_camera;
	float camera_sensitivity;

public:
	bool can_update;

	int key_speed;
	int key_forward;
	int key_backward;
	int key_up;
	int key_down;
	int key_left;
	int key_right;
};