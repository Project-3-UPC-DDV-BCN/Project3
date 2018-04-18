#pragma once

#include "Data.h"
#include "Component.h"
#include <list>
#include <string>
#include "MathGeoLib/float4x4.h"

struct CollisionData;

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	Component* AddComponent(Component::ComponentType component_type);
	Component* GetComponent(Component::ComponentType component_type);
	Component* GetComponent(std::string component_type);
	void DestroyComponent(Component* component);

	void AddChild(GameObject* gameobject);
	bool IsActive() const;
	void SetActive(bool active);
	void SetStatic(bool is_static);
	bool IsStatic() const;
	bool IsSelected() const;
	void SetSelected(bool selected);
	bool IsRoot() const;
	void SetRoot(bool root);
	GameObject* GetParent() const;
	void SetParent(GameObject* parent);
	void SetName(std::string name);
	std::string GetName() const;
	void SetTag(std::string tag);
	std::string GetTag() const;
	void SetLayer(std::string layer);
	std::string GetLayer() const;
	void SetIsUsedInPrefab(bool used);
	bool GetIsUsedInPrefab()const;
	void SetNewUID();
	void SetUID(uint uid);

	int GetAllChildsCount() const;
	void GetAllChildsName(std::vector<std::string>& names);
	void GetAllChildsMeshesNames(std::vector<std::string>& names);
	void GetAllChilds(std::vector<GameObject*>& childs_list) const;

	void UpdateBoundingBox();
	math::float4x4 GetGlobalTransfomMatrix();
	math::float4x4 GetOpenGLMatrix();
	void UpdateGlobalMatrix();
	void UpdateCamera();
	void SetGlobalTransfomMatrix(const float4x4 &matrix);
	math::float3 GetGlobalPosition();

	void SetParentByID(UID parent_id);

	void InitScripts();
	void StartScripts();
	void UpdateScripts();
	void OnCollisionEnter(CollisionData& col_data);
	void OnCollisionStay(CollisionData& col_data);
	void OnCollisionExit(CollisionData& col_data);
	void OnTriggerEnter(CollisionData& col_data);
	void OnTriggerStay(CollisionData& col_data);
	void OnTriggerExit(CollisionData& col_data);

	void UpdateFactory();

	// Sets element as UI and can be used by a component canvas
	void SetIsUI(bool set);

	// Returns true if the GameObject it's a UI element
	bool GetIsUI() const;

	bool GetIsCanvas() const;

	void Destroy();
	void OnDestroy();

	UID GetUID() const;

	void Save(Data& data, bool save_children = true);
	void Load(Data & data);

	bool Update();

	void DeleteFromResourcesDestructor();

public:
	std::list<GameObject*> childs;
	std::list<Component*> components_list;
	bool is_on_destroy;
	bool used_in_scene;
	bool was_active;

private:
	bool active;
	GameObject* parent;
	std::string name;
	std::string tag;
	std::string layer;
	bool is_root;
	UID uuid;
	bool is_selected;
	bool is_static;
	bool is_used_in_prefab;
	bool is_ui;
	bool is_canvas;
};

