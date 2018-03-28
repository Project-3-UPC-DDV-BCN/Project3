#include "ModuleResources.h"
#include "Texture.h"
#include "Mesh.h"
#include "Prefab.h"
#include "GameObject.h"
#include "Material.h"
#include "Resource.h"
#include "Data.h"
#include "ParticleData.h"
#include "Resource.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleMeshImporter.h"
#include "ModuleTextureImporter.h"
#include "ModuleParticleImporter.h"
#include "ModulePrefabImporter.h"
#include "ModuleMaterialImporter.h"
#include "ComponentMeshRenderer.h"
#include "Script.h"
#include "ModuleScriptImporter.h"
#include "PhysicsMaterial.h"
#include "ModulePhysMatImporter.h"
#include "BlastModel.h"
#include "ModuleBlastMeshImporter.h"
#include "Shader.h"
#include "ModuleShaderImporter.h"
#include "ShaderProgram.h"
#include "Font.h"
#include "SoundBank.h"
#include "SoundBankResource.h"
#include "ModuleAudioImporter.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"

ModuleResources::ModuleResources(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Resources";
}

ModuleResources::~ModuleResources()
{
	for (std::map<uint, GameObject*>::iterator it = gameobjects_list.begin(); it != gameobjects_list.end(); ++it) 
	{
		it->second->DeleteFromResourcesDestructor();
		it->second = nullptr;
	}
	gameobjects_list.clear();

	for (std::map<uint, Texture*>::iterator it = textures_list.begin(); it != textures_list.end(); ++it) {
		RELEASE(it->second);
	}
	textures_list.clear();

	for (std::map<uint, Mesh*>::iterator it = meshes_list.begin(); it != meshes_list.end(); ++it) {
		RELEASE(it->second);
	}
	meshes_list.clear();

	for (std::map<uint, Prefab*>::iterator it = prefabs_list.begin(); it != prefabs_list.end(); ++it) {
		RELEASE(it->second);
	}
	prefabs_list.clear();
	
	for (std::map<uint, Material*>::iterator it = materials_list.begin(); it != materials_list.end(); ++it) {
		RELEASE(it->second);
	}
	materials_list.clear();

	for (std::map<uint, Font*>::iterator it = fonts_list.begin(); it != fonts_list.end(); ++it)
	{
		RELEASE(it->second);
	}
	fonts_list.clear();
	for (std::map<uint, SoundBankResource*>::iterator it = soundbanks_list.begin(); it != soundbanks_list.end(); ++it)
	{
		RELEASE(it->second);
	}
	fonts_list.clear();

	Data shprograms;
	uint i = 0;
	for (std::map<uint, ShaderProgram*>::iterator it = shader_programs_list.begin(); it != shader_programs_list.end(); ++it) 
	{
		if (it->second->GetUsedCount() > 0)
		{
			shprograms.CreateSection("shprogram_" + std::to_string(i));
			shprograms.AddUInt("uuid", it->second->GetUID());
			shprograms.AddUInt("vertex_shader", (it->second->GetVertexShader() != nullptr) ? it->second->GetVertexShader()->GetUID() : 0);
			shprograms.AddUInt("fragment_shader", (it->second->GetFragmentShader() != nullptr) ? it->second->GetFragmentShader()->GetUID() : 0);
			shprograms.CloseSection();
			++i;
		}

		RELEASE(it->second);
	}
	shader_programs_list.clear();
	shprograms.AddUInt("num_sections", i);
	shprograms.SaveAsMeta(LIBRARY_SHADERS_FOLDER"shprograms");

	for (std::map<uint, Shader*>::iterator it = shaders_list.begin(); it != shaders_list.end(); ++it) 
	{
		RELEASE(it->second);
	}
	shaders_list.clear();

	for (std::map<uint, GOAPGoal*>::iterator it = goap_goals_list.begin(); it != goap_goals_list.end(); ++it)
	{
		RELEASE(it->second);
	}
	goap_goals_list.clear();

}

bool ModuleResources::Init(Data * editor_config)
{
	FillResourcesLists();
	return true;
}

void ModuleResources::FillResourcesLists()
{
	std::string resources_folder_path;
	if (!App->IsGame())
	{
		resources_folder_path = App->file_system->StringToPathFormat(ASSETS_FOLDER_PATH);

		if (!App->file_system->DirectoryExist(LIBRARY_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_TEXTURES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_TEXTURES_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_MESHES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MESHES_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PREFABS_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_MATERIALS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MATERIALS_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_FONTS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_FONTS_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_SHADERS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SHADERS_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_BMODEL_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_BMODEL_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_SOUNDBANK_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SOUNDBANK_FOLDER_PATH);
		if (!App->file_system->DirectoryExist(LIBRARY_GOAP_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_GOAP_FOLDER_PATH);

	}
	else
	{
		resources_folder_path = App->file_system->StringToPathFormat(LIBRARY_FOLDER_PATH);
	}

	CreateDefaultShaders();
	CreateDefaultMaterial();
	
	std::vector<std::string> files_in_assets = App->file_system->GetAllFilesRecursively(resources_folder_path);
	
	std::string shprog_meta_file;
	bool exist_shprog_meta = false;
	if (App->file_system->DirectoryExist(LIBRARY_SHADERS_FOLDER_PATH))
	{
		std::vector<std::string> files_in_shader_library = App->file_system->GetFilesInDirectory(App->file_system->StringToPathFormat(LIBRARY_SHADERS_FOLDER_PATH));

		for (std::vector<std::string>::iterator it = files_in_shader_library.begin(); it != files_in_shader_library.end(); it++)
		{
			std::string extension = App->file_system->GetFileExtension(*it);
			if (extension == ".shprog")
			{
				ShaderProgram* program = new ShaderProgram();
				program->LoadFromLibrary((*it).c_str());
			}
			else if (extension == ".meta") //meta file should be loaded after all programs and shaders
			{
				shprog_meta_file = *it;
				exist_shprog_meta = true;
			}
		}
	}

	/*for (std::vector<std::string>::iterator it = files_in_assets.begin(); it != files_in_assets.end(); it++)
	{
		if (App->file_system->GetFileName(*it).find("_blast") != std::string::npos) continue;
		else if (App->file_system->GetFileName(*it).find("_GOAPGoal") != std::string::npos) LoadGOAPGoal(*it);
		else if (App->file_system->GetFileName(*it).find("_GOAPAction") != std::string::npos) LoadGOAPAction(*it);
		else CreateResource(*it);
	}
		CreateResource(*it);
	}*/

	// Meshes
	std::vector<std::string> mesh_order;
	//Textures
	std::vector<std::string> texture_order;
	//Shader
	std::vector<std::string> shader_order;
	//ShaderProgram
	std::vector<std::string> shader_program_order;
	//Materials
	std::vector<std::string> material_order;
	//Scripts
	std::vector<std::string> script_order;
	//Audio

	//Font
	std::vector<std::string> font_order;
	//PhysicsMat
	std::vector<std::string> phys_mat_order;
	//BlastMesh
	std::vector<std::string> blast_mesh_order;
	//GOAPGoal
	std::vector<std::string> goap_goal_order;
	//GOAPAction
	std::vector<std::string> goap_action_order;
	//ParticleFX
	std::vector<std::string> particle_order;
	//prefabs
	std::vector<std::string> prefab_order;
	//SoundBank
	std::vector<std::string> soundback_order;

	// Create lists in order of Drive
	for (std::vector<std::string>::iterator it = files_in_assets.begin(); it != files_in_assets.end(); ++it)
	{
		std::string extension = App->file_system->GetFileExtension(*it);
		Resource::ResourceType type;
		if (!App->IsGame())
		{
			type = AssetExtensionToResourceType(extension);
		}
		else
		{
			type = LibraryExtensionToResourceType(extension);
		}
		if (type == Resource::Unknown)
		{
			if (App->file_system->GetFileName(*it).find("_GOAPGoal") != std::string::npos) type = Resource::GOAPGoalResource;
			if (App->file_system->GetFileName(*it).find("_GOAPAction") != std::string::npos) type = Resource::GOAPActionResource;
		}

		switch (type) {
		case Resource::MeshResource:
			if (App->file_system->GetFileName(*it).find("_blast") != std::string::npos) continue;
			mesh_order.push_back(*it);
			break;
		case Resource::TextureResource:
			texture_order.push_back(*it);
			break;
		case Resource::ShaderResource:
			shader_order.push_back(*it);
			break;
		case Resource::ShaderProgramResource:
			shader_program_order.push_back(*it);
			break;
		case Resource::MaterialResource:
			material_order.push_back(*it);
			break;
		case Resource::ScriptResource:
			script_order.push_back(*it);
			break;
		case Resource::AudioResource:
			break;
		case Resource::FontResource:
			font_order.push_back(*it);
			break;
		case Resource::PhysicsMatResource:
			phys_mat_order.push_back(*it);
			break;
		case Resource::BlastMeshResource:
			blast_mesh_order.push_back(*it);
			break;
		case Resource::GOAPGoalResource:
			goap_goal_order.push_back(*it);
			break;
		case Resource::GOAPActionResource:
			goap_action_order.push_back(*it);
			break;
		case Resource::ParticleFXResource:
			particle_order.push_back(*it);
			break;
		case Resource::PrefabResource:
			prefab_order.push_back(*it);
			break;
		case Resource::SoundBankResource:
			soundback_order.push_back(*it);
			break;
		case Resource::SceneResource:
			if (!App->IsGame())
			{
				if (!App->file_system->DirectoryExist(LIBRARY_SCENES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SCENES_FOLDER_PATH);
				std::string scene_name = App->file_system->GetFileName(*it);
				App->file_system->Copy(*it, LIBRARY_SCENES_FOLDER + scene_name);
				scene_list.push_back(LIBRARY_SCENES_FOLDER + scene_name);
			}
			else
			{
				scene_list.push_back(*it);
			}
			break;
		}
	}

	for (std::vector<string>::iterator it = texture_order.begin(); it != texture_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = material_order.begin(); it != material_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = mesh_order.begin(); it != mesh_order.end(); ++it)
		CreateLibraryFile(Resource::MeshResource ,*it);

	for (std::vector<string>::iterator it = blast_mesh_order.begin(); it != blast_mesh_order.end(); ++it)
		CreateResource(*it);

	std::vector<std::string> meshes_in_mesh_library = App->file_system->GetFilesInDirectory(App->file_system->StringToPathFormat(LIBRARY_MESHES_FOLDER_PATH));
	for (std::vector<std::string>::iterator it = meshes_in_mesh_library.begin(); it != meshes_in_mesh_library.end(); ++it)
	{
		Mesh* m = App->mesh_importer->LoadMeshFromLibrary(*it);
		AddMesh(m);
	}

	for (std::vector<string>::iterator it = script_order.begin(); it != script_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = prefab_order.begin(); it != prefab_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = shader_order.begin(); it != shader_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = shader_program_order.begin(); it != shader_program_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = soundback_order.begin(); it != soundback_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = font_order.begin(); it != font_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = phys_mat_order.begin(); it != phys_mat_order.end(); ++it)
		CreateResource(*it);

	for (std::vector<string>::iterator it = goap_goal_order.begin(); it != goap_goal_order.end(); ++it)
		LoadGOAPGoal(*it);

	for (std::vector<string>::iterator it = goap_action_order.begin(); it != goap_action_order.end(); ++it)
		LoadGOAPAction(*it);

	for (std::vector<string>::iterator it = particle_order.begin(); it != particle_order.end(); ++it)
		CreateResource(*it);

	if (exist_shprog_meta) //shader meta should be the last thing because it contains the programs
		LoadShaderProgramMeta(shprog_meta_file);
}

void ModuleResources::AddResource(Resource * resource)
{
	if (resource == nullptr) return;
	Resource::ResourceType type = resource->GetType();
	switch (type)
	{
	case Resource::TextureResource:
		AddTexture((Texture*)resource);
		break;
	case Resource::MeshResource:
		AddMesh((Mesh*)resource);
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		AddPrefab((Prefab*)resource);
		break;
	case Resource::ScriptResource:
		AddScript((Script*)resource);
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:

		break;
	case Resource::FontResource:
		AddFont((Font*)resource);
		break;
	case Resource::MaterialResource:
		AddMaterial((Material*)resource);
		break;
	case Resource::PhysicsMatResource:
		AddPhysMaterial((PhysicsMaterial*)resource);
		break;
	case Resource::BlastMeshResource:
		AddBlastModel((BlastModel*)resource);
	case Resource::ShaderResource:
		AddShader((Shader*)resource);
		break;
	case Resource::ShaderProgramResource:
		AddShaderProgram((ShaderProgram*)resource);
		break;
	case Resource::SoundBankResource:
		AddSoundBank((SoundBankResource*)resource);
		break;
	case Resource::Unknown:
		break;
	}
}

void ModuleResources::ImportFile(std::string path)
{
	std::string extension = App->file_system->GetFileExtension(path);
	std::string file_name = App->file_system->GetFileName(path);
	Resource::ResourceType type = AssetExtensionToResourceType(extension);

	if (extension == ".fbx" || extension == ".FBX") type = Resource::MeshResource;
	else if (extension == ".vshader" || extension == ".fshader") type = Resource::ShaderResource;
	else if (extension == ".bnk") type = Resource::SoundBankResource;

	bool exist = false;

	switch (type)
	{
	case Resource::TextureResource:
		if (!App->file_system->DirectoryExist(ASSETS_TEXTURES_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_TEXTURES_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_TEXTURES_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_TEXTURES_FOLDER + file_name);
		path = ASSETS_TEXTURES_FOLDER + file_name;
		break;
	case Resource::MeshResource:
		if (!App->file_system->DirectoryExist(ASSETS_FBX_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_FBX_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_FBX_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_FBX_FOLDER + file_name);
		path = ASSETS_FBX_FOLDER + file_name;
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		if (!App->file_system->DirectoryExist(ASSETS_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_PREFABS_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_PREFABS_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_PREFABS_FOLDER + file_name);
		path = ASSETS_PREFABS_FOLDER + file_name;
		break;
	case Resource::ScriptResource:
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:
		break;
	case Resource::FontResource:
		if (!App->file_system->DirectoryExist(ASSETS_FONTS_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_FONTS_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_FONTS_FOLDER_PATH + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_FONTS_FOLDER + file_name);
		path = ASSETS_FONTS_FOLDER + file_name;
		break;
	case Resource::RenderTextureResource:
		break;
	case Resource::GameObjectResource:
		break;
	case Resource::MaterialResource:
		break;
	case Resource::BlastMeshResource:
		if (!App->file_system->DirectoryExist(ASSETS_FBX_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_FBX_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_FBX_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_FBX_FOLDER + file_name);
		path = ASSETS_FBX_FOLDER + file_name;
		break;
	case Resource::Unknown:
		break;
	case Resource::ShaderResource:
		if (!App->file_system->DirectoryExist(ASSETS_SHADERS_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_SHADERS_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_SHADERS_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_SHADERS_FOLDER + file_name);
		path = ASSETS_SHADERS_FOLDER + file_name;
		break;
	case Resource::SoundBankResource:
		if (!App->file_system->DirectoryExist(ASSETS_SOUNDBANK_FOLDER_PATH)) App->file_system->Create_Directory(ASSETS_SOUNDBANK_FOLDER_PATH);
		if (App->file_system->FileExist(ASSETS_SOUNDBANK_FOLDER + file_name))
		{
			exist = true;
			break;
		}
		App->file_system->Copy(path, ASSETS_SOUNDBANK_FOLDER + file_name);
		path = ASSETS_SOUNDBANK_FOLDER + file_name;
		break;
	default:
		break;
	}

	if (!exist)CreateResource(path);
}

Texture * ModuleResources::GetTexture(std::string name) const
{
	for (std::map<uint, Texture*>::const_iterator it = textures_list.begin(); it != textures_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Texture * ModuleResources::GetTexture(UID uid) const
{
	for (std::map<uint, Texture*>::const_iterator it = textures_list.begin(); it != textures_list.end(); it++)
	{
		if (it->first == uid) return it->second;
	}
	return nullptr;
}

void ModuleResources::AddTexture(Texture * texture)
{
	if (texture != nullptr)
	{
		textures_list[texture->GetUID()] = texture;
	}
}

void ModuleResources::RemoveTexture(Texture * texture)
{
	if (texture)
	{
		std::map<uint, Texture*>::iterator it = textures_list.find(texture->GetUID());
		if (it != textures_list.end()) textures_list.erase(it);
	}
}

std::map<uint, Texture*> ModuleResources::GetTexturesList() const
{
	return textures_list;
}

Mesh * ModuleResources::GetMesh(std::string name) const
{
	for (std::map<uint, Mesh*>::const_iterator it = meshes_list.begin(); it != meshes_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Mesh * ModuleResources::GetMesh(UID uid) const
{
	if (meshes_list.find(uid) != meshes_list.end()) return meshes_list.at(uid);
	return nullptr;
}

void ModuleResources::AddMesh(Mesh * mesh)
{
	if (mesh != nullptr)
	{
		meshes_list[mesh->GetUID()] = mesh;
	}
}

void ModuleResources::RemoveMesh(Mesh * mesh)
{
	if (mesh)
	{
		std::map<uint, Mesh*>::iterator it = meshes_list.find(mesh->GetUID());
		if (it != meshes_list.end()) meshes_list.erase(it);
	}
}

std::map<uint, Mesh*> ModuleResources::GetMeshesList() const
{
	return meshes_list;
}

Prefab * ModuleResources::GetPrefab(std::string name) const
{
	for (std::map<uint, Prefab*>::const_iterator it = prefabs_list.begin(); it != prefabs_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Prefab * ModuleResources::GetPrefab(UID uid) const
{
	if (prefabs_list.find(uid) != prefabs_list.end()) return prefabs_list.at(uid);
	return nullptr;
}

void ModuleResources::AddPrefab(Prefab * prefab)
{
	if (prefab != nullptr)
	{
		prefabs_list[prefab->GetUID()] = prefab;
	}
}

void ModuleResources::RemovePrefab(Prefab * prefab)
{
	if (prefab)
	{
		std::map<uint, Prefab*>::iterator it = prefabs_list.find(prefab->GetUID());
		if (it != prefabs_list.end()) prefabs_list.erase(it);
	}
}

std::map<uint, Prefab*> ModuleResources::GetPrefabsList() const
{
	return prefabs_list;
}

GameObject * ModuleResources::GetGameObject(std::string name) const
{
	for (std::map<uint, GameObject*>::const_iterator it = gameobjects_list.begin(); it != gameobjects_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

GameObject * ModuleResources::GetGameObject(UID uid) const
{
	if (gameobjects_list.find(uid) != gameobjects_list.end()) return gameobjects_list.at(uid);
	return nullptr;
}

void ModuleResources::AddGameObject(GameObject * gameobject)
{
	if (gameobject != nullptr)
	{
		gameobjects_list[gameobject->GetUID()] = gameobject;
	}
}

void ModuleResources::RemoveGameObject(GameObject * gameobject)
{
	if (gameobject)
	{
		std::map<uint, GameObject*>::iterator it = gameobjects_list.find(gameobject->GetUID());
		if (it != gameobjects_list.end()) gameobjects_list.erase(it);
	}
}

std::map<uint, GameObject*> ModuleResources::GetGameobjectsList() const
{
	return gameobjects_list;
}

Material * ModuleResources::GetMaterial(std::string name) const
{
	for (std::map<uint, Material*>::const_iterator it = materials_list.begin(); it != materials_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Material * ModuleResources::GetMaterial(UID uid) const
{
	if (materials_list.find(uid) != materials_list.end()) return materials_list.at(uid);
	return nullptr;
}

void ModuleResources::AddMaterial(Material * material)
{
	if (material != nullptr)
	{
		materials_list[material->GetUID()] = material;
	}
}

void ModuleResources::RemoveMaterial(Material * material)
{
	if (material)
	{
		std::map<uint, Material*>::iterator it = materials_list.find(material->GetUID());
		if (it != materials_list.end()) materials_list.erase(it);
	}
}

std::map<uint, Material*> ModuleResources::GetMaterialsList() const
{
	return materials_list;
}

Script * ModuleResources::GetScript(std::string name) const
{
	for (std::map<uint, Script*>::const_iterator it = scripts_list.begin(); it != scripts_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Script * ModuleResources::GetScript(UID uid) const
{
	if (scripts_list.find(uid) != scripts_list.end()) return scripts_list.at(uid);
	return nullptr;
}

void ModuleResources::AddScript(Script * script)
{
	if (script != nullptr)
	{
		scripts_list[script->GetUID()] = script;
	}
}

void ModuleResources::RemoveScript(Script * script)
{
	if (script)
	{
		std::map<uint, Script*>::iterator it = scripts_list.find(script->GetUID());
		if (it != scripts_list.end()) scripts_list.erase(it);
	}
}

std::map<uint, Script*> ModuleResources::GetScriptsList() const
{
	return scripts_list;
}

PhysicsMaterial * ModuleResources::GetPhysMaterial(std::string name) const
{
	for (std::map<uint, PhysicsMaterial*>::const_iterator it = phys_materials_list.begin(); it != phys_materials_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Shader * ModuleResources::GetShader(std::string name) const
{
	for (std::map<uint, Shader*>::const_iterator it = shaders_list.begin(); it != shaders_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

PhysicsMaterial * ModuleResources::GetPhysMaterial(UID uid) const
{
	if (phys_materials_list.find(uid) != phys_materials_list.end()) return phys_materials_list.at(uid);
	return nullptr;
}

void ModuleResources::AddPhysMaterial(PhysicsMaterial * material)
{
	if (material != nullptr)
	{
		phys_materials_list[material->GetUID()] = material;
	}
}

void ModuleResources::RemovePhysMaterial(PhysicsMaterial * material)
{
	if (material)
	{
		std::map<uint, PhysicsMaterial*>::iterator it = phys_materials_list.find(material->GetUID());
		if (it != phys_materials_list.end()) phys_materials_list.erase(it);
	}
}

std::map<uint, PhysicsMaterial*> ModuleResources::GetPhysMaterialsList() const
{
	return phys_materials_list;
}

Shader * ModuleResources::GetShader(UID uid) const
{
	if (shaders_list.find(uid) != shaders_list.end()) return shaders_list.at(uid);
	return nullptr;
}

void ModuleResources::AddShader(Shader * shader)
{
	if (shader != nullptr)
	{
		shaders_list[shader->GetUID()] = shader;
	}
}

void ModuleResources::RemoveShader(Shader * shader)
{
	if (shader)
	{
		std::map<uint, Shader*>::iterator it = shaders_list.find(shader->GetUID());
		if (it != shaders_list.end()) shaders_list.erase(it);
	}
}

std::map<uint, Shader*> ModuleResources::GetShadersList() const
{
	return shaders_list;
}

Font * ModuleResources::GetFont(std::string name) const
{
	for (std::map<uint, Font*>::const_iterator it = fonts_list.begin(); it != fonts_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

ParticleData * ModuleResources::GetParticleTemplate(std::string name) const
{
	for (std::map<uint, ParticleData*>::const_iterator it = particles_list.begin(); it != particles_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

Font * ModuleResources::GetFont(UID uid) const
{
	if (fonts_list.find(uid) != fonts_list.end()) return fonts_list.at(uid);
	return nullptr;
}

void ModuleResources::AddFont(Font * font)
{
	if (font != nullptr)
	{
		fonts_list[font->GetUID()] = font;
	}
}

void ModuleResources::RemoveFont(Font * font)
{
	if (font)
	{
		std::map<uint, Font*>::iterator it = fonts_list.find(font->GetUID());
		if (it != fonts_list.end()) fonts_list.erase(it);
	}
}

std::map<uint, Font*> ModuleResources::GetFontsList() const
{
	return fonts_list;
}

ParticleData * ModuleResources::GetParticleTemplate(UID uid) const
{
	if (particles_list.find(uid) != particles_list.end()) return particles_list.at(uid);
	return nullptr;
}

void ModuleResources::AddParticleTemplate(ParticleData * particle)
{
	if (particle != nullptr)
	{
		particles_list[particle->GetUID()] = particle;
	}
}

void ModuleResources::RemoveParticleTemplate(ParticleData * particle)
{
	if (particle)
	{
		std::map<uint, ParticleData*>::iterator it = particles_list.find(particle->GetUID());
		if (it != particles_list.end()) particles_list.erase(it);
	}
}

std::map<uint, ParticleData*> ModuleResources::GetParticlesList() const
{
	return particles_list;
}

ShaderProgram * ModuleResources::GetShaderProgram(std::string name) const
{
	for (std::map<uint, ShaderProgram*>::const_iterator it = shader_programs_list.begin(); it != shader_programs_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

BlastModel * ModuleResources::GetBlastModel(std::string name) const
{
	for (std::map<uint, BlastModel*>::const_iterator it = blast_models_list.begin(); it != blast_models_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

BlastModel * ModuleResources::GetBlastModel(UID uid) const
{
	if (blast_models_list.find(uid) != blast_models_list.end()) return blast_models_list.at(uid);
	return nullptr;
}

void ModuleResources::AddBlastModel(BlastModel * mesh)
{
	if (mesh != nullptr)
	{
		blast_models_list[mesh->GetUID()] = mesh;
	}
}

void ModuleResources::RemoveBlastModel(BlastModel * mesh)
{
	if (mesh)
	{
		std::map<uint, BlastModel*>::iterator it = blast_models_list.find(mesh->GetUID());
		if (it != blast_models_list.end()) blast_models_list.erase(it);
	}
}

std::map<uint, BlastModel*> ModuleResources::GetBlastModelsList() const
{
	return blast_models_list;
}

SoundBankResource * ModuleResources::GetSoundBank(std::string name) const
{
	for (std::map<uint, SoundBankResource*>::const_iterator it = soundbanks_list.begin(); it != soundbanks_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
}

SoundBankResource * ModuleResources::GetSoundBank(UID uid) const
{
	if (soundbanks_list.find(uid) != soundbanks_list.end()) return soundbanks_list.at(uid);
	return nullptr;
}

void ModuleResources::AddSoundBank(SoundBankResource * sbk)
{
	if (sbk != nullptr)
	{
		soundbanks_list[sbk->GetUID()] = sbk;
	}
}

void ModuleResources::RemoveSoundBank(SoundBankResource * sbk)
{
	if (sbk)
	{
		std::map<uint, SoundBankResource*>::iterator it = soundbanks_list.find(sbk->GetUID());
		if (it != soundbanks_list.end()) soundbanks_list.erase(it);
	}
}

std::map<uint, SoundBankResource*> ModuleResources::GetSoundBanksList() const
{
	return soundbanks_list;
}

ShaderProgram * ModuleResources::GetShaderProgram(UID uid) const
{
	if (shader_programs_list.find(uid) != shader_programs_list.end()) return shader_programs_list.at(uid);
	return nullptr;
}

ShaderProgram * ModuleResources::GetShaderProgram(Shader * vertex, Shader * fragment) const
{
	ShaderProgram* ret = nullptr;
	for (std::map<uint, ShaderProgram*>::const_iterator it = shader_programs_list.begin(); it != shader_programs_list.end(); it++)
	{
		if (it->second->GetVertexShader() == vertex && it->second->GetFragmentShader() == fragment)
		{
			ret = it->second;
			break;
		}
	}
	return ret;
}

void ModuleResources::AddShaderProgram(ShaderProgram * program)
{
	if (program != nullptr)
	{
		shader_programs_list[program->GetUID()] = program;
	}
}

void ModuleResources::RemoveShaderProgram(ShaderProgram * program)
{
	if (program)
	{
		std::map<uint, ShaderProgram*>::iterator it = shader_programs_list.find(program->GetUID());
		if (it != shader_programs_list.end()) shader_programs_list.erase(it);
	}
}

std::map<uint, ShaderProgram*> ModuleResources::GetShaderProgramList() const
{
	return shader_programs_list;
}

void ModuleResources::LoadShaderProgramMeta(std::string path) const
{
	Data d;
	d.LoadJSON(path);
	uint sections = d.GetUInt("num_sections");

	for (int i = 0; i < sections; ++i)
	{
		std::string section_name = "shprogram_" + std::to_string(i);
		if (d.EnterSection(section_name))
		{
			UID program = d.GetUInt("uuid");
			ShaderProgram* shprog = GetShaderProgram(program);
			if (shprog != nullptr)
			{
				UID vert_uid = d.GetUInt("vertex_shader");
				if (vert_uid != 0)
				{
					Shader* vert_shader = GetShader(vert_uid);
					if (vert_shader != nullptr)
					{
						shprog->SetVertexShader(vert_shader);
					}
					else
					{
						CONSOLE_WARNING("Vertex shader %d not found for %s", vert_uid, section_name.c_str());
					}
				}

				UID frag_uid = d.GetUInt("fragment_shader");
				if (frag_uid != 0)
				{
					Shader* frag_shader = GetShader(frag_uid);
					if (frag_shader != nullptr)
					{
						shprog->SetFragmentShader(frag_shader);
					}
					else
					{
						CONSOLE_WARNING("Fragment shader %d not found for %s", frag_uid, section_name.c_str());
					}
				}
			}
			else
			{
				CONSOLE_WARNING("%s not found!", section_name.c_str());
			}

			d.LeaveSection();
		}
	}
}

void ModuleResources::OnShaderUpdate(Shader * shader) const
{
	for (std::map<uint, ShaderProgram*>::const_iterator it = shader_programs_list.begin(); it != shader_programs_list.end(); it++)
	{
		if (it->second->GetVertexShader() == shader || it->second->GetFragmentShader() == shader)
		{
			it->second->LinkShaderProgram(); //if the modified shader affects this program, link the program again
		}
	}
}

GOAPGoal * ModuleResources::GetGOAPGoal(std::string name) const
{
	GOAPGoal* ret = nullptr;
	for (std::map<uint, GOAPGoal*>::const_iterator it = goap_goals_list.begin(); it != goap_goals_list.end(); it++)
	{
		if (it->second->GetName() == name)
		{
			ret = it->second;
			break;
		}
	}
	return ret;
}

GOAPGoal * ModuleResources::GetGOAPGoal(UID uid) const
{
	if (goap_goals_list.find(uid) != goap_goals_list.end()) return goap_goals_list.at(uid);
	return nullptr;
}

void ModuleResources::AddGOAPGoal(GOAPGoal * goal)
{
	if (goal != nullptr)
	{
		goap_goals_list[goal->GetUID()] = goal;
	}
}

void ModuleResources::RemoveGOAPGoal(GOAPGoal * goal)
{
	if (goal)
	{
		std::map<uint, GOAPGoal*>::iterator it = goap_goals_list.find(goal->GetUID());
		if (it != goap_goals_list.end())
		{
			RELEASE(it->second);
			goap_goals_list.erase(it);
		}
	}
}

std::map<uint, GOAPGoal*> ModuleResources::GetGOAPGoalList() const
{
	return goap_goals_list;
}

GOAPAction * ModuleResources::GetGOAPAction(std::string name) const
{
	GOAPAction* ret = nullptr;
	for (std::map<uint, GOAPAction*>::const_iterator it = goap_actions_list.begin(); it != goap_actions_list.end(); it++)
	{
		if (it->second->GetName() == name)
		{
			ret = it->second;
			break;
		}
	}
	return ret;
}

GOAPAction * ModuleResources::GetGOAPAction(UID uid) const
{
	if (goap_actions_list.find(uid) != goap_actions_list.end()) return goap_actions_list.at(uid);
	return nullptr;
}

void ModuleResources::AddGOAPAction(GOAPAction * action)
{
	if (action != nullptr)
	{
		goap_actions_list[action->GetUID()] = action;
	}
}

void ModuleResources::RemoveGOAPGoal(GOAPAction * action)
{
	if (action)
	{
		std::map<uint, GOAPAction*>::iterator it = goap_actions_list.find(action->GetUID());
		if (it != goap_actions_list.end())
		{
			RELEASE(it->second);
			goap_actions_list.erase(it);
		}
	}
}

std::map<uint, GOAPAction*> ModuleResources::GetGOAPActionList() const
{
	return goap_actions_list;
}

std::vector<std::string> ModuleResources::GetSceneList() const
{
	return scene_list;
}

Resource::ResourceType ModuleResources::AssetExtensionToResourceType(std::string str)
{
	if (str == ".jpg" || str == ".png" || str == ".tga" || str == ".dds") return Resource::TextureResource;
	else if (str == ".fbx" || str == ".FBX") return Resource::MeshResource;
	else if (str == ".bmesh") return Resource::BlastMeshResource;
	else if (str == ".cs" || str == ".lua") return Resource::ScriptResource;
	else if (str == ".wav" || str == ".ogg") return Resource::AudioResource;
	else if (str == ".jprefab") return Resource::PrefabResource;
	else if (str == ".mat") return Resource::MaterialResource;
	else if (str == ".pmat") return Resource::PhysicsMatResource;
	else if (str == ".animation") return Resource::AnimationResource;
	else if (str == ".particle") return Resource::ParticleFXResource;
	else if (str == ".scene" || str == ".jscene") return Resource::SceneResource;
	else if (str == ".ttf") return Resource::FontResource;
	else if (str == ".vshader") return Resource::ShaderResource;
	else if (str == ".fshader") return Resource::ShaderResource;
	else if (str == ".bnk") return Resource::SoundBankResource;

	return Resource::Unknown;
}

Resource::ResourceType ModuleResources::LibraryExtensionToResourceType(std::string str)
{
	if (str == ".dds") return Resource::TextureResource;
	else if (str == ".mesh") return Resource::MeshResource;
	else if (str == ".bmesh") return Resource::BlastMeshResource;
	else if (str == ".scene" || str == ".jscene") return Resource::SceneResource;
	else if (str == ".mat") return Resource::MaterialResource;
	else if (str == ".pmat") return Resource::PhysicsMatResource;
	else if (str == ".particle") return Resource::ParticleFXResource;
	else if (str == ".dll") return Resource::ScriptResource;
	else if (str == ".jprefab" || str == ".fbx" || str == ".FBX") return Resource::PrefabResource;
	else if (str == ".vshader") return Resource::ShaderResource;
	else if (str == ".fshader") return Resource::ShaderResource;
	else if (str == ".ttf") return Resource::FontResource;
	else if (str == ".bnk") return Resource::SoundBankResource;

	return Resource::Unknown;
}

std::string ModuleResources::ResourceTypeToLibraryExtension(Resource::ResourceType type)
{
	if (type == Resource::TextureResource) return ".dds";
	else if (type == Resource::MeshResource) return ".mesh";
	else if (type == Resource::BlastMeshResource) return ".bmesh";
	else if (type == Resource::SceneResource) return ".scene";
	else if (type == Resource::ParticleFXResource) return ".particle";
	else if (type == Resource::PrefabResource) return ".prefab";
	else if (type == Resource::MaterialResource) return ".mat";
	else if (type == Resource::PhysicsMatResource) return ".pmat";
	else if (type == Resource::ScriptResource) return ".dll";
	else if (type == Resource::ShaderResource) return ".shader";
	else if (type == Resource::FontResource) return ".ttf";
	
	return "";
}

bool ModuleResources::HasMetaFile(std::string file_path)
{
	return App->file_system->FileExist(file_path + ".meta");
}

std::string ModuleResources::GetLibraryFile(std::string file_path)
{
	std::string extension = App->file_system->GetFileExtension(file_path);

	Resource::ResourceType type = AssetExtensionToResourceType(extension);

	if (extension == ".fbx" || extension == ".FBX") type = Resource::PrefabResource;
	else if (extension == ".bnk") type == Resource::SoundBankResource;

	std::string library_file;
	std::string directory;
	std::string file_name = App->file_system->GetFileNameWithoutExtension(file_path);

	switch (type)
	{
	case Resource::TextureResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_TEXTURES_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".dds", directory, false))
		{
			library_file = directory + file_name + ".dds";
		}
		break;
	case Resource::MeshResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_MESHES_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".mesh", directory, false))
		{
			library_file = directory + file_name + ".mesh";
		}
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_PREFABS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".prefab", directory, false))
		{
			library_file = directory + file_name + ".prefab";
		}
		break;
	case Resource::ScriptResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_SCRIPTS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".dll", directory, false))
		{
			library_file = directory + file_name + ".dll";
		}
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:
		break;
	case Resource::FontResource:
		break;
	case Resource::MaterialResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_MATERIALS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".mat", directory, false))
		{
			library_file = directory + file_name + ".mat";
		}
		break;
	case Resource::PhysicsMatResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_PHYS_MATS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".pmat", directory, false))
		{
			library_file = directory + file_name + ".pmat";
		}
		break;
	case Resource::BlastMeshResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_BMODEL_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".bmodel", directory, false))
		{
			library_file = directory + file_name + ".bmodel";
		}
		break;
	case Resource::ShaderResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_SHADERS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".shader", directory, false))
		{
			library_file = directory + file_name + ".shader";
		}
		break;
	case Resource::SoundBankResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_SOUNDBANK_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".bnk", directory, false))
		{
			library_file = directory + file_name + ".bnk";
		}
		break;
	case Resource::Unknown:
		break;
	default:
		break;
	}

	return library_file;
}

std::string ModuleResources::CreateLibraryFile(Resource::ResourceType type, std::string file_path)
{
	std::string ret;

	if (!App->file_system->DirectoryExist(LIBRARY_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_FOLDER_PATH);

	switch (type)
	{
	case Resource::TextureResource:
		if (!App->file_system->DirectoryExist(LIBRARY_TEXTURES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_TEXTURES_FOLDER_PATH);
		ret = App->texture_importer->ImportTexture(file_path);
		break;
	case Resource::MeshResource:
		if (!App->file_system->DirectoryExist(LIBRARY_MESHES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MESHES_FOLDER_PATH);
		ret = App->mesh_importer->ImportMesh(file_path);
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		if (!App->file_system->DirectoryExist(LIBRARY_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PREFABS_FOLDER_PATH);
		ret = App->prefab_importer->ImportPrefab(file_path);
		break;
	case Resource::ScriptResource:
		if (!App->file_system->DirectoryExist(LIBRARY_SCRIPTS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SCRIPTS_FOLDER_PATH);
		ret = App->script_importer->ImportScript(file_path);
		break;
	case Resource::AudioResource:
		break;
	case Resource::ParticleFXResource:
		if (!App->file_system->DirectoryExist(LIBRARY_PARTICLES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PARTICLES_FOLDER_PATH);
		ret = App->particle_importer->ImportTemplate(file_path);
		break;
	case Resource::FontResource:
		if (!App->file_system->DirectoryExist(LIBRARY_FONTS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_FONTS_FOLDER_PATH);
		ret = App->font_importer->ImportFont(file_path);
		break;
	case Resource::MaterialResource:
		if (!App->file_system->DirectoryExist(LIBRARY_MATERIALS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MATERIALS_FOLDER_PATH);
		ret = App->material_importer->ImportMaterial(file_path);
		break;
	case Resource::PhysicsMatResource:
		if (!App->file_system->DirectoryExist(LIBRARY_PHYS_MATS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PHYS_MATS_FOLDER_PATH);
		ret = App->phys_mats_importer->ImportPhysicsMaterial(file_path);
		break;
	case Resource::BlastMeshResource:
		if (!App->file_system->DirectoryExist(LIBRARY_MESHES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MESHES_FOLDER_PATH);
		ret = App->blast_mesh_importer->ImportModel(file_path);
		break;
	case Resource::ShaderResource:
		if (!App->file_system->DirectoryExist(LIBRARY_SHADERS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SHADERS_FOLDER_PATH);
		ret = App->shader_importer->ImportShader(file_path);
		break;
	case Resource::SoundBankResource:
		if (!App->file_system->DirectoryExist(LIBRARY_SOUNDBANK_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SOUNDBANK_FOLDER_PATH);
		ret = App->audio_importer->ImportSoundBank(file_path);
		break;
	case Resource::Unknown:
		break;
	default:
		break;
	}

	return ret;
}

Resource * ModuleResources::CreateResourceFromLibrary(std::string library_path)
{
	std::string extension = App->file_system->GetFileExtension(library_path);
	std::string name = App->file_system->GetFileNameWithoutExtension(library_path);
	Resource::ResourceType type = LibraryExtensionToResourceType(extension);
	Resource* resource = nullptr;

	switch (type)
	{
	case Resource::TextureResource:
		if (GetTexture(name) != nullptr)
		{
			resource = (Resource*)GetTexture(name);
			break;
		}
		resource = (Resource*)App->texture_importer->LoadTextureFromLibrary(library_path);
		break;
	case Resource::MeshResource:
		if (GetMesh(name) != nullptr)
		{
			resource = (Resource*)GetMesh(name); 
			break;
		}
		resource = (Resource*)App->mesh_importer->LoadMeshFromLibrary(library_path);
		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		if (GetPrefab(name) != nullptr)
		{
			resource = (Resource*)GetPrefab(name);
			break;
		}
		resource = (Resource*)App->prefab_importer->LoadPrefabFromLibrary(library_path);
		break;
	case Resource::ScriptResource:
		if (GetScript(name) != nullptr)
		{
			resource = (Resource*)GetScript(name);
			break;
		}
		resource = (Resource*)App->script_importer->LoadScriptFromLibrary(library_path);
		break;
	case Resource::AudioResource:

		break;
	case Resource::ParticleFXResource:
		if (GetParticleTemplate(name) != nullptr)
		{
			resource = (Resource*)GetParticleTemplate(name);
			break;
		}
		resource = (Resource*)App->particle_importer->LoadTemplateFromLibrary(library_path);
		break;
	case Resource::FontResource:
		resource = (Resource*)GetFont(name);

		if (resource == nullptr)
			resource = (Resource*)App->font_importer->LoadFontFromLibrary(library_path);

		break;
	case Resource::MaterialResource:
		if (GetMaterial(name) != nullptr)
		{
			resource = (Resource*)GetMaterial(name);
			break;
		}
		resource = (Resource*)App->material_importer->LoadMaterialFromLibrary(library_path);
		break;
	case Resource::PhysicsMatResource:
		if (GetPhysMaterial(name) != nullptr)
		{
			resource = (Resource*)GetPhysMaterial(name);
			break;
		}
		resource = (Resource*)App->phys_mats_importer->LoadPhysicsMaterialFromLibrary(library_path);
		break;
	case Resource::ShaderResource:
		if (GetShader(name) != nullptr)
		{
			resource = (Resource*)GetShader(name);
			break;
		}
		resource = (Resource*)App->shader_importer->LoadShaderFromLibrary(library_path);
		break;
	case Resource::Unknown:
		break;
	case Resource::BlastMeshResource:
		if (GetBlastModel(name) != nullptr)
		{
			resource = (Resource*)GetBlastModel(name);
			break;
		}
		resource = (Resource*)App->blast_mesh_importer->LoadModelFromLibrary(library_path, false);
		break;
	case Resource::SoundBankResource:
		if (GetSoundBank(name) != nullptr)
		{
			resource = (Resource*)GetSoundBank(name);
			break;
		}
		resource = (Resource*)App->audio_importer->LoadSoundBankFromLibrary(library_path);
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		AddResource(resource);
	}

	return resource;
}

void ModuleResources::CreateResource(std::string file_path)
{
	std::string extension = App->file_system->GetFileExtension(file_path);
	std::string library_path;
	if (AssetExtensionToResourceType(extension) == Resource::ResourceType::Unknown) return;
	Resource* resource = nullptr;

	if (HasMetaFile(file_path))
	{
		Data data;
		if (data.LoadJSON(file_path + ".meta"))
		{
			library_path = data.GetString("Library_path");
			Resource::ResourceType type = (Resource::ResourceType)data.GetInt("Type");
			if (!App->file_system->FileExist(library_path))
			{
				if (extension == ".fbx" || extension == ".FBX") type = Resource::MeshResource;
				library_path = CreateLibraryFile(type, file_path);
			}
			else
			{
				if (extension == ".cs")
				{
					if (App->file_system->CompareFilesTime(file_path, library_path))
					{
						App->script_importer->ImportScript(file_path);
					}
				}
				if (extension == ".vshader" || extension == ".vfshader")
				{
					if (App->file_system->CompareFilesTime(file_path, library_path))
					{
						App->shader_importer->ImportShader(file_path);
					}
				}
				if (extension == ".bnk")
				{
					if (App->file_system->CompareFilesTime(file_path, library_path))
					{
						App->audio_importer->ImportSoundBank(file_path);
					}
				}
				if (extension == ".bmesh")
				{
					std::string model_name = App->file_system->GetFileNameWithoutExtension(library_path);

					if (!App->file_system->FileExist(LIBRARY_BMODEL_FOLDER + model_name + ".bmodel"))
					{
						library_path = CreateLibraryFile(type, file_path);
					}
				}
			}
			resource = CreateResourceFromLibrary(library_path);
			if (resource != nullptr)
			{
				resource->SetAssetsPath(file_path);
			}
		}
	}
	else if (GetLibraryFile(file_path) != "")
	{
		std::string path = GetLibraryFile(file_path);
		if (!App->IsGame())
		{
			if (extension == ".cs")
			{
				if (App->file_system->CompareFilesTime(file_path, path))
				{
					App->script_importer->ImportScript(file_path);
				}
			}
			if (extension == ".vshader" || extension == ".fshader")
			{
				if (App->file_system->CompareFilesTime(file_path, path))
				{
					App->shader_importer->ImportShader(file_path);
				}
			}
			if (extension == ".bnk")
			{
				if (App->file_system->CompareFilesTime(file_path, library_path))
				{
					App->audio_importer->ImportSoundBank(file_path);
				}
			}
		}
		resource = CreateResourceFromLibrary(path);

		if (!App->IsGame())
		{
			if (resource != nullptr)
			{
				resource->SetAssetsPath(file_path);
				if (!HasMetaFile(file_path)) resource->CreateMeta();
			}
		}
	}
	else
	{
		Resource::ResourceType type = AssetExtensionToResourceType(extension);
		library_path = CreateLibraryFile(type, file_path);
		resource = CreateResourceFromLibrary(library_path);
		if (resource != nullptr)
		{
			resource->SetAssetsPath(file_path);
			resource->CreateMeta();
		}
	}
}

void ModuleResources::DeleteResource(std::string file_path)
{
	std::string extension = App->file_system->GetFileExtension(file_path);
	Resource::ResourceType type = LibraryExtensionToResourceType(extension);
	std::string resource_name = App->file_system->GetFileNameWithoutExtension(file_path);

	Texture* texture = nullptr;
	Prefab* prefab = nullptr;
	Material* material = nullptr;
	PhysicsMaterial* phys_material = nullptr;
	Shader* shader = nullptr;
	SoundBankResource* sbk = nullptr;

	if (extension == ".fbx" || extension == ".FBX") type = Resource::PrefabResource;
	else if (extension == ".vshader" || extension == ".fshader") type = Resource::ShaderResource;
	else if (extension == ".bnk") type = Resource::SoundBankResource;

	switch (type)
	{
	case Resource::TextureResource:
		texture = GetTexture(resource_name);
		if (texture != nullptr)
		{
			App->file_system->Delete_File(texture->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemoveTexture(texture);
		}
		break;
	case Resource::MeshResource:

		break;
	case Resource::SceneResource:
		break;
	case Resource::AnimationResource:
		break;
	case Resource::PrefabResource:
		prefab = GetPrefab(resource_name);
		if (prefab != nullptr)
		{
			App->file_system->Delete_File(prefab->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemovePrefab(prefab);
			if (extension == ".fbx" || extension == ".FBX")
			{
				DeleteFBXMeshes(prefab->GetRootGameObject());
			}
		}
		break;
	case Resource::ScriptResource:
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
		break;
	case Resource::MaterialResource:
		material = GetMaterial(resource_name);
		if (material != nullptr)
		{
			App->file_system->Delete_File(material->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemoveMaterial(material);
		}
		break;
	case Resource::PhysicsMatResource:
		phys_material = GetPhysMaterial(resource_name);
		if (phys_material != nullptr)
		{
			App->file_system->Delete_File(phys_material->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemovePhysMaterial(phys_material);
		}
		break;
	case Resource::ShaderResource:
		shader = GetShader(resource_name);
		if (shader != nullptr)
		{
			App->file_system->Delete_File(shader->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemoveShader(shader);
		}
		break;
	case Resource::Unknown:
		break;
	case Resource::SoundBankResource:
		sbk = GetSoundBank(resource_name);
		if (sbk != nullptr)
		{
			App->file_system->Delete_File(sbk->GetLibraryPath());
			App->file_system->Delete_File(file_path + ".meta");
			RemoveSoundBank(sbk);
		}
	default:
		break;
	}
}

void ModuleResources::DeleteFBXMeshes(GameObject* gameobject)
{
	if (gameobject != nullptr)
	{
		for (std::list<GameObject*>::iterator it = gameobject->childs.begin(); it != gameobject->childs.end(); it++)
		{
			DeleteFBXMeshes(*it);
		}
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)gameobject->GetComponent(Component::CompMeshRenderer);

		if (mesh_renderer != nullptr)
		{
			Mesh* mesh = mesh_renderer->GetMesh();
			if (mesh != nullptr)
			{
				App->file_system->Delete_File(mesh->GetLibraryPath());
				RemoveMesh(mesh);
			}
		}

		/*ComponentBlastMeshRenderer* blast_mesh_renderer = (ComponentBlastMeshRenderer*)gameobject->GetComponent(Component::CompMeshRenderer);

		if (blast_mesh_renderer != nullptr)
		{
			BlastMesh* blast_mesh = blast_mesh_renderer->GetMesh();
			if (blast_mesh != nullptr)
			{
				App->file_system->Delete_File(blast_model->GetLibraryPath());
				RemoveBlastMesh(blast_mesh);
			}
		}*/
	}
}

void ModuleResources::CreateDefaultShaders()
{
	CONSOLE_DEBUG("-------------- Creating Default Shaders -------------");
	if (!App->file_system->DirectoryExist(SHADER_DEFAULT_FOLDER_PATH)) App->file_system->Create_Directory(SHADER_DEFAULT_FOLDER_PATH);

	//Default Shader

	std::string vert_default_path = SHADER_DEFAULT_FOLDER "default_vertex.vshader";
	if (!App->file_system->FileExist(vert_default_path))
	{
		Shader* default_vert = new Shader();
		default_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);
		
		std::string shader_text = 
		"#version 400 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 texCoord;\n"
		"layout(location = 2) in vec3 normals;\n"
		"layout(location = 3) in vec4 color;\n\n"
		"layout(location = 4) in vec3 tangents;\n"
		"layout(location = 5) in vec3 bitangents;\n"
		"out vec4 ourColor;\n"
		"out vec3 Normal;\n"
		"out vec2 TexCoord;\n"
		"out vec3 FragPos;\n\n"
		"out vec3 TangentFragPos;\n"
		"out mat3 TBN;\n"
		"uniform mat4 Model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n\n"
		"void main()\n"
		"{ \n"
		"	gl_Position = projection * view * Model * vec4(position, 1.0f);\n"
		"	FragPos = vec3(Model * vec4(position, 1.0));"
		"	Normal = mat3(transpose(inverse(Model))) * normals;\n"
		"	mat3 normalMatrix = transpose(inverse(mat3(Model)));\n"
		"	vec3 T = normalize(normalMatrix * tangents);\n"
		"	vec3 N = normalize(normalMatrix * normals);\n"
		"	T = normalize(T - dot(T, N) * N);\n"
		"	vec3 B = cross(N, T);\n"
		"	TBN = transpose(mat3(T, B, N));\n"
		"	TangentFragPos = TBN * FragPos;\n"
		"	ourColor = color;\n"
		"	TexCoord = texCoord.xy;\n"
		"}";

		default_vert->SetContent(shader_text);
		std::ofstream outfile(vert_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_vert);
	}
	CreateResource(vert_default_path);

	std::string frag_default_path = SHADER_DEFAULT_FOLDER "default_fragment.fshader";
	if (!App->file_system->FileExist(frag_default_path))
	{
		Shader* default_frag = new Shader();
		default_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 400 core\n\n"

			"in vec4 ourColor;\n"
			"in vec3 Normal;\n"
			"in vec3 FragPos;\n"
			"in vec2 TexCoord;\n"
			"in vec3 TangentFragPos;\n"
			"in mat3 TBN;\n"
			"out vec4 color;\n\n"

			"uniform bool has_material_color;\n"
			"uniform vec4 material_color;\n"
			"uniform bool has_texture;\n"
			"uniform bool has_texture2;\n"
			"uniform bool is_ui;\n"
			"uniform bool has_normalmap;\n\n"
			"uniform bool has_opacity;"
			"uniform bool has_light;\n"

			"uniform bool own_uvs_diffuse;\n"
			"uniform bool own_uvs_diffuse2;\n"
			"uniform bool own_uvs_normalmap;\n"

			"uniform sampler2D Tex_Diffuse;\n\n"
			"uniform sampler2D Tex_Diffuse2;\n\n"
			"uniform sampler2D Tex_NormalMap;\n\n"
			"uniform sampler2D Tex_Opacity;\n"

			"uniform vec2 Tex_Diffuse_UV;\n"
			"uniform vec2 Tex_Diffuse2_UV;\n"
			"uniform vec2 Tex_NormalMap_UV;\n"
			"uniform vec2 Tex_Opacity_UV;\n"

			"struct DirLight {\n"
			"	vec3 direction;\n\n"

			"	vec3 ambient;\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n\n"

			"	vec4 color;\n"
			"	bool active;\n"
			"};\n\n"

			"struct PointLight {\n"
			"	vec3 position;\n\n"

			"	float constant;\n"
			"	float linear;\n"
			"	float quadratic;\n\n"

			"	vec3 ambient;\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n\n"

			"	vec4 color;\n"
			"	bool active;\n"
			"};\n"

			"struct SpotLight {\n"
			"	vec3 position;\n"
			"	vec3 direction;\n\n"
			"	float cutOff;\n"
			"	float outerCutOff;\n\n"

			"	float constant;\n"
			"	float linear;\n"
			"	float quadratic;\n\n"

			"	vec3 ambient;\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n"
			"	vec4 color;\n"
			"	bool active;\n"
			"};\n\n"

			"#define NR_POINT_LIGHTS 8\n"
			"#define NR_DIREC_LIGHTS 2\n"
			"#define NR_SPOT_LIGHTS 8\n\n"
			"#define AMBIENT_LIGHT 0.35\n"

			"uniform vec3 viewPos;\n"
			"uniform DirLight dirLights[NR_DIREC_LIGHTS];\n"
			"uniform PointLight pointLights[NR_POINT_LIGHTS];\n"
			"uniform SpotLight spotLights[NR_SPOT_LIGHTS];\n"

			"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
			"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
			"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n\n"
			"void ApplyOpacity();\n"
			"void SetFragmentColor();\n"
			"void main()\n"
			"{\n"
			"	SetFragmentColor();\n"
			"	if (is_ui == false)\n"
			"	{\n"
			"		vec3 normal; vec3 viewDir;  vec3 fragPosarg;\n"

			"		if (has_normalmap)\n"
			"		{\n"
			"			if (own_uvs_normalmap == false)\n"
			"			normal = normalize(texture(Tex_NormalMap, TexCoord).rgb * 2.0 - 1.0);\n"
			"			else\n"
			"			normal = normalize(texture(Tex_NormalMap, TexCoord * Tex_NormalMap_UV).rgb * 2.0 - 1.0);\n"
			"			vec3 TangentViewPos = TBN * viewPos;\n"
			"			viewDir = normalize(TangentViewPos - TangentFragPos);\n"
			"			fragPosarg = TangentFragPos;\n"
			"		}\n"
			"		else\n"
			"		{\n"
			"			normal = normalize(Normal);\n"
			"			viewDir = normalize(viewPos - FragPos);\n"
			"			fragPosarg = FragPos;\n"
			"		}\n"
			"			vec3 result = vec3(0.0, 0.0, 0.0); \n"
			"			for (int i = 0; i < NR_DIREC_LIGHTS; i++)\n"
			"				result += CalcDirLight(dirLights[i], normal, viewDir);\n"
			"			for (int k = 0; k < NR_POINT_LIGHTS; k++)\n"
			"				result += CalcPointLight(pointLights[k], normal, fragPosarg, viewDir);\n"
			"			for (int j = 0; j < NR_SPOT_LIGHTS; j++)\n"
			"				result += CalcSpotLight(spotLights[j], normal, fragPosarg, viewDir);\n"
			"			if (has_light)\n"
			"				color = vec4((color.rgb * AMBIENT_LIGHT + result), 1.0);  \n"	
			"			else color = vec4((color.rgb), 1.0);\n"
			"			if (has_opacity)"
			"			{\n"
			"				ApplyOpacity();\n"
						"}\n"
			"}\n"

			"}\n\n"

			"	vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n"
			"{\n"
			"	if (light.active == true)\n"
			"{\n"
			"	vec3 lightDir;\n"
			"	if (has_normalmap)\n"
			"	{\n"
			"		lightDir = normalize(-TBN * light.direction);\n"
			"	}\n"
			"	else\n"
			"		lightDir = (-light.direction);\n"
			"		float diff = max(dot(lightDir, normal), 0.0);\n"
			"		vec3 reflectDir = reflect(-lightDir, normal);\n"
			"		vec3 halfwayDir = normalize(lightDir + viewDir);\n"
			"		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);\n"
			"		vec3 ambient = light.ambient * vec3(color);\n"
			"		vec3 diffuse = light.diffuse * diff * vec3(color);\n"
			"		vec3 specular = light.specular * spec;\n"
			"		return (ambient + diffuse + specular) * vec3(light.color);\n"
			"}\n"
			"else\n"
			"	return vec3(0.0, 0.0, 0.0);\n"
			"}\n"

			"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
			"{\n"
			"	if (light.active == true)\n"
			"	{\n"
			"	vec3 lightPos; vec3 lightDir;\n"
			"	if (has_normalmap)\n"
			"	{\n"
			"		lightPos = TBN * light.position;\n"
			"		lightDir = normalize(lightPos - fragPos);"
			"	}\n"
			"	else\n"
			"{\n"
			"		lightPos = light.position;\n"
			"		lightDir = normalize(light.position - fragPos);\n\n"
			"}\n"
			"		float diff = max(dot(lightDir, normal), 0.0);\n\n"

			"		vec3 reflectDir = reflect(-lightDir, normal);\n"
			"		vec3 halfwayDir = normalize(lightDir + viewDir);	\n"
			"		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);\n"

			"		float distance = length(lightPos - fragPos);\n"
			"		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n\n"

			"		vec3 ambient = light.ambient * vec3(color);\n"
			"		vec3 diffuse = light.diffuse * diff * vec3(color);\n"
			"		vec3 specular = light.specular * spec;\n"
			"		ambient *= attenuation;\n"
			"		diffuse *= attenuation;\n"
			"		specular *= attenuation;\n"
			"		return (ambient + diffuse + specular) * vec3(light.color);\n"
			"	}\n"
			"	else\n"
			"		return vec3(0.0, 0.0, 0.0);\n"
			"}\n\n"

			"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
			"{\n"
			"	if (light.active == true)\n"
			"	{\n"
			"	vec3 lightPos; vec3 lightDir;\n"
			"	if (has_normalmap)\n"
			"	{\n"
			"		lightPos = TBN * light.position;\n"
			"		lightDir = normalize(lightPos - fragPos);\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		lightPos = light.position;\n"
			"		lightDir = normalize(light.position - fragPos);\n"
			"	}\n"
			"		float diff = max(dot(lightDir, normal), 0.0);\n\n"

			"		vec3 reflectDir = reflect(-lightDir, normal);\n"
			"		vec3 halfwayDir = normalize(lightDir + viewDir);\n"
			"		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);\n\n"

			"		float distance = length(lightPos - fragPos);\n"
			"		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n\n"

			"		float theta = dot(lightDir, normalize(-light.direction));\n"
			"		float epsilon = light.cutOff - light.outerCutOff;\n"
			"		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n\n"

			"		vec3 ambient = light.ambient * vec3(color);\n"
			"		vec3 diffuse = light.diffuse * diff * vec3(color);\n"
			"		vec3 specular = light.specular * spec;\n"
			"		ambient *= attenuation * intensity;\n"
			"		diffuse *= attenuation * intensity;\n"
			"		specular *= attenuation * intensity;\n"
			"		return (ambient + diffuse + specular) * vec3(light.color);\n"
			"	}\n"
			"	else\n"
			"		return vec3(0.0, 0.0, 0.0);\n"
			"}\n"
			"void SetFragmentColor()\n"
			"{\n"
			"	if (has_texture)\n"
			"	{\n"
			"		if (own_uvs_diffuse == false)\n"	
			"		color = texture(Tex_Diffuse, TexCoord);\n"
			"		else\n"
			"		color = texture(Tex_Diffuse, TexCoord * Tex_Diffuse_UV);\n"
			"	}\n"
			"	else if (has_material_color)\n"
			"	color = material_color;\n"
			"	else\n"
			"	color = ourColor;\n"
			"	if (has_texture2)\n"
			"	{\n"
			"	vec4 diffuse2 = vec4(0.0, 0.0, 0.0, 0.0);"
			"		if (own_uvs_diffuse2 == false)\n"
			"		diffuse2 = texture(Tex_Diffuse2, TexCoord); \n"
			"		else\n"
			"		diffuse2 = texture(Tex_Diffuse2, TexCoord * Tex_Diffuse2_UV);\n"
			"		if (diffuse2.a > 0.1) color = diffuse2;"
			"	}\n"
			"}\n"
			"void ApplyOpacity()\n"
			"{\n"
				"vec4 opacity = texture(Tex_Opacity, TexCoord);\n"
				"color.a = opacity.r;\n"
			"}\n"				
				;

		default_frag->SetContent(shader_text);
		std::ofstream outfile(frag_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_frag);
	}
	CreateResource(frag_default_path);

	ShaderProgram* prog = new ShaderProgram();
	prog->SetName("default_shader_program");

	Shader* vertex = GetShader("default_vertex");
	prog->SetVertexShader(vertex);

	Shader* fragment = GetShader("default_fragment");	
	prog->SetFragmentShader(fragment);

	//Particles Shader
	std::string vert_particle_default_path = SHADER_DEFAULT_FOLDER "default_particle_vertex.vshader";
	if (!App->file_system->FileExist(vert_particle_default_path))
	{
		Shader* default_particle_vert = new Shader();
		default_particle_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec3 texCoord;\n"
			"layout(location = 2) in vec3 normals;\n"
			"layout(location = 3) in vec4 color;\n\n"
			"out vec4 ourColor;\n"
			"out vec3 Normal;\n"
			"out vec2 TexCoord;\n\n"
			"uniform mat4 Model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n\n"
			"void main()\n"
			"{ \n"
			"	gl_Position = projection * view * Model * vec4(position, 1.0f);\n"
			"	ourColor = color;\n"
			"	TexCoord = texCoord.xy;\n"
			"}";

		default_particle_vert->SetContent(shader_text);
		std::ofstream outfile(vert_particle_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_particle_vert);
	}
	CreateResource(vert_particle_default_path);

	std::string frag_particle_default_path = SHADER_DEFAULT_FOLDER "default_particle_fragment.fshader";
	if (!App->file_system->FileExist(frag_particle_default_path))
	{
		Shader* default_particle_frag = new Shader();
		default_particle_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 330 core\n"
			"in vec4 ourColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n\n"
			"out vec4 color;\n\n"
			"uniform bool has_material_color;\n"
			"uniform vec4 material_color;\n"
			"uniform bool alpha_interpolation;\n"
			"uniform float alpha_percentage;\n"
			"uniform bool has_texture;\n"
			"uniform sampler2D ourTexture;\n\n"
			"void main()\n"
			"{\n"
			"	if(has_texture)\n"
			"	{\n"
			"		color = texture(ourTexture, TexCoord);\n"
			"		color.rgb = color.rgb + material_color.rgb;\n"
			"	}\n"
			"	else if(has_material_color)\n"
			"		color = material_color;\n"
			"	else\n"
			"		color = ourColor;\n"

			"   if(color.a < 0.1f)\n"
			"		discard;\n"

			"	if(alpha_interpolation)\n"
			"		color.a = alpha_percentage;\n"

			"}";

		default_particle_frag->SetContent(shader_text);
		std::ofstream outfile(frag_particle_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_particle_frag);
	}
	CreateResource(frag_particle_default_path);

	ShaderProgram* part_prog = new ShaderProgram();
	part_prog->SetName("default_particle_program");

	Shader* part_vertex = GetShader("default_particle_vertex");
	part_prog->SetVertexShader(part_vertex);

	Shader* part_fragment = GetShader("default_particle_fragment");
	part_prog->SetFragmentShader(part_fragment);
	
	part_prog->LinkShaderProgram(); 
	prog->LinkShaderProgram();

	AddResource(prog);
	AddResource(part_prog); 
	prog->LinkShaderProgram();

	AddResource(prog);

	//Create a grid fragment shader
	std::string frag_grid_path = SHADER_DEFAULT_FOLDER "grid_fragment.fshader";
	if (!App->file_system->FileExist(frag_grid_path))
	{
		Shader* grid_frag = new Shader();
		grid_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 400 core\n"
			"in vec4 ourColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n\n"
			"out vec4 color;\n\n"
			"uniform vec4 line_color;\n\n"
			"void main()\n"
			"{\n"
			"	float x,y;\n"
			"	x = fract(TexCoord.x*25.0);\n"
			"	y = fract(TexCoord.y*25.0);\n\n"
			"	// Draw a black and white grid.\n"
			"	if (x > 0.93 || y > 0.93) \n	{\n"
			"	color = line_color;\n	}\n"
			"	else\n{\n"
			"	discard;\n	}\n"
			"}";

		grid_frag->SetContent(shader_text);
		std::ofstream outfile(frag_grid_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(grid_frag);
	}
	CreateResource(frag_grid_path);

	prog = new ShaderProgram();
	prog->SetName("grid_shader_program");

	vertex = GetShader("default_vertex");
	prog->SetVertexShader(vertex);

	fragment = GetShader("grid_fragment");
	prog->SetFragmentShader(fragment);

	prog->LinkShaderProgram();

	AddResource(prog);

	// cubemap shader
	std::string vert_cubemap_path = SHADER_DEFAULT_FOLDER "cubemap_vertex.vshader";
	if (!App->file_system->FileExist(vert_cubemap_path))
	{
		Shader* cubemap_vert = new Shader();
		cubemap_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 400 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec3 texCoord;\n"
			"layout(location = 2) in vec3 normals;\n"
			"layout(location = 3) in vec4 color;\n\n"
			"out vec4 ourColor;\n"
			"out vec3 Normal;\n"
			"out vec3 TexCoord;\n\n"
			"uniform mat4 Model; \n"
			"uniform mat4 view; \n"
			"uniform mat4 projection;\n\n "
			"void main()\n"
			"{ \n"
			"	gl_Position = projection * view * Model * vec4(position, 1.0f);\n"
			"	ourColor = color;\n"
			"	TexCoord = -position;\n"
			"}";

		cubemap_vert->SetContent(shader_text);
		std::ofstream outfile(vert_cubemap_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(cubemap_vert);
	}
	CreateResource(vert_cubemap_path);

	std::string frag_cubemap_path = SHADER_DEFAULT_FOLDER "cubemap_fragment.fshader";
	if (!App->file_system->FileExist(frag_cubemap_path))
	{
		Shader* cubemap_frag = new Shader();
		cubemap_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 400 core\n"
			"in vec4 ourColor;\n"
			"in vec3 Normal;\n"
			"in vec3 TexCoord;\n\n"
			"out vec4 color;\n\n"
			"uniform samplerCube ourTexture;\n\n"
			"void main()\n"
			"{\n"
			"	color = texture(ourTexture, TexCoord);\n"
			"}";

		cubemap_frag->SetContent(shader_text);
		std::ofstream outfile(frag_cubemap_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(cubemap_frag);
	}
	CreateResource(frag_cubemap_path);

	prog = new ShaderProgram();
	prog->SetName("cubemap_shader_program");

	vertex = GetShader("cubemap_vertex");
	prog->SetVertexShader(vertex);

	fragment = GetShader("cubemap_fragment");
	prog->SetFragmentShader(fragment);

	prog->LinkShaderProgram();

	AddResource(prog);

	//Default Debug Shaders
	std::string deb_vert_default_path = SHADER_DEFAULT_FOLDER "default_debug_vertex.vshader";
	if (!App->file_system->FileExist(deb_vert_default_path))
	{
		Shader* default_deb_vert = new Shader();
		default_deb_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;\n"
			"uniform mat4 Model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n\n"
			"void main()\n"
			"{ \n"
			"	gl_Position = projection * view * Model * vec4(position, 1.0f);\n"
			"}";

		default_deb_vert->SetContent(shader_text);
		std::ofstream outfile(deb_vert_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_deb_vert);
	}
	CreateResource(deb_vert_default_path);

	std::string deb_frag_default_path = SHADER_DEFAULT_FOLDER "default_debug_fragment.fshader";
	if (!App->file_system->FileExist(deb_frag_default_path))
	{
		Shader* default_deb_frag = new Shader();
		default_deb_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 330 core\n"
			"out vec4 color;\n\n"
			"uniform vec4 debug_color;\n"
			"void main()\n"
			"{\n"
			"		color = debug_color;\n"
			"}";

		default_deb_frag->SetContent(shader_text);
		std::ofstream outfile(deb_frag_default_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(default_deb_frag);
	}
	CreateResource(deb_frag_default_path);

	prog = new ShaderProgram();
	prog->SetName("default_debug_program");

	vertex = GetShader("default_debug_vertex");
	prog->SetVertexShader(vertex);

	fragment = GetShader("default_debug_fragment");
	prog->SetFragmentShader(fragment);

	prog->LinkShaderProgram();

	AddResource(prog);

	//Depth Shader
	std::string vert_depth_path = SHADER_DEFAULT_FOLDER "depth_shader_vertex.vshader";
	if (!App->file_system->FileExist(vert_depth_path))
	{
		Shader* depth_vert = new Shader();
		depth_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"

			"uniform mat4 lightSpaceMatrix;\n"
			"uniform mat4 model;\n"

			"void main()\n"
			"{\n"
			"	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n"
			"}\n"
			;

		depth_vert->SetContent(shader_text);
		std::ofstream outfile(vert_depth_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(depth_vert);
	}
	CreateResource(vert_depth_path);


	std::string frag_depth_path = SHADER_DEFAULT_FOLDER "depth_shader_fragment.fshader";
	if (!App->file_system->FileExist(frag_depth_path))
	{
		Shader* depth_frag = new Shader();
		depth_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"	#version 330 core\n"
			"	void main()\n"
			"{\n"
			"	gl_FragDepth = gl_FragCoord.z;\n"
			"}\n"
			;
		depth_frag->SetContent(shader_text);
		std::ofstream outfile(frag_depth_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(depth_frag);
	}
	CreateResource(frag_depth_path);

	ShaderProgram* depthprog = new ShaderProgram();
	depthprog->SetName("depth_shader_program");

	Shader* depthvertex = GetShader("depth_shader_vertex");
	depthprog->SetVertexShader(depthvertex);

	Shader* depthfragment = GetShader("depth_shader_fragment");
	depthprog->SetFragmentShader(depthfragment);

	depthprog->LinkShaderProgram();

	AddResource(depthprog);


	//Depth Debug Shader
	/*std::string vert_depthdebug_path = SHADER_DEFAULT_FOLDER "depthdebug_shader_vertex.vshader";
	if (!App->file_system->FileExist(vert_depthdebug_path))
	{
		Shader* depthdebug_vert = new Shader();
		depthdebug_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec2 aTexCoords;\n"

		"out vec2 TexCoords;\n"
		"uniform mat4 Model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"	TexCoords = aTexCoords.xy;\n"
		"	gl_Position = projection * view * Model * vec4(aPos, 1.0f);\n"
		"}\n"
			;

		depthdebug_vert->SetContent(shader_text);
		std::ofstream outfile(vert_depthdebug_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(depthdebug_vert);
	}
	CreateResource(vert_depthdebug_path);


	std::string frag_depthdebug_path = SHADER_DEFAULT_FOLDER "depthdebug_shader_fragment.fshader";
	if (!App->file_system->FileExist(frag_depthdebug_path))
	{
		Shader* depthdebug_frag = new Shader();
		depthdebug_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 330 core\n"
			"out vec4 FragColor;\n"

		"in vec2 TexCoords;\n"

	"	uniform sampler2D depthMap;\n"
		"uniform float near_plane;\n"
		"uniform float far_plane;\n"


		"float LinearizeDepth(float depth)\n"
		"{\n"
		"	float z = depth * 2.0 - 1.0; // Back to NDC \n"
		"	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));\n"
		"}\n"
	
		"void main()\n"
		"{\n"
		"	float depthValue = texture(depthMap, TexCoords).r;\n"
		
		"	FragColor = vec4(vec3(depthValue), 1.0); // orthographic\n"
		"}\n"
			;
		depthdebug_frag->SetContent(shader_text);
		std::ofstream outfile(frag_depthdebug_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(depthdebug_frag);
	}
	CreateResource(frag_depthdebug_path);

	ShaderProgram* depthdebugprog = new ShaderProgram();
	depthdebugprog->SetName("depthdebug_shader_program");

	Shader* depthdebugvertex = GetShader("depthdebug_shader_vertex");
	depthdebugprog->SetVertexShader(depthdebugvertex);

	Shader* depthdebugfragment = GetShader("depthdebug_shader_fragment");
	depthdebugprog->SetFragmentShader(depthdebugfragment);

	depthdebugprog->LinkShaderProgram();

	AddResource(depthdebugprog);*/

	// outline shader
	std::string outline_vert_path = SHADER_DEFAULT_FOLDER "outline_vertex.vshader";
	if (!App->file_system->FileExist(outline_vert_path))
	{
		Shader* outline_vert = new Shader();
		outline_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);

		std::string shader_text =
			"#version 400 core\n"
			"layout(location = 0) in vec3 position;\n"
			"layout(location = 1) in vec3 texCoord;\n\n"
			"out vec3 TexCoord;\n\n"
			"uniform mat4 Model; \n"
			"uniform mat4 view; \n"
			"uniform mat4 projection;\n\n "
			"void main()\n"
			"{ \n"
			"	gl_Position = projection * view * Model * vec4(position, 1.0f);\n"
			"	TexCoord = texCoord;\n"
			"}";

		outline_vert->SetContent(shader_text);
		std::ofstream outfile(outline_vert_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(outline_vert);
	}
	CreateResource(outline_vert_path);

	std::string outline_frag_path = SHADER_DEFAULT_FOLDER "outline_fragment.fshader";
	if (!App->file_system->FileExist(outline_frag_path))
	{
		Shader* outline_frag = new Shader();
		outline_frag->SetShaderType(Shader::ShaderType::ST_FRAGMENT);

		std::string shader_text =
			"#version 400 core\n"
			"out vec4 color;\n\n"
			"void main()\n"
			"{\n"
			"	color = vec4(0.4, 0.28, 0.26, 1.0);\n"
			"}";

		outline_frag->SetContent(shader_text);
		std::ofstream outfile(outline_frag_path.c_str(), std::ofstream::out);
		outfile << shader_text;
		outfile.close();
		RELEASE(outline_frag);
	}
	CreateResource(outline_frag_path);

	ShaderProgram* outline_prog = new ShaderProgram();
	outline_prog->SetName("outline_shader_program");

	Shader* outline_vertex = GetShader("outline_vertex");
	outline_prog->SetVertexShader(outline_vertex);

	Shader* outline_fragment = GetShader("outline_fragment");
	outline_prog->SetFragmentShader(outline_fragment);

	outline_prog->LinkShaderProgram();

	AddResource(outline_prog);
}

void ModuleResources::CreateDefaultMaterial()
{
	CONSOLE_DEBUG("-------------- Creating Default Material -------------");
	if (!App->file_system->DirectoryExist(MATERIAL_DEFAULT_FOLDER_PATH)) App->file_system->Create_Directory(MATERIAL_DEFAULT_FOLDER_PATH);

	std::string default_mat = MATERIAL_DEFAULT_FOLDER "engine_default_mat.mat";
	if (!App->file_system->FileExist(default_mat))
	{
		Material* new_mat = new Material();
		new_mat->SetName("default_mat");
		new_mat->SetAssetsPath(MATERIAL_DEFAULT_FOLDER "engine_default_mat.mat");
		new_mat->SetLibraryPath(LIBRARY_MATERIALS_FOLDER"engine_default_mat.mat");
		Data d;
		new_mat->Save(d);

		d.SaveAsBinary(default_mat);
		d.SaveAsBinary(LIBRARY_MATERIALS_FOLDER"engine_default_mat.mat");

		RELEASE(new_mat);
	}
	CreateResource(default_mat);
}

bool ModuleResources::CheckResourceName(std::string& name)
{
	bool ret = false;

	for (std::string::iterator it = name.begin(); it != name.end(); it++)
	{
		if (*it == ':' || *it == '\\' || *it == '//' || *it == '*' || *it == '?' || *it == '"' || *it == '<' || *it == '>' || *it == '|')
		{
			*it = '_';
			ret = true;
		}
	}

	return ret;
}

void ModuleResources::LoadGOAPGoal(std::string path)
{
	GOAPGoal* goal = new GOAPGoal();
	Data file;
	if (HasMetaFile(path))
	{
		Data data;
		if (data.LoadJSON(path + ".meta"))
		{
			std::string lib_path = data.GetString("lib_path");
			if (!App->file_system->FileExist(lib_path))
				App->file_system->Copy(path, lib_path);
			file.LoadJSON(lib_path);
		}	
		goal->Load(file);
	}
	else
	{
		file.LoadJSON(path);
		goal->Load(file);
		goal->CreateMeta();
	}
	AddGOAPGoal(goal);
}

void ModuleResources::LoadGOAPAction(std::string path)
{
	//if (App->file_system->GetFileExtension(path) == ".cs" || App->file_system->GetFileExtension(path) == ".meta")
	if(App->file_system->GetFileExtension(path) != ".json")
		return;
	std::string name = App->file_system->GetFileNameWithoutExtension(path);
	GOAPAction* action = new GOAPAction(name.c_str(),1,false);
	Data file;
	if (HasMetaFile(path))
	{
		Data data;
		if (data.LoadJSON(path + ".meta"))
		{
			std::string lib_path = data.GetString("lib_path");
			if (!App->file_system->FileExist(lib_path))
				App->file_system->Copy(path, lib_path);
			file.LoadJSON(lib_path);
		}
		action->Load(file);
	}
	else
	{
		file.LoadJSON(path);
		action->Load(file);
		action->CreateMeta();
	}
	AddGOAPAction(action);
}
