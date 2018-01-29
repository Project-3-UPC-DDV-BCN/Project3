#include "ResourcesWindow.h"
#include "Application.h"
#include "ModuleResources.h"
#include "Texture.h"
#include "Mesh.h"
#include "Prefab.h"
#include "Material.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Script.h"
#include "PhysicsMaterial.h"
#include "BlastMesh.h"

ResourcesWindow::ResourcesWindow()
{
	active = false;
	window_name = "Resources";

	texture_to_return = nullptr;
	mesh_to_return = nullptr;
	prefab_to_return = nullptr;
	gameobject_to_return = nullptr;
	material_to_return = nullptr;
	script_to_return = nullptr;
	phys_mat_to_return = nullptr;
	blast_mesh_to_return = nullptr;

	texture_changed = false;
	mesh_changed = false;
	prefab_changed = false;
	gameobject_changed = false;
	material_changed = false;
	script_changed = false;
	phys_mat_changed = false;
	blast_mesh_changed = false;

	type = Resource::Unknown;
	go_filter = GoFilterNone;
}

ResourcesWindow::~ResourcesWindow()
{
}

void ResourcesWindow::DrawWindow()
{
	ImGui::SetNextWindowSize({ 300,400 });
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	switch (type)
	{
	case Resource::TextureResource:
		textures_list = App->resources->GetTexturesList();
		if (ImGui::Selectable("None##Texture"))
		{
			texture_to_return = nullptr;
			texture_changed = true;
			break;
		}
		for (std::map<uint, Texture*>::const_iterator it = textures_list.begin(); it != textures_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				texture_to_return = it->second;
				texture_changed = true;
				break;
			}
		}
		break;
	case Resource::MeshResource:
		meshes_list = App->resources->GetMeshesList();
		if (ImGui::Selectable("None##Mesh"))
		{
			mesh_to_return = nullptr;
			mesh_changed = true;
			break;
		}
		for (std::map<uint, Mesh*>::const_iterator it = meshes_list.begin(); it != meshes_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				mesh_to_return = it->second;
				mesh_changed = true;
				break;
			}
		}
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		prefabs_list = App->resources->GetPrefabsList();
		if (ImGui::Selectable("None##prefab"))
		{
			prefab_to_return = nullptr;
			prefab_changed = true;
			break;
		}
		for (std::map<uint, Prefab*>::const_iterator it = prefabs_list.begin(); it != prefabs_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				prefab_to_return = it->second;
				prefab_changed = true;
				break;
			}
		}
		break;
	case Resource::ScriptResource:
		scripts_list = App->resources->GetScriptsList();
		if (ImGui::Selectable("None##script"))
		{
			script_to_return = nullptr;
			script_changed = true;
			break;
		}
		for (std::map<uint, Script*>::const_iterator it = scripts_list.begin(); it != scripts_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				script_to_return = it->second;
				script_changed = true;
				break;
			}
		}
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:
		break;
	case Resource::FontResource:
		break;
	case Resource::RenderTextureResource:
		break;
	case Resource::GameObjectResource:
		gameobjects_list = App->scene->scene_gameobjects;
		if (ImGui::Selectable("None##gameobject"))
		{
			gameobject_to_return = nullptr;
			gameobject_changed = true;
			break;
		}
		for (std::list<GameObject*>::const_iterator it = gameobjects_list.begin(); it != gameobjects_list.end(); it++)
		{
			if (go_filter != GoFilterNone)
			{
				switch (go_filter)
				{
				case ResourcesWindow::GoFilterRigidBody:
					if ((*it)->GetComponent(Component::CompRigidBody) == nullptr) continue;
					break;
				}
			}
			if (ImGui::Selectable((*it)->GetName().c_str()))
			{
				gameobject_to_return = *it;
				gameobject_changed = true;
				break;
			}
		}
		break;
	case Resource::MaterialResource:
		materials_list = App->resources->GetMaterialsList();
		if (ImGui::Selectable("None##material"))
		{
			material_to_return = nullptr;
			material_changed = true;
			break;
		}
		for (std::map<uint, Material*>::const_iterator it = materials_list.begin(); it != materials_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				material_to_return = it->second;
				material_changed = true;
				break;
			}
		}
		break;
	case Resource::PhysicsMatResource:
		phys_material_list = App->resources->GetPhysMaterialsList();
		if (ImGui::Selectable("None##physmaterial"))
		{
			phys_mat_to_return = nullptr;
			phys_mat_changed = true;
			break;
		}
		for (std::map<uint, PhysicsMaterial*>::const_iterator it = phys_material_list.begin(); it != phys_material_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				phys_mat_to_return = it->second;
				phys_mat_changed = true;
				break;
			}
		}
		break;
	case Resource::Unknown:
		break;
	case Resource::BlastMeshResource:
		blast_meshes_list = App->resources->GetBlastMeshesList();
		if (ImGui::Selectable("None##Mesh"))
		{
			blast_mesh_to_return = nullptr;
			blast_mesh_changed = true;
			break;
		}
		for (std::map<uint, BlastMesh*>::const_iterator it = blast_meshes_list.begin(); it != blast_meshes_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				blast_mesh_to_return = it->second;
				blast_mesh_changed = true;
				break;
			}
		}
		break;
	default:
		break;
	}

	ImGui::End();
}

void ResourcesWindow::SetResourceType(Resource::ResourceType type)
{
	this->type = type;
}

Texture * ResourcesWindow::GetTexture() const
{
	return texture_to_return;
}

Mesh * ResourcesWindow::GetMesh() const
{
	return mesh_to_return;
}

Prefab * ResourcesWindow::GetPrefab() const
{
	return prefab_to_return;
}

GameObject * ResourcesWindow::GetGameobject() const
{
	return gameobject_to_return;
}

Material * ResourcesWindow::GetMaterial() const
{
	return material_to_return;
}

Script * ResourcesWindow::GetScript() const
{
	return script_to_return;
}

PhysicsMaterial * ResourcesWindow::GetPhysMat() const
{
	return phys_mat_to_return;
}

BlastMesh * ResourcesWindow::GetBlastMesh() const
{
	return blast_mesh_to_return;
}

void ResourcesWindow::Reset()
{
	texture_changed = false;
	mesh_changed = false;
	prefab_changed = false;
	gameobject_changed = false;
	material_changed = false;
	script_changed = false;
	phys_mat_changed = false;
	blast_mesh_changed = false;

	texture_to_return = nullptr;
	mesh_to_return = nullptr;
	prefab_to_return = nullptr;
	gameobject_to_return = nullptr;
	material_to_return = nullptr;
	script_to_return = nullptr;
	phys_mat_to_return = nullptr;
	blast_mesh_to_return = nullptr;

	go_filter = GoFilterNone;
}
