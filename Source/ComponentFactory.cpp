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

	object_to_spawn = nullptr;
	object_count = 0;
	life_time = 0;
	spawn_position = float3::zero;
	spawn_rotation = float3::zero;
}

ComponentFactory::~ComponentFactory()
{
}

void ComponentFactory::SetFactoryObject(Prefab * gameobject)
{
	if (gameobject)
	{
		object_to_spawn = gameobject;
	}
	else
	{
		CONSOLE_ERROR("Prefab assigned to factory is null");
	}
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
	GameObject* go = nullptr;
	if(!spawn_objects_list.empty()) go = spawn_objects_list.front();
	if (go != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
		if (transform)
		{
			transform->SetPosition(spawn_position);
			//transform->SetRotation(spawn_rotation);
			transform->SetScale(spawn_scale);
			ComponentRigidBody* rb = (ComponentRigidBody*)go->GetComponent(Component::CompRigidBody);
			if (rb)
			{
				float4x4 m = transform->GetOpenGLMatrix();
				rb->SetTransform(m.ptr());
				//rb->SetLinearVelocity({ 0,0,0 });
				/*rb->SetPosition(spawn_position);
				rb->SetRotation(spawn_rotation);*/
			}
			spawned_objects[go] = life_time;
			spawn_objects_list.remove(go);
			go->SetActive(true);
		}
	}
	else
	{
		CONSOLE_ERROR("Factory component in %s is empty. Wait for more GameObjects", GetGameObject()->GetName().c_str());
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

Prefab * ComponentFactory::GetFactoryObject() const
{
	return object_to_spawn;
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
	if (object_to_spawn && object_to_spawn->GetRootGameObject())
	{
		ComponentTransform* transform = (ComponentTransform*)object_to_spawn->GetRootGameObject()->GetComponent(Component::CompTransform);
		if (transform)
		{
			original_rotation = transform->GetLocalRotation();
			original_scale = transform->GetLocalScale();
			SetSpawnRotation(original_rotation);
			SetSpawnScale(original_scale);
		}

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
			object_to_spawn->GetRootGameObject()->Save(data, true);
			for (int i = 0; i < App->scene->saving_index; i++) {
				GameObject* go = new GameObject();
				data.EnterSection("GameObject_" + std::to_string(i));
				go->Load(data, true);
				if (GetGameObject() && i == 0)
				{
					go->SetRoot(true);
					//go->SetParent(GetGameObject());
					spawn_objects_list.push_back(go);
					go->SetActive(false);
				}
				App->scene->AddGameObjectToScene(go);
				App->resources->AddGameObject(go);
				data.LeaveSection();
			}
			App->scene->saving_index = 0;
		}
	}
	else
	{
		CONSOLE_ERROR("%s Factory component is trying to spawn a null prefab or any object from the prefab is null", GetGameObject()->GetName().c_str());
	}
}

void ComponentFactory::CheckLifeTimes()
{
	for (std::map<GameObject*, float>::iterator it = spawned_objects.begin(); it != spawned_objects.end();)
	{
		if (it->second <= 0)
		{
			ComponentTransform* transform = (ComponentTransform*)it->first->GetComponent(Component::CompTransform);
			if (transform)
			{
				it->first->SetActive(false);
				transform->SetPosition(original_position);
				transform->SetRotation(original_rotation);
				transform->SetScale(original_scale);
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
	int object_uid = 0;
	if (object_to_spawn)
	{
		object_uid = object_to_spawn->GetUID();
	}
	data.AddInt("Prefab_UID", object_uid);
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
	object_to_spawn = App->resources->GetPrefab(data.GetInt("Prefab_UID"));
	if (!object_to_spawn)
	{
		CONSOLE_WARNING("Prefab loaded for component Factory in %s doesn't exist", GetGameObject()->GetName().c_str());
	}
}
