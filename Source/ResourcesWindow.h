#pragma once
#include "Window.h"
#include "Resource.h"
#include <map>
#include <list>
#include "Shader.h"

class Texture;
class Mesh;
class Prefab;
class GameObject;
class Material;
class Script;
class Font;
class PhysicsMaterial;
class BlastModel;
class SoundBank;

class ResourcesWindow :
	public Window
{
public:

	enum GameObjectFilter {
		GoFilterRigidBody, GoFilterNone
	};

	ResourcesWindow();
	virtual ~ResourcesWindow();

	void DrawWindow();
	void SetResourceType(Resource::ResourceType type);
	Texture* GetTexture() const;
	Mesh* GetMesh() const;
	Prefab* GetPrefab() const;
	GameObject* GetGameobject() const;
	Material* GetMaterial() const;
	Script* GetScript() const;
	PhysicsMaterial* GetPhysMat() const;
	BlastModel* GetBlastModel() const;
	Shader* GetShader() const;
	Font* GetFont() const;
	SoundBank* GetSoundBank() const;
	void SetShaderType(Shader::ShaderType type);
	void SetCurrentInputName(std::string name);
	std::string GetCurrentInputName() const;
	void Reset();

private:
	Resource::ResourceType type;
	std::map<uint, Texture*> textures_list;
	std::map<uint, Mesh*> meshes_list;
	std::map<uint, Prefab*> prefabs_list;
	std::list<GameObject*> gameobjects_list;
	std::map<uint, Material*> materials_list;
	std::map<uint, Script*> scripts_list;
	std::map<uint, PhysicsMaterial*> phys_material_list;
	std::map<uint, BlastModel*> blast_models_list;
	std::map<uint, Shader*> shaders_list;
	std::map<uint, Font*> fonts_list;
	std::map<uint, SoundBank*> soundbanks_list;

	Texture* texture_to_return;
	Mesh* mesh_to_return;
	Prefab* prefab_to_return;
	GameObject* gameobject_to_return;
	Material* material_to_return;
	Script* script_to_return;
	PhysicsMaterial* phys_mat_to_return;
	BlastModel* blast_model_to_return;
	Shader* shader_to_return;
	Shader::ShaderType shader_type;
	Font* font_to_return;
	SoundBank* soundbank_to_return;

public:
	bool texture_changed;
	bool mesh_changed;
	bool prefab_changed;
	bool gameobject_changed;
	bool material_changed;
	bool script_changed;
	bool phys_mat_changed;
	bool blast_model_changed;
	bool soundbank_changed;
	
	GameObjectFilter go_filter;
	bool shader_changed;
	bool font_changed;

	std::string current_input_name;
};

