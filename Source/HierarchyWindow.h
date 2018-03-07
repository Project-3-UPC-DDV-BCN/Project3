#pragma once

#include "Window.h"

class GameObject;

class HierarchyWindow :
	public Window
{
public:
	HierarchyWindow();
	virtual ~HierarchyWindow();

	void DrawWindow();
	void DrawSceneGameObjects(GameObject* gameObject);
	void IsMouseOver(GameObject* gameObject);

private:
	void CreatePrefabWindow();

private:
	bool show_rename_window;
	char node_name[30];
	bool show_rename_error;
	GameObject* open_gameobject_node;
	GameObject* gameobject_to_rename;
	float rename_window_y;
	bool show_create_prefab_window;
	GameObject* prefab_to_create;
	std::string prefab_assets_path_to_create;
	std::string prefab_library_path_to_create;
};

