#pragma once
#include "Module.h"
#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Primitive.h"
#include "ComponentParticleEmmiter.h"
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
class BlastModel;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleScene();

	void CreateMainCamera();
	void CreateMainLight();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	update_status Update(float dt);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void SaveData(Data* data);

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateLightObject(GameObject* parent = nullptr);
	GameObject* DuplicateGameObject(GameObject* gameObject);
	void RenameDuplicatedGameObject(GameObject * gameObject, bool justIncrease = false);
	GameObject* FindGameObject(uint id) const;
	GameObject* FindGameObjectByName(std::string name) const;

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
	void LoadSceneIntoCurrent(std::string path);
	void SaveScene(std::string path);

	bool LoadPrefab(std::string path, std::string extension, Data& data, bool destroy_scene = false, bool duplicate = false, std::list<GameObject*>& new_gos = std::list<GameObject*>());
	void SavePrefab(std::list<GameObject*> gos, std::string path, std::string extension, Data data);

	void LoadPrefabToScene(Prefab* prefab);
	void CreatePrefab(GameObject* gameobject, std::string assets_path, std::string library_path);

	void LoadBlastModel(BlastModel* model);

	void DrawSkyBox(float3 pos, ComponentCamera* active_camera);

	void InitScripts();

	// UI
	GameObject* CreateCanvas(GameObject* parent = nullptr);
	GameObject* CreateImage(GameObject* parent = nullptr);
	GameObject* CreateText(GameObject* parent = nullptr);
	GameObject* CreateProgressBar(GameObject* parent = nullptr);
	GameObject* CreateButton(GameObject* parent = nullptr);
	GameObject* CreateRadar(GameObject* parent = nullptr);

	void SetParticleSystemsState();

private:
	bool RecursiveCheckActiveParents(GameObject* gameobject);
	void HandleInput();

	void LoadSceneNow();
	void DestroyGameObjectNow();

public:
	std::list<GameObject*> selected_gameobjects;
	std::list<GameObject*> root_gameobjects;
	std::list<GameObject*> scene_gameobjects;
	std::map<std::string, int> scene_gameobjects_name_counter;
	std::list<ComponentCamera*> scene_cameras;
	std::list<ComponentParticleEmmiter*> scene_emmiters; 
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

	int triangles_count;
	float skybox_size = 100.f;

private:
	std::list<GameObject*> gameobjects_to_destroy;
	CubeMap* skybox;

	bool to_load_scene = false;
	std::string scene_to_load;
	bool destroy_current = false;
	std::string current_scene_path;
};
