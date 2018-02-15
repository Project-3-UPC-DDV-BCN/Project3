#pragma once
#include "Module.h"
#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Primitive.h"
#include <map>
#include <string>
#include "Octree.h"
#include "imgui\ImGuizmo\ImGuizmo.h"

class GameObject;
class Texture;
class ComponentCamera;
class Mesh;
class Prefab;
class CubeMap;
class SkyDome;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleScene();

	void CreateMainCamera();

	bool Start();
	update_status Update(float dt);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* DuplicateGameObject(GameObject* gameObject);
	void RenameDuplicatedGameObject(GameObject * gameObject, bool justIncrease = false);
	GameObject* FindGameObject(uint id) const;

	void AddGameObjectToScene(GameObject* gameobject);
	void AddGameObjectToDestroy(GameObject* gameobject);
	void DestroyAllGameObjects();
	void RemoveWithoutDelete(GameObject* gameobject);

	void ApplyTextureToSelectedGameObjects(Texture* texture);
	void ApplyTextureToGameObject(GameObject* gameobject, Texture* texture);

	int GetNumCameras() const;

	void InsertGoInOctree(ComponentMeshRenderer* mesh);
	void EraseGoInOctree(ComponentMeshRenderer* mesh);
	void GetOctreeIntersects(std::list<ComponentMeshRenderer*>& list, AABB& box);

	void NewScene(bool loading_scene);
	void LoadScene(std::string path);
	void SaveScene(std::string path) const;

	void LoadPrefab(Prefab* prefab);
	void CreatePrefab(GameObject* gameobject);

	void DrawSkyBox(float3 pos);

	void InitScripts();

private:
	bool RecursiveCheckActiveParents(GameObject* gameobject);
	void HandleInput();

public:
	std::list<GameObject*> selected_gameobjects;
	std::list<GameObject*> root_gameobjects;
	std::list<GameObject*> scene_gameobjects;
	std::map<std::string, int> scene_gameobjects_name_counter;
	std::list<ComponentCamera*> scene_cameras;
	std::list<ComponentMeshRenderer*> static_meshes;
	std::list<ComponentMeshRenderer*> dynamic_meshes;
	Octree octree;
	bool draw_octree;
	int saving_index;
	std::string scene_name;

	GameObject* main_camera;

	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;

	Data* tmp_scene_data;

private:
	std::list<GameObject*> gameobjects_to_destroy;
	CubeMap* skybox;
	
};
