#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentTransform.h"
#include "ComponentParticleEmmiter.h"
#include "ComponentBillboard.h"
#include "ComponentCamera.h"
#include "Mesh.h"
#include "ModuleResources.h"
#include "ComponentScript.h"
#include "ComponentFactory.h"
#include "ComponentRectTransform.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "ComponentLight.h"
#include "ComponentProgressBar.h"
#include "ComponentRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentJointDistance.h"
#include "ComponentAudioSource.h"
#include "ComponentListener.h"
#include "ComponentDistorsionZone.h"
#include "ComponentLight.h"
#include "ComponentBlast.h"
#include "ComponentGOAPAgent.h"
#include "ModulePhysics.h"
#include "ComponentButton.h"
#include "ComponentRadar.h"

GameObject::GameObject(GameObject* parent)
{
	active = true;
	this->parent = nullptr;
	if (parent != nullptr)
	{
		SetParent(parent);
	}
	else
	{
		is_root = true;
	}
	tag = "Default";
	layer = "Default";
	name = "New GameObject";
	is_on_destroy = false;
	is_selected = false;
	is_static = false;
	is_used_in_prefab = false;
	is_ui = false;
	AddComponent(Component::CompTransform);
	used_in_scene = false;
	uuid = App->RandomNumber().Int();
}

GameObject::~GameObject()
{
	App->resources->RemoveGameObject(this);
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); ++it) {
		(*it)->CleanUp();
		RELEASE(*it);
	}

	for (std::list<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		RELEASE(*it);
	}
}

Component * GameObject::AddComponent(Component::ComponentType component_type)
{
	Component* component = nullptr;

	switch (component_type)
	{
	case Component::CompTransform:
		components_list.push_back(component = new ComponentTransform(this));
		break;
	case Component::CompCamera:
		components_list.push_back(component = new ComponentCamera(this));
		break;
	case Component::CompRigidBody:
		components_list.push_back(component = new ComponentRigidBody(this));
		break;
	case Component::CompMeshRenderer:
		components_list.push_back(component = new ComponentMeshRenderer(this));
		break;
	case Component::CompBoxCollider:
		components_list.push_back(component = new ComponentCollider(this, ComponentCollider::BoxCollider));
		break;
	case Component::CompSphereCollider:
		components_list.push_back(component = new ComponentCollider(this, ComponentCollider::SphereCollider));
		break;
	case Component::CompCapsuleCollider:
		components_list.push_back(component = new ComponentCollider(this, ComponentCollider::CapsuleCollider));
		break;
	case Component::CompMeshCollider:
		components_list.push_back(component = new ComponentCollider(this, ComponentCollider::MeshCollider));
		break;
	case Component::CompAnimaton:
		break;
	case Component::CompScript:
		components_list.push_back(component = new ComponentScript(this));
		break;
	case Component::CompParticleSystem:
		components_list.push_back(component = new ComponentParticleEmmiter(this));
		break;
	case Component::CompBillboard:
		components_list.push_back(component = new ComponentBillboard(this));
		break;
	case Component::CompFactory:
		components_list.push_back(component = new ComponentFactory(this));
		break;
	case Component::CompRectTransform:
		if (GetComponent(Component::CompRectTransform) == nullptr)
			components_list.push_front(component = new ComponentRectTransform(this));
		break;
	case Component::CompCanvas:
		SetIsUI(true);
		if (GetComponent(Component::CompCanvas) == nullptr)
		{
			components_list.push_back(component = new ComponentCanvas(this));
			is_canvas = true;
			SetName("Canvas");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompImage:
		SetIsUI(true);
		if (GetComponent(Component::CompImage) == nullptr)
		{
			components_list.push_back(component = new ComponentImage(this));
			SetName("Image");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompText:
		SetIsUI(true);
		if (GetComponent(Component::CompText) == nullptr)
		{
			components_list.push_back(component = new ComponentText(this));
			SetName("Text");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompProgressBar:
		SetIsUI(true);
		if (GetComponent(Component::CompProgressBar) == nullptr)
		{
			components_list.push_back(component = new ComponentProgressBar(this));
			SetName("ProgressBar");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompButton:
		SetIsUI(true);
		if (GetComponent(Component::CompButton) == nullptr)
		{
			components_list.push_back(component = new ComponentButton(this));
			SetName("Button");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompRadar:
		SetIsUI(true);
		if (GetComponent(Component::CompRadar) == nullptr)
		{
			components_list.push_back(component = new ComponentRadar(this));
			SetName("Radar");
			App->scene->RenameDuplicatedGameObject(this);
		}
		break;
	case Component::CompDistanceJoint:
		components_list.push_back(component = new ComponentJointDistance(this));
	case Component::CompAudioSource:
		components_list.push_back(component = new ComponentAudioSource(this));
		break;
	case Component::CompAudioListener:
		components_list.push_back(component = new ComponentListener(this));
		break;
	case Component::CompAudioDistZone:
		components_list.push_back(component = new ComponentDistorsionZone(this));
		break;
	case Component::CompLight:
		components_list.push_back(component = new ComponentLight(this));
		break;
	case Component::CompGOAPAgent:
		components_list.push_back(component = new ComponentGOAPAgent(this));
		break;
	case Component::CompBlast:
		components_list.push_back(component = new ComponentBlast(this));
	default:
		break;
	}

	return component;
}

Component * GameObject::GetComponent(Component::ComponentType component_type)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == component_type) {
			return (*it);
		}
	}
	return nullptr;
}

Component * GameObject::GetComponent(std::string component_type)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetName() == component_type) {
			return (*it);
		}
	}
	return nullptr;
}

void GameObject::DestroyComponent(Component* component)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end();) {
		if (*it == component) 
		{
			if (component->GetType() == Component::CompCanvas)
				is_canvas = false;

			(*it)->CleanUp();
			RELEASE(*it);
			it = components_list.erase(it);
		}
		else 
		{
			it++;
		}
	}
}

void GameObject::AddChild(GameObject * gameobject)
{
	childs.push_back(gameobject);
}

bool GameObject::IsActive() const
{
	return active;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
	if (is_static)
	{
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::CompMeshRenderer);
		if (mesh_renderer != nullptr)
		{
			if (active)
			{
				App->scene->InsertGoInOctree(mesh_renderer);
			}
			else
			{
				App->scene->EraseGoInOctree(mesh_renderer);
			}
		}
	}

	ComponentRigidBody* rb = (ComponentRigidBody*)GetComponent(Component::CompRigidBody);
	if (rb)
	{
		if (!active)
		{
			//rb->SetToSleep();
			App->physics->RemoveRigidBodyFromScene(rb->GetRigidBody(), nullptr);
			App->physics->RemoveNonBlastActorFromList(rb->GetRigidBody());
		}
		else
		{
			//rb->WakeUp();
			App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
			App->physics->AddNonBlastActorToList(rb->GetRigidBody(), this);
		}
	}

	/*if (App->IsPlaying())
	{
		ComponentScript * comp_script = nullptr;
		for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
			if ((*it)->GetType() == Component::CompScript) {
				comp_script = (ComponentScript*)*it;
				if (active)
				{
					comp_script->OnEnable();
				}
				else
				{
					comp_script->OnDisable();
				}
			}
		}
	}*/
}

void GameObject::SetStatic(bool is_static)
{
	this->is_static = is_static;
	if (active)
	{
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::CompMeshRenderer);
		if (mesh_renderer != nullptr)
		{
			if (is_static)
			{
				App->scene->InsertGoInOctree(mesh_renderer);
				App->scene->static_meshes.push_back(mesh_renderer);
			}
			else
			{
				App->scene->EraseGoInOctree(mesh_renderer);
				App->scene->static_meshes.remove(mesh_renderer);
			}
		}
	}
}

bool GameObject::IsStatic() const
{
	return is_static;
}

bool GameObject::IsSelected() const
{
	return is_selected;
}

void GameObject::SetSelected(bool selected)
{
	is_selected = selected;
}

bool GameObject::IsRoot() const
{
	return is_root;
}

void GameObject::SetRoot(bool root)
{
	is_root = root;
}

GameObject * GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetParent(GameObject * parent)
{
	if (this->parent == parent) {
		return;
	}

	if (this->parent != nullptr) {
		this->parent->childs.remove(this);
	}

	this->parent = parent;
	if (this->parent != nullptr)
	{
		this->parent->childs.push_back(this);
	}

	if (is_root)
	{
		is_root = false;
		App->scene->root_gameobjects.remove(this);
	}
	else
	{
		if (this->parent == nullptr)
		{
			is_root = true;
			App->scene->root_gameobjects.push_back(this);
		}
	}
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetTag(std::string tag)
{
	this->tag = tag;
}

std::string GameObject::GetTag() const
{
	return tag;
}

void GameObject::SetLayer(std::string layer)
{
	this->layer = layer;
}

std::string GameObject::GetLayer() const
{
	return layer;
}

void GameObject::SetIsUsedInPrefab(bool used)
{
	is_used_in_prefab = used;
}

bool GameObject::GetIsUsedInPrefab() const
{
	return is_used_in_prefab;
}

void GameObject::SetNewUID()
{
	uuid = App->RandomNumber().Int();
}

int GameObject::GetAllChildsCount() const
{
	int count = childs.size();
	for (std::list<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); it++)
	{
		count += (*it)->GetAllChildsCount();
	}

	return count;
}

void GameObject::GetAllChildsName(std::vector<std::string>& names)
{
	for (std::list<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); it++)
	{
		(*it)->GetAllChildsName(names);
		names.push_back((*it)->name);
	}
}

void GameObject::GetAllChildsMeshesNames(std::vector<std::string>& names)
{
	for (std::list<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); it++)
	{
		(*it)->GetAllChildsName(names);
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::CompMeshRenderer);
		if (mesh_renderer != nullptr)
		{
			names.push_back((*it)->name);
		}
	}
}

void GameObject::GetAllChilds(std::vector<GameObject*>& childs_list) const
{
	for (std::list<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); it++)
	{
		(*it)->GetAllChilds(childs_list);
		childs_list.push_back(*it);
	}
}

void GameObject::UpdateBoundingBox()
{
	ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::CompMeshRenderer);
	if (mesh_renderer != nullptr)
	{
		mesh_renderer->UpdateBoundingBox();
	}
}

math::float4x4 GameObject::GetGlobalTransfomMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::CompTransform);
	return transform->GetMatrix();
}

math::float4x4 GameObject::GetOpenGLMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::CompTransform);
	return transform->GetOpenGLMatrix();
}

void GameObject::UpdateGlobalMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::CompTransform);
	transform->UpdateGlobalMatrix();
}

void GameObject::UpdateCamera()
{
	ComponentCamera* camera = (ComponentCamera*)GetComponent(Component::CompCamera);
	if (camera != nullptr)
	{
		camera->UpdatePosition();
	}
}

void GameObject::SetGlobalTransfomMatrix(const float4x4 & matrix)
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::CompTransform);
	transform->SetMatrix(matrix);
}

void GameObject::SetParentByID(UID parent_id)
{
	SetParent(App->scene->FindGameObject(parent_id));
}

void GameObject::InitScripts()
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->InitScript();
		}
	}
}

void GameObject::StartScripts()
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->StartScript();
		}
	}
}

void GameObject::UpdateScripts()
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->UpdateScript();
		}
	}
}

void GameObject::OnCollisionEnter(GameObject* other_collider)
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnCollisionEnter(other_collider);
		}
	}
}

void GameObject::OnCollisionStay(GameObject* other_collider)
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnCollisionStay(other_collider);
		}
	}
}

void GameObject::OnCollisionExit(GameObject* other_collider)
{
	ComponentScript* comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnCollisionExit(other_collider);
		}
	}
}

void GameObject::OnTriggerEnter(GameObject* other_collider)
{
	ComponentScript * comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnTriggerEnter(other_collider);
		}
	}
}

void GameObject::OnTriggerStay(GameObject* other_collider)
{
	ComponentScript * comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnTriggerStay(other_collider);
		}
	}
}

void GameObject::OnTriggerExit(GameObject* other_collider)
{
	ComponentScript * comp_script = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompScript) {
			comp_script = (ComponentScript*)*it;
			comp_script->OnTriggerExit(other_collider);
		}
	}
}

void GameObject::UpdateFactory()
{
	ComponentFactory* comp_factory = nullptr;
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == Component::CompFactory) {
			comp_factory = (ComponentFactory*)*it;
			comp_factory->CheckLifeTimes();
		}
	}
}

void GameObject::SetIsUI(bool set)
{
	if (!is_ui && set)
		AddComponent(Component::CompRectTransform);
	
	if (is_ui && !set)
		DestroyComponent(GetComponent(Component::CompRectTransform));
	
	is_ui = set;
}

bool GameObject::GetIsUI() const
{
	return is_ui;
}

bool GameObject::GetIsCanvas() const
{
	return is_canvas;
}

void GameObject::Destroy()
{
	App->scene->AddGameObjectToDestroy(this);
}

void GameObject::OnDestroy()
{
	is_on_destroy = true;

	std::list<GameObject*>::iterator it = find(App->scene->selected_gameobjects.begin(), App->scene->selected_gameobjects.end(), this);
	if (it != App->scene->selected_gameobjects.end()) {
		App->scene->selected_gameobjects.erase(it);
		is_selected = false;
	}

	std::list<GameObject*>::iterator it2 = find(App->scene->scene_gameobjects.begin(), App->scene->scene_gameobjects.end(), this);
	if (it2 != App->scene->scene_gameobjects.end()) {
		App->scene->scene_gameobjects.erase(it2);
	}
	App->resources->RemoveGameObject(this);

	ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::CompMeshRenderer);
	if (mesh_renderer) mesh_renderer->UnloadFromMemory();
	
	for (std::list<GameObject*>::iterator it = childs.begin(); it != childs.end();) {
		if (*it != nullptr) {
			(*it)->OnDestroy();
			CONSOLE_DEBUG("GameObject Destroyed: %s", (*it)->GetName().c_str());
			RELEASE(*it);
			it = childs.erase(it);
		}
	}

	if (parent != nullptr && !parent->is_on_destroy) {
		parent->childs.remove(this);
	}
	
}

UID GameObject::GetUID() const
{
	return uuid;
}


void GameObject::Save(Data & data, bool is_duplicated)
{
	std::string tempName = name;  //<- needed if is a duplicated game_object
	/*if (is_duplicated) {
		App->scene->RenameDuplicatedGameObject(this);
	}*/

	data.CreateSection("GameObject_" + std::to_string(App->scene->saving_index++));
	uint new_uuid = uuid;
	bool temp_active = active;
	bool temp_static = is_static;
	bool temp_selected = is_selected;
	std::string temp_tag = tag;
	std::string temp_layer = layer;
	if (is_duplicated)
	{
		uuid = App->RandomNumber().Int();
		/*active = true;
		is_static = false;
		is_selected = false;
		tag = "Default";
		layer = "Default";*/
	}
	data.AddUInt("UUID", uuid);
	data.AddString("Name", name);
	data.AddString("Tag", tag);
	data.AddString("Layer", layer);
	data.AddBool("Active", active);

	data.CreateSection("Components");
	data.AddInt("Components_Count", components_list.size());
	int componentIndex = 0;
	for (std::list<Component*>::const_iterator it = components_list.begin(); it != components_list.end(); it++) {
		data.CreateSection("Component_" + std::to_string(componentIndex));
		(*it)->Save(data);
		data.CloseSection();
		componentIndex++;
	}
	data.CloseSection();

	data.AddUInt("ParentID", parent ? parent->GetUID() : 0);
	data.AddBool("IsRoot", is_root);
	data.AddBool("IsStatic", is_static);
	data.CloseSection();

	//Save all childs recursively
	for (std::list<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); it++) {
		(*it)->Save(data, is_duplicated);
	}

	name = tempName;
	uuid = new_uuid;
	active = temp_active;
	is_static = temp_static;
	is_selected = temp_selected;
	tag = temp_tag;
	layer = temp_layer;
}

void GameObject::Load(Data & data, bool is_prefab)
{
	uuid = data.GetUInt("UUID");
	name = data.GetString("Name");
	tag = data.GetString("Tag");
	layer = data.GetString("Layer");
	active = data.GetBool("Active");
	data.EnterSection("Components");
	int componentsCount = data.GetInt("Components_Count");
	for (int i = 0; i < componentsCount; i++)
	{
		data.EnterSection("Component_" + std::to_string(i));
		int comp_type = data.GetInt("Type");
		uint comp_id = data.GetUInt("UUID");
		bool exist = false;
		if (comp_type == 0)
		{
			ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::CompTransform);
			transform->Load(data);
			data.LeaveSection();
			continue;
		}
		if (comp_id > 0)
		{
			for (Component* comp : components_list)
			{
				if (comp->GetUID() == comp_id)
				{
					comp->Load(data);
					exist = true;
					break;
				}
			}

			if (!exist)
			{
				Component* new_comp = AddComponent((Component::ComponentType)comp_type);

				if (new_comp != nullptr)
					new_comp->Load(data);
			}
		}
		else
		{
			CONSOLE_ERROR("Could not load component from gameobject: %s (Wrong component id?)", name.c_str());
		}
		data.LeaveSection();
	}
	data.LeaveSection();

	UID parent_id = data.GetUInt("ParentID");
	if (parent_id != 0) 
	{
		SetParentByID(parent_id);
	}

	is_root = data.GetBool("IsRoot");

	if (!is_prefab) 
	{
		//Store gameObject name to know the existing gameObjects when loading scene
		int gameObjectCount = 1;
		bool inParenthesis = false;
		std::string str;
		std::string tempName = name;
		for (int i = 0; i < name.size(); i++) 
		{
			if (name[i] == ')') 
			{
				inParenthesis = false;
				if (name[i + 1] == '\0') 
				{
					break;
				}
				else 
				{
					str.clear();
				}
			}
			if (inParenthesis) 
			{
				str.push_back(name[i]);
			}
			if (name[i] == '(') 
			{
				inParenthesis = true;
			}
		}
		if (atoi(str.c_str()) != 0) 
		{
			name.erase(name.end() - (str.length() + 2), name.end());
			gameObjectCount = stoi(str);
		}

		std::map<std::string, int>::iterator it = App->scene->scene_gameobjects_name_counter.find(name);
		if (it != App->scene->scene_gameobjects_name_counter.end()) 
		{
			if (App->scene->scene_gameobjects_name_counter[name] < gameObjectCount) 
			{
				App->scene->scene_gameobjects_name_counter[name] = gameObjectCount;
			}
		}
		else 
		{
			App->scene->scene_gameobjects_name_counter[name] = 1;
		}
		name = tempName;
	}
}

bool GameObject::Update()
{
	bool ret = true;

	for (std::list<Component*>::iterator c = components_list.begin(); c != components_list.end(); ++c)
	{
		ret = (*c)->Update();

		if (ret == false)
			break;
	}

	return ret;
}

void GameObject::DeleteFromResourcesDestructor()
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); ++it) {
		RELEASE(*it);
	}
}
