#include "ComponentFactory.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleTime.h"
#include "Application.h"
#include "Prefab.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ComponentRigidBody.h"

ComponentFactory::ComponentFactory(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Factory");
	SetType(ComponentType::CompFactory);
	SetGameObject(attached_gameobject);

	prefab_to_spawn_name = "";
	object_count = 0;
	life_time = 1;
	spawn_position = float3::zero;
	spawn_rotation = float3::zero;
}

ComponentFactory::~ComponentFactory()
{
}

bool ComponentFactory::SetFactoryObjectName(std::string _prefab_to_spawn_name)
{
	bool ret = false;

	if (prefab_to_spawn_name != _prefab_to_spawn_name)
	{
		prefab_to_spawn_name = _prefab_to_spawn_name;

		prefab_to_spawn = App->resources->GetPrefab(prefab_to_spawn_name);

		if (prefab_to_spawn != nullptr)
			ret = true;
	}

	return ret;
}

void ComponentFactory::SetObjectCount(int count)
{
	object_count = count;
}

void ComponentFactory::SetSpawnPos(float3 position)
{
	spawn_position = position;
}

void ComponentFactory::SetSpawnRotation(float3 rotation)
{
	spawn_rotation = rotation;
}

void ComponentFactory::SetSpawnScale(float3 scale)
{
	spawn_scale = scale;
}

void ComponentFactory::SetLifeTime(float life_time)
{
	this->life_time = life_time;
}

GameObject* ComponentFactory::Spawn()
{
	BROFILER_CATEGORY("Component - Factory - Spawn", Profiler::Color::Beige);

	GameObject* go = nullptr;

	if(!spawn_objects_list.empty()) 
		go = spawn_objects_list.front();

	if (go != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
		ComponentTransform* factory_trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

		if (transform != nullptr)
		{
			transform->SetPosition(float3(factory_trans->GetGlobalPosition()));
			transform->SetRotation(float3(0, 0, 0));
			go->SetParent(nullptr);

			spawned_objects[go] = life_time;
			spawn_objects_list.remove(go);
			go->SetActive(true);
		}
	}
	else
	{
		//CONSOLE_ERROR("Factory component in %s is empty. Wait for more GameObjects", GetGameObject()->GetName().c_str());
	}

	return go;
}

int ComponentFactory::GetCurrentCount() const
{
	return spawn_objects_list.size();
}

float ComponentFactory::GetLifeTime() const
{
	return life_time;
}

std::string ComponentFactory::GetFactoryObjectName() const
{
	return prefab_to_spawn_name;
}

int ComponentFactory::GetObjectCount() const
{
	return object_count;
}

float3 ComponentFactory::GetSpawnPosition() const
{
	return spawn_position;
}

float3 ComponentFactory::GetSpawnRotation() const
{
	return spawn_rotation;
}

float3 ComponentFactory::GetSpawnScale() const
{
	return spawn_scale;
}

void ComponentFactory::StartFactory()
{
	BROFILER_CATEGORY("Component - Factory - StartFactory", Profiler::Color::Beige);

	if (prefab_to_spawn != nullptr )
	{		
		if (GetGameObject() != nullptr)
		{
			ComponentTransform* transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
			if (transform)
			{
				original_position = transform->GetGlobalPosition();
				SetSpawnPos(original_position);
			}
		}

		for (int i = 0; i < object_count; i++)
		{
			Data data;
			std::list<GameObject*> new_go;
			if (App->scene->LoadPrefab(prefab_to_spawn->GetLibraryPath(), "jprefab", data, false, true, new_go))
			{
				GameObject* duplicated = *new_go.begin();
				duplicated->SetParent(GetGameObject());
				duplicated->SetActive(false);
				spawn_objects_list.push_back(duplicated);
			}
		}
	}
	else
	{
		CONSOLE_ERROR("%s Factory component is trying to spawn a null prefab or any object from the prefab is null", GetGameObject()->GetName().c_str());
	}
}

void ComponentFactory::ClearFactory()
{
	for (std::list<GameObject*>::iterator it = spawn_objects_list.begin(); it != spawn_objects_list.end(); ++it)
	{
		(*it)->Destroy();
	}

	for (std::map<GameObject*, float>::iterator it = spawned_objects.begin(); it != spawned_objects.end(); ++it)
	{
		it->first->Destroy();
	}

	spawned_objects.clear();
	spawn_objects_list.clear();
}

void ComponentFactory::CheckLifeTimes()
{
	BROFILER_CATEGORY("Component - Factory - CheckLifeTimes", Profiler::Color::Beige);

	for (std::map<GameObject*, float>::iterator it = spawned_objects.begin(); it != spawned_objects.end();)
	{
		if (it->second <= 0)
		{
			ComponentTransform* transform = (ComponentTransform*)it->first->GetComponent(Component::CompTransform);
			if (transform)
			{
				it->first->SetParent(GetGameObject());
				it->first->SetActive(false);
				transform->SetPosition(float3(0, 0, 0));
				transform->SetRotation(float3(0, 0, 0));
				spawn_objects_list.push_back(it->first);
				it = spawned_objects.erase(it);
			}
		}
		else
		{
			it->second -= App->time->GetGameDt();
			it++;
		}
	}
}

void ComponentFactory::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddInt("Object_count", object_count);
	data.AddVector3("Spawn_position", spawn_position);
	data.AddVector3("Spawn_rotation", spawn_rotation);
	data.AddVector3("Spawn_scale", spawn_scale);
	data.AddFloat("Life_time", life_time);
	data.AddString("Object_to_spawn_name", prefab_to_spawn_name);
}

void ComponentFactory::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	object_count = data.GetInt("Object_count");
	spawn_position = data.GetVector3("Spawn_position");
	spawn_rotation = data.GetVector3("Spawn_rotation");
	spawn_scale = data.GetVector3("Spawn_scale");
	life_time = data.GetFloat("Life_time");
	SetFactoryObjectName(data.GetString("Object_to_spawn_name"));
}
