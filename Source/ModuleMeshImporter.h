#pragma once
#include "Module.h"
#include <string>
#include "Assimp\include\quaternion.h"
#include "Assimp\include\vector3.h"

class Mesh;
struct aiNode;
struct aiScene;
struct aiMaterial;
class GameObject;
class Data;
class Material;
class Texture;
class Prefab;

class ModuleMeshImporter :
	public Module
{
public:
	ModuleMeshImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleMeshImporter();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

	std::string ImportMesh(std::string path);
	Mesh* LoadMeshFromLibrary(std::string path);
	void SaveMeshToLibrary(Mesh& mesh);

private:
	//Loads the model and returns the root gameobject
	GameObject* LoadMeshNode(GameObject* parent, aiNode* node, const aiScene& scene, const char* path);
	void GetDummyTransform(aiNode& node, aiVector3D& pos, aiQuaternion& rot, aiVector3D& scale);
	void LoadMaterial(Material& material, const aiMaterial& ai_material);
	Texture* CreateTexture(std::string mat_texture_path);

	void CreateSkeletonsAndVertexWeights(Prefab* prefab, const aiScene& scene);

	void CreatePrimitives() const;
	void CreateBox() const;
	void CreatePlane() const;
	void CreateSphere() const;

};

void Callback(const char* message, char* c);

