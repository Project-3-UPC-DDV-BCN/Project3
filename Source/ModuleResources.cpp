#include "ModuleResources.h"
#include "Texture.h"
#include "Mesh.h"
#include "Prefab.h"
#include "GameObject.h"
#include "Material.h"
#include "Resource.h"
#include "Data.h"
#include "Resource.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleMeshImporter.h"
#include "ModuleTextureImporter.h"
#include "ModulePrefabImporter.h"
#include "ModuleMaterialImporter.h"
#include "ComponentMeshRenderer.h"
#include "Script.h"
#include "ModuleScriptImporter.h"
#include "Shader.h"
#include "ModuleShaderImporter.h"
#include "ShaderProgram.h"


ModuleResources::ModuleResources(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Resources";
}

ModuleResources::~ModuleResources()
{
	for (std::map<uint, GameObject*>::iterator it = gameobjects_list.begin(); it != gameobjects_list.end(); ++it) {
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

	for (std::map<uint, Shader*>::iterator it = shaders_list.begin(); it != shaders_list.end(); ++it) {
		RELEASE(it->second);
	}
	shaders_list.clear();
}

bool ModuleResources::Init(Data * editor_config)
{
	FillResourcesLists();
	return true;
}

void ModuleResources::FillResourcesLists()
{
	std::string assets_folder_path = App->file_system->StringToPathFormat(ASSETS_FOLDER_PATH);
	std::vector<std::string> files_in_assets = App->file_system->GetAllFilesRecursively(assets_folder_path);

	if (!App->file_system->DirectoryExist(LIBRARY_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_FOLDER_PATH);
	if (!App->file_system->DirectoryExist(LIBRARY_TEXTURES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_TEXTURES_FOLDER_PATH);
	if (!App->file_system->DirectoryExist(LIBRARY_MESHES_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MESHES_FOLDER_PATH);
	if (!App->file_system->DirectoryExist(LIBRARY_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PREFABS_FOLDER_PATH);
	if (!App->file_system->DirectoryExist(LIBRARY_MATERIALS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MATERIALS_FOLDER_PATH);
	if (!App->file_system->DirectoryExist(LIBRARY_SHADERS_FOLDER_PATH))
	{
		App->file_system->Create_Directory(LIBRARY_SHADERS_FOLDER_PATH);
	}

	CreateDefaultShaders();
	CreateDefaultMaterial();
	
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

	for (std::vector<std::string>::iterator it = files_in_assets.begin(); it != files_in_assets.end(); it++)
	{
		CreateResource(*it);
	}

	if (exist_shprog_meta)
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
		break;
	case Resource::MaterialResource:
		AddMaterial((Material*)resource);
		break;
	case Resource::ShaderResource:
		AddShader((Shader*)resource);
		break;
	case Resource::ShaderProgramResource:
		AddShaderProgram((ShaderProgram*)resource);
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
		break;
	case Resource::RenderTextureResource:
		break;
	case Resource::GameObjectResource:
		break;
	case Resource::MaterialResource:
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
	if (textures_list.find(uid) != textures_list.end()) return textures_list.at(uid);
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

Shader * ModuleResources::GetShader(std::string name) const
{
	for (std::map<uint, Shader*>::const_iterator it = shaders_list.begin(); it != shaders_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
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

ShaderProgram * ModuleResources::GetShaderProgram(std::string name) const
{
	for (std::map<uint, ShaderProgram*>::const_iterator it = shader_programs_list.begin(); it != shader_programs_list.end(); it++)
	{
		if (it->second != nullptr && it->second->GetName() == name) return it->second;
	}
	return nullptr;
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
		d.EnterSection(section_name);
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

Resource::ResourceType ModuleResources::AssetExtensionToResourceType(std::string str)
{
	if (str == ".jpg" || str == ".png" || str == ".tga" || str == ".dds") return Resource::TextureResource;
	else if (str == ".fbx" || str == ".FBX") return Resource::MeshResource;
	else if (str == ".cs" || str == ".lua") return Resource::ScriptResource;
	else if (str == ".wav" || str == ".ogg") return Resource::AudioResource;
	else if (str == ".prefab") return Resource::PrefabResource;
	else if (str == ".mat") return Resource::MaterialResource;
	else if (str == ".animation") return Resource::AnimationResource;
	else if (str == ".particleFX") return Resource::ParticleFXResource;
	else if (str == ".scene") return Resource::SceneResource;
	else if (str == ".ttf") return Resource::FontResource;
	else if (str == ".vshader") return Resource::ShaderResource;
	else if (str == ".fshader") return Resource::ShaderResource;

	return Resource::Unknown;
}

Resource::ResourceType ModuleResources::LibraryExtensionToResourceType(std::string str)
{
	if (str == ".dds") return Resource::TextureResource;
	else if (str == ".mesh") return Resource::MeshResource;
	else if (str == ".scene") return Resource::SceneResource;
	else if (str == ".mat") return Resource::MaterialResource;
	else if (str == ".dll") return Resource::ScriptResource;
	else if (str == ".prefab" || str == ".fbx" || str == ".FBX") return Resource::PrefabResource;
	else if (str == ".vshader") return Resource::ShaderResource;
	else if (str == ".fshader") return Resource::ShaderResource;

	return Resource::Unknown;
}

std::string ModuleResources::ResourceTypeToLibraryExtension(Resource::ResourceType type)
{
	if (type == Resource::TextureResource) return ".dds";
	else if (type == Resource::MeshResource) return ".mesh";
	else if (type == Resource::SceneResource) return ".scene";
	else if (type == Resource::PrefabResource) return ".prefab";
	else if (type == Resource::MaterialResource) return ".mat";
	else if (type == Resource::ScriptResource) return ".dll";
	else if (type == Resource::ShaderResource) return ".shader";
	
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
	case Resource::ShaderResource:
		directory = App->file_system->StringToPathFormat(LIBRARY_SHADERS_FOLDER);
		if (App->file_system->FileExistInDirectory(file_name + ".shader", directory, false))
		{
			library_file = directory + file_name + ".shader";
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
		break;
	case Resource::FontResource:
		break;
	case Resource::MaterialResource:
		if (!App->file_system->DirectoryExist(LIBRARY_MATERIALS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MATERIALS_FOLDER_PATH);
		ret = App->material_importer->ImportMaterial(file_path);
		break;
	case Resource::ShaderResource:
		if (!App->file_system->DirectoryExist(LIBRARY_SHADERS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SHADERS_FOLDER_PATH);
		ret = App->shader_importer->ImportShader(file_path);
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
		break;
	case Resource::FontResource:
		break;
	case Resource::MaterialResource:
		if (GetMaterial(name) != nullptr)
		{
			resource = (Resource*)GetMaterial(name);
			break;
		}
		resource = (Resource*)App->material_importer->LoadMaterialFromLibrary(library_path);
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
		resource = CreateResourceFromLibrary(path);
		if (resource != nullptr)
		{
			resource->SetAssetsPath(file_path);
			if (!HasMetaFile(file_path)) resource->CreateMeta();
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
	Shader* shader = nullptr;

	if (extension == ".fbx" || extension == ".FBX") type = Resource::PrefabResource;
	else if (extension == ".vshader" || extension == ".fshader") type = Resource::ShaderResource;

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
	}
}

void ModuleResources::CreateDefaultShaders()
{
	CONSOLE_DEBUG("-------------- Creating Default Shaders -------------");
	if (!App->file_system->DirectoryExist(SHADER_DEFAULT_FOLDER_PATH)) App->file_system->Create_Directory(SHADER_DEFAULT_FOLDER_PATH);

	//Default Rendering Shaders
	std::string vert_default_path = SHADER_DEFAULT_FOLDER "default_vertex.vshader";
	if (!App->file_system->FileExist(vert_default_path))
	{
		Shader* default_vert = new Shader();
		default_vert->SetShaderType(Shader::ShaderType::ST_VERTEX);
		
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
			"#version 330 core\n"
			"in vec4 ourColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n\n"
			"out vec4 color;\n\n"
			"uniform bool has_material_color;\n"
			"uniform vec4 material_color;\n"
			"uniform bool has_texture;\n"
			"uniform sampler2D ourTexture;\n\n"
			"void main()\n"
			"{\n"
			"	if(has_texture)\n"
			"		color = texture(ourTexture, TexCoord);\n"
			"	else if(has_material_color)\n"
			"		color = material_color;\n"
			"	else\n"
			"		color = ourColor;\n"
			"}";

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
}

void ModuleResources::CreateDefaultMaterial()
{
	CONSOLE_DEBUG("-------------- Creating Default Material -------------");
	if (!App->file_system->DirectoryExist(MATERIAL_DEFAULT_FOLDER_PATH)) App->file_system->Create_Directory(MATERIAL_DEFAULT_FOLDER_PATH);

	std::string default_mat = MATERIAL_DEFAULT_FOLDER "default_material.mat";
	if (!App->file_system->FileExist(default_mat))
	{
		Material* new_mat = new Material();
		new_mat->SetName("default_material");
		Data d;
		new_mat->Save(d);

		d.SaveAsBinary(default_mat);

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
