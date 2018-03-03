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
#include "BlastModel.h"
#include "Shader.h"
#include "Font.h"
#include "SoundBank.h"

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
	blast_model_to_return = nullptr;
	shader_to_return = nullptr;
	font_to_return = nullptr;

	texture_changed = false;
	mesh_changed = false;
	prefab_changed = false;
	gameobject_changed = false;
	material_changed = false;
	script_changed = false;
	phys_mat_changed = false;
	blast_model_changed = false;
	shader_changed = false;
	font_changed = false;
	soundbank_changed = false;

	shader_type = Shader::ShaderType::ST_NULL;

	type = Resource::Unknown;
	go_filter = GoFilterNone;

	current_input_name = "";
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

	static char input_text[256];
	ImGui::Text("Resource Filter: ");
	ImGui::SameLine();
	ImGui::TextColored({ 0,1,1,1 }, "*case sensitive!");
	ImGui::InputText("", input_text, 256);
	ImGui::Spacing();

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
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					texture_to_return = it->second;
					texture_changed = true;
					break;
				}
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
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					mesh_to_return = it->second;
					mesh_changed = true;
					break;
				}
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
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					prefab_to_return = it->second;
					prefab_changed = true;
					break;
				}
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
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					script_to_return = it->second;
					script_changed = true;
					break;
				}
			}
		}
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:
		break;
	case Resource::FontResource:
		fonts_list = App->resources->GetFontsList();
		if (ImGui::Selectable("None##fonts"))
		{
			font_to_return = nullptr;
			font_changed = true;
			break;
		}
		for (std::map<uint, Font*>::const_iterator it = fonts_list.begin(); it != fonts_list.end(); it++)
		{
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					font_to_return = it->second;
					font_changed = true;
					break;
				}
			}
		}
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

			std::string name = (*it)->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					gameobject_to_return = *it;
					gameobject_changed = true;
					break;
				}
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
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					material_to_return = it->second;
					material_changed = true;
					break;
				}
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
	case Resource::ShaderResource:
		shaders_list = App->resources->GetShadersList();
		if (ImGui::Selectable("None##shader"))
		{
			shader_to_return = nullptr;
			shader_changed = true;
			break;
		}
		for (std::map<uint, Shader*>::const_iterator it = shaders_list.begin(); it != shaders_list.end(); it++)
		{
			if (it->second->GetShaderType() != shader_type) continue;
			std::string name = it->second->GetName();
			if (input_text[0] == 0 || name.find(input_text) != std::string::npos)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					shader_to_return = it->second;
					shader_changed = true;
					break;
				}
			}
		}
		break;
	case Resource::Unknown:
		break;
	case Resource::BlastMeshResource:
		blast_models_list = App->resources->GetBlastModelsList();
		if (ImGui::Selectable("None##Mesh"))
		{
			blast_model_to_return = nullptr;
			blast_model_changed = true;
			break;
		}
		for (std::map<uint, BlastModel*>::const_iterator it = blast_models_list.begin(); it != blast_models_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				blast_model_to_return = it->second;
				blast_model_changed = true;
				break;
			}
		}
		break;
	case Resource::SoundBankResource:
		soundbanks_list = App->resources->GetBlastModelsList();
		if (ImGui::Selectable("None##Mesh"))
		{
			blast_model_to_return = nullptr;
			blast_model_changed = true;
			break;
		}
		for (std::map<uint, BlastModel*>::const_iterator it = blast_models_list.begin(); it != blast_models_list.end(); it++)
		{
			if (ImGui::Selectable(it->second->GetName().c_str()))
			{
				blast_model_to_return = it->second;
				blast_model_changed = true;
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

BlastModel * ResourcesWindow::GetBlastModel() const
{
	return blast_model_to_return;
}

Shader * ResourcesWindow::GetShader() const
{
	return shader_to_return;
}

Font * ResourcesWindow::GetFont() const
{
	return font_to_return;
}

SoundBank * ResourcesWindow::GetSoundBank() const
{
	return soundbank_to_return;
}

void ResourcesWindow::SetShaderType(Shader::ShaderType type)
{
	shader_type = type;
}

void ResourcesWindow::SetCurrentInputName(std::string name)
{
	current_input_name = name;
}

std::string ResourcesWindow::GetCurrentInputName() const
{
	return current_input_name;
}

void ResourcesWindow::Reset()
{
	texture_changed = false;
	mesh_changed = false;
	prefab_changed = false;
	gameobject_changed = false;
	material_changed = false;
	script_changed = false;
	font_changed = false;
	phys_mat_changed = false;
	blast_model_changed = false;
	
	texture_to_return = nullptr;
	mesh_to_return = nullptr;
	prefab_to_return = nullptr;
	gameobject_to_return = nullptr;
	material_to_return = nullptr;
	script_to_return = nullptr;
	font_to_return = nullptr;
	phys_mat_to_return = nullptr;
	blast_model_to_return = nullptr;

	go_filter = GoFilterNone;

	shader_type = Shader::ShaderType::ST_NULL;
	current_input_name = "";
}
