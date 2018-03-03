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
class PhysicsMaterial;
class BlastModel;
class ParticleData; 
class Shader;
class ShaderProgram;
class Font;
class SoundBankResource;

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

	PhysicsMaterial* GetPhysMaterial(std::string name) const;
	PhysicsMaterial* GetPhysMaterial(UID uid) const;
	void AddPhysMaterial(PhysicsMaterial* material);
	void RemovePhysMaterial(PhysicsMaterial* material);
	std::map<uint, PhysicsMaterial*> GetPhysMaterialsList() const;

	BlastModel* GetBlastModel(std::string name) const;
	BlastModel* GetBlastModel(UID uid) const;
	void AddBlastModel(BlastModel* mesh);
	void RemoveBlastModel(BlastModel* mesh);
	std::map<uint, BlastModel*> GetBlastModelsList() const;

	SoundBankResource* GetSoundBank(std::string name) const;
	SoundBankResource* GetSoundBank(UID uid) const;
	void AddSoundBank(SoundBankResource* sbk);
	void RemoveSoundBank(SoundBankResource* sbk);
	std::map<uint, SoundBankResource*> GetSoundBanksList() const;

	Shader* GetShader(std::string name) const;
	Shader* GetShader(UID uid) const;
	void AddShader(Shader* shader);
	void RemoveShader(Shader* shader);
	std::map<uint, Shader*> GetShadersList() const;

	Font* GetFont(std::string name) const;
	Font* GetFont(UID uid) const;
	void AddFont(Font* font);
	void RemoveFont(Font* font);
	std::map<uint, Font*> GetFontsList() const;

	ParticleData* GetParticleTemplate(std::string name) const;
	ParticleData* GetParticleTemplate(UID uid) const;
	void AddParticleTemplate(ParticleData* texture);
	void RemoveParticleTemplate(ParticleData* texture);
	std::map<uint, ParticleData*> GetParticlesList() const;

	ShaderProgram* GetShaderProgram(std::string name) const;
	ShaderProgram* GetShaderProgram(UID uid) const;
	ShaderProgram* GetShaderProgram(Shader* vertex, Shader* fragment) const;
	void AddShaderProgram(ShaderProgram* program);
	void RemoveShaderProgram(ShaderProgram* program);
	std::map<uint, ShaderProgram*> GetShaderProgramList() const;
	void LoadShaderProgramMeta(std::string path) const;	
	void OnShaderUpdate(Shader* shader) const;

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

	void CreateDefaultShaders();
	void CreateDefaultMaterial();

private:
	std::map<uint, Texture*> textures_list;
	std::map<uint, Mesh*> meshes_list;
	std::map<uint, Prefab*> prefabs_list;
	std::map<uint, GameObject*> gameobjects_list;
	std::map<uint, Material*> materials_list;
	std::map<uint, Script*> scripts_list;
	std::map<uint, PhysicsMaterial*> phys_materials_list;
	std::map<uint, BlastModel*> blast_models_list;
	std::map<uint, ParticleData*> particles_list;
	std::map<uint, Shader*> shaders_list;
	std::map<uint, ShaderProgram*> shader_programs_list;
	std::map<uint, Font*> fonts_list;
	std::map<uint, SoundBankResource*> soundbanks_list;
};

