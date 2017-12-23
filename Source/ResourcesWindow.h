#pragma once
#include "Window.h"
#include "Resource.h"
#include <map>
#include <list>

class Texture;
class Mesh;
class Prefab;
class GameObject;
class Material;
class Script;
class Shader;

class ResourcesWindow :
	public Window
{
public:
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
	Shader* GetShader() const;
	void SetShaderType(Shader::ShaderType type);
	void Reset();

private:
	Resource::ResourceType type;
	std::map<uint, Texture*> textures_list;
	std::map<uint, Mesh*> meshes_list;
	std::map<uint, Prefab*> prefabs_list;
	std::list<GameObject*> gameobjects_list;
	std::map<uint, Material*> materials_list;
	std::map<uint, Script*> scripts_list;
	std::map<uint, Shader*> shaders_list;

	Texture* texture_to_return;
	Mesh* mesh_to_return;
	Prefab* prefab_to_return;
	GameObject* gameobject_to_return;
	Material* material_to_return;
	Script* script_to_return;
	Shader* shader_to_return;
	Shader::ShaderType shader_type;

public:
	bool texture_changed;
	bool mesh_changed;
	bool prefab_changed;
	bool gameobject_changed;
	bool material_changed;
	bool script_changed;
	bool shader_changed;
};

