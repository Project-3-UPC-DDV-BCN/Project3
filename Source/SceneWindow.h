#pragma once

#include "Window.h"
#include "MathGeoLib/float3.h"

class SceneWindow :
	public Window
{
public:
	SceneWindow();
	virtual ~SceneWindow();

	void DrawWindow();
	void DrawMenuBar();
	bool IsWindowFocused() const;
	bool IsMouseHoveringWindow() const;
	ImVec2 GetWindowSize() const;
	ImVec2 GetWindowPos() const;
private:
	float scene_width;
	float scene_height;
	bool wireframe_mode;
	bool is_window_focused;
	bool is_mouse_hovering_window;
	ImVec2 window_size;
	ImVec2 window_pos;

	float3 last_used_scale;
};

