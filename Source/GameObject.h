#pragma once

#include "Data.h"
#include "Component.h"
#include <list>
#include <string>
#include "MathGeoLib\Math\float4x4.h"

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

	int GetAllChildsCount() const;
	void GetAllChildsName(std::vector<std::string>& names);
	void GetAllChildsMeshesNames(std::vector<std::string>& names);

	void UpdateBoundingBox();
	math::float4x4 GetGlobalTransfomMatrix();
	const float* GetOpenGLMatrix();
	void UpdateGlobalMatrix();
	void UpdateCamera();
	void SetGlobalTransfomMatrix(const float4x4 &matrix);

	void SetParentByID(UID parent_id);

	void InitScripts();
	void StartScripts();
	void UpdateScripts();

	void UpdateFactory();

	// Sets element as UI and can be used by a component canvas
	void SetIsUI(bool set);

	// Returns true if the GameObject it's a UI element
	bool GetIsUI() const;

	bool GetIsCanvas() const;

	void Destroy();
	void OnDestroy();

	UID GetUID() const;

	void Save(Data& data, bool isDuplicated = false);
	void Load(Data & data, bool isPrefab = false);

	bool Update();

	void DeleteFromResourcesDestructor();

public:
	std::list<GameObject*> childs;
	std::list<Component*> components_list;
	bool is_on_destroy;
	bool used_in_scene;

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

