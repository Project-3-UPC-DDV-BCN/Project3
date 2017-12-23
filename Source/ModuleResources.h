#pragma once
#include "Module.h"
#include <map>
#include "Resource.h"

class Texture;
class Mesh;
class Prefab;
class GameObject;
class Resources;
class Material;
class Script;
class Shader;
class ShaderProgram;

class ModuleResources :
	public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleResources();

	bool Init(Data* editor_config = nullptr);

	void FillResourcesLists();

	void AddResource(Resource* resource);
	void ImportFile(std::string path);

	Texture* GetTexture(std::string name) const;
	Texture* GetTexture(UID uid) const;
	void AddTexture(Texture* texture);
	void RemoveTexture(Texture* texture);
	std::map<uint, Texture*> GetTexturesList() const;

	Mesh* GetMesh(std::string name) const;
	Mesh* GetMesh(UID uid) const;
	void AddMesh(Mesh* mesh);
	void RemoveMesh(Mesh* mesh);
	std::map<uint, Mesh*> GetMeshesList() const;

	Prefab* GetPrefab(std::string name) const;
	Prefab* GetPrefab(UID uid) const;
	void AddPrefab(Prefab* prefab);
	void RemovePrefab(Prefab* prefab);
	std::map<uint, Prefab*> GetPrefabsList() const;

	GameObject* GetGameObject(std::string name) const;
	GameObject* GetGameObject(UID uid) const;
	void AddGameObject(GameObject* gameobject);
	void RemoveGameObject(GameObject* gameobject);
	std::map<uint, GameObject*> GetGameobjectsList() const;

	Material* GetMaterial(std::string name) const;
	Material* GetMaterial(UID uid) const;
	void AddMaterial(Material* material);
	void RemoveMaterial(Material* material);
	std::map<uint, Material*> GetMaterialsList() const;

	Script* GetScript(std::string name) const;
	Script* GetScript(UID uid) const;
	void AddScript(Script* script);
	void RemoveScript(Script* script);
	std::map<uint, Script*> GetScriptsList() const;

	Shader* GetShader(std::string name) const;
	Shader* GetShader(UID uid) const;
	void AddShader(Shader* shader);
	void RemoveShader(Shader* shader);
	std::map<uint, Shader*> GetShadersList() const;

	ShaderProgram* GetShaderProgram(std::string name) const;
	ShaderProgram* GetShaderProgram(UID uid) const;
	void AddShaderProgram(ShaderProgram* program);
	void RemoveShaderProgram(ShaderProgram* program);
	std::map<uint, ShaderProgram*> GetShaderProgramList() const;

	Resource::ResourceType AssetExtensionToResourceType(std::string str);
	Resource::ResourceType LibraryExtensionToResourceType(std::string str);
	std::string ResourceTypeToLibraryExtension(Resource::ResourceType type);

	bool HasMetaFile(std::string file_path);
	std::string GetLibraryFile(std::string file_path);
	std::string CreateLibraryFile(Resource::ResourceType type, std::string file_path);
	Resource* CreateResourceFromLibrary(std::string library_path);
	void CreateResource(std::string file_path);
	void DeleteResource(std::string file_path);
	//Checks if name is valid (not contain invalid characters) and change it for a valid name if necessary
	bool CheckResourceName(std::string& name);

private:
	void DeleteFBXMeshes(GameObject* gameobject);

private:
	std::map<uint, Texture*> textures_list;
	std::map<uint, Mesh*> meshes_list;
	std::map<uint, Prefab*> prefabs_list;
	std::map<uint, GameObject*> gameobjects_list;
	std::map<uint, Material*> materials_list;
	std::map<uint, Script*> scripts_list;
	std::map<uint, Shader*> shaders_list;
	std::map<uint, ShaderProgram*> shader_programs_list;
};

