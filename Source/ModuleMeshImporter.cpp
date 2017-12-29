#include "ModuleMeshImporter.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Mesh.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleTextureImporter.h"
#include "ComponentMeshRenderer.h"
#include "ModuleResources.h"
#include "Material.h"
#include "Texture.h"
#include "Prefab.h"
#include "ImportWindow.h"

#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/cimport.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleMeshImporter::ModuleMeshImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleMeshImporter::~ModuleMeshImporter()
{
}

bool ModuleMeshImporter::Init(Data * editor_config)
{
	name = "Mesh_Importer";

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = &Callback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleMeshImporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

std::string ModuleMeshImporter::ImportMesh(std::string path)
{
	std::string ret;

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* root_node = scene->mRootNode;
		if (!App->file_system->DirectoryExist(LIBRARY_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PREFABS_FOLDER_PATH);
		std::string library_path = LIBRARY_PREFABS_FOLDER + App->file_system->GetFileNameWithoutExtension(path) + ".prefab";
		Data data;
		Prefab* prefab = new Prefab();
		prefab->SetRootGameObject(LoadMeshNode(nullptr, root_node, *scene, path.c_str()));
		prefab->SetAssetsPath(path);
		prefab->SetLibraryPath(library_path);
		prefab->SetName(App->file_system->GetFileNameWithoutExtension(path));
		prefab->Save(data);
		if (!App->file_system->DirectoryExist(LIBRARY_PREFABS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_PREFABS_FOLDER_PATH);
		data.SaveAsBinary(library_path);
		App->resources->AddPrefab(prefab);

		ret = library_path;

		CONSOLE_DEBUG("Object succesfully loaded from, %s", path);
		aiReleaseImport(scene);
	}
	else
	{
		CONSOLE_ERROR("Cannot load object from %s", path);
	}

	return ret;
}

GameObject* ModuleMeshImporter::LoadMeshNode(GameObject * parent, aiNode * node, const aiScene & scene, const char * path)
{
	GameObject* ret = nullptr;
	GameObject* root = nullptr;

	if (node->mNumMeshes < 1)
	{
		std::string s_node_name(node->mName.C_Str());
		aiVector3D pos;
		aiQuaternion quat;
		aiVector3D scale;
		if (s_node_name.find("$AssimpFbx$_PreRotation") != std::string::npos || s_node_name.find("$AssimpFbx$_Rotation") != std::string::npos ||
			s_node_name.find("$AssimpFbx$_PostRotation") != std::string::npos || s_node_name.find("$AssimpFbx$_Scaling") != std::string::npos ||
			s_node_name.find("$AssimpFbx$_Translation") != std::string::npos)
		{
			GetDummyTransform(*node, pos, quat, scale);
			aiVector3D n_pos;
			aiQuaternion n_quat;
			aiVector3D n_scale;
			node->mTransformation.Decompose(n_scale, n_quat, n_pos);
			pos += n_pos;
			quat = quat * n_quat;
			scale = n_scale;
		}
		else
		{
			node->mTransformation.Decompose(scale, quat, pos);
		}

		std::string root_go_name;
		(s_node_name.find("RootNode") != std::string::npos) ? root_go_name = App->file_system->GetFileNameWithoutExtension(path) : root_go_name = node->mName.C_Str();
		if (parent && root_go_name == parent->GetName())
		{
			root = parent;
		}
		else
		{
			root = new GameObject(parent);
			root->SetName(root_go_name);
			ComponentTransform* transform = (ComponentTransform*)root->GetComponent(Component::CompTransform);
			math::Quat math_quat(quat.x, quat.y, quat.z, quat.w);
			float3 rotation = math::RadToDeg(math_quat.ToEulerXYZ());
			transform->SetPosition({ pos.x, pos.y, pos.z });
			transform->SetRotation(rotation);
			transform->SetScale({ scale.x, scale.y, scale.z });
			if (!node->mParent)
			{
				root->SetRoot(true);
				ret = root;
			}
		}
	}
	else
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			bool mesh_created = true; //If node have more than 1 mesh and last mesh returned false, we need to reset the return for the new mesh.

			aiMesh* ai_mesh = scene.mMeshes[node->mMeshes[i]];

			// -------------- Mesh --------------------
			Mesh* mesh = new Mesh();
			std::string mesh_name = (std::string)node->mName.C_Str();
			if (i > 0) mesh_name += "_" + std::to_string(i);
			mesh->SetName(mesh_name);

			mesh->num_vertices = ai_mesh->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * 3];
			memcpy(mesh->vertices, ai_mesh->mVertices, sizeof(float) * mesh->num_vertices * 3);
			CONSOLE_DEBUG("New mesh ""%s"" with %d vertices", node->mName.C_Str(), mesh->num_vertices);

			if (ai_mesh->HasFaces())
			{
				mesh->num_indices = ai_mesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices]; // assume each face is a triangle
				CONSOLE_DEBUG("New mesh ""%s"" with %d indices.", node->mName.C_Str(), mesh->num_indices);
				for (uint j = 0; j < ai_mesh->mNumFaces; ++j)
				{
					if (ai_mesh->mFaces[j].mNumIndices != 3)
					{
						CONSOLE_DEBUG("WARNING, geometry face %d with != 3 indices! Not imported", j);
						mesh_created = false;
					}
					else
					{
						memcpy(&mesh->indices[j * 3], ai_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}
				CONSOLE_DEBUG("New mesh ""%s"" with %d triangles.", node->mName.C_Str(), mesh->num_indices / 3);
			}

			if (!mesh_created) continue;

			float* normals = nullptr;
			if (ai_mesh->HasNormals())
			{
				normals = new float[mesh->num_vertices * 3];
				memcpy(normals, ai_mesh->mNormals, sizeof(float) * mesh->num_vertices * 3);
				CONSOLE_DEBUG("Mesh ""%s"" has Normals", node->mName.C_Str());
			}

			float* colors = nullptr;
			if (ai_mesh->HasVertexColors(0))
			{
				colors = new float[mesh->num_vertices * 4];
				memcpy(colors, ai_mesh->mColors[0], sizeof(float) * mesh->num_vertices * 4);
				CONSOLE_DEBUG("Mesh ""%s"" has Colors", node->mName.C_Str());
			}

			float* texture_coords = nullptr;
			if (ai_mesh->HasTextureCoords(0))
			{
				texture_coords = new float[mesh->num_vertices * 3];
				memcpy(texture_coords, ai_mesh->mTextureCoords[0], sizeof(float) * mesh->num_vertices * 3);
				CONSOLE_DEBUG("Mesh ""%s"" has UVs", node->mName.C_Str());
			}

			///Create the data buffer
			mesh->vertices_data = new float[mesh->num_vertices * 13]; // vert pos, tex coords, normals, color

			float null[3] = { 0.f,0.f,0.f };
			float null_color[4] = { 1.f,1.f,1.f,1.f };
			for (int v = 0; v < mesh->num_vertices; ++v)
			{
				//copy vertex pos
				memcpy(mesh->vertices_data + v * 13, mesh->vertices + v * 3, sizeof(float) * 3);

				//copy tex coord
				if (texture_coords != nullptr)
					memcpy(mesh->vertices_data + v * 13 + 3, texture_coords + v * 3, sizeof(float) * 3);
				else
					memcpy(mesh->vertices_data + v * 13 + 3, null, sizeof(float) * 3);

				//copy normals
				if (normals != nullptr)
					memcpy(mesh->vertices_data + v * 13 + 6, normals + v * 3, sizeof(float) * 3);
				else
					memcpy(mesh->vertices_data + v * 13 + 6, null, sizeof(float) * 3);

				//copy colors
				if (colors != nullptr)
					memcpy(mesh->vertices_data + v * 13 + 9, colors + v * 4, sizeof(float) * 4);
				else
					memcpy(mesh->vertices_data + v * 13 + 9, null_color, sizeof(float) * 4);
			}

			mesh->CreateVerticesFromData();
			// -------------------------------------

			Material* material = nullptr;
			if (scene.HasMaterials())
			{
				aiMaterial* ai_mat = scene.mMaterials[ai_mesh->mMaterialIndex];

				if (ai_mat != nullptr)
				{
					aiString mat_name;
					ai_mat->Get(AI_MATKEY_NAME, mat_name);
					material = App->resources->GetMaterial(mat_name.C_Str());
					if (material == nullptr)
					{
						material = new Material();
						material->SetName(mat_name.C_Str());
						LoadMaterial(*material, *ai_mat);
						std::string model_name = App->file_system->GetFileNameWithoutExtension(path);
						std::string directory = App->file_system->GetFileDirectory(path);
						std::string materials_directory = directory + "/" + model_name + " - materials";
						std::string material_library_path = LIBRARY_MATERIALS_FOLDER + material->GetName() + ".mat";
						std::string material_assets_path = materials_directory + "/" + material->GetName() + ".mat";
						material->SetAssetsPath(material_assets_path);
						material->SetLibraryPath(material_library_path);
						if (!App->file_system->DirectoryExist(materials_directory))
						{
							App->file_system->Create_Directory(materials_directory);
						}
						App->resources->AddMaterial(material);
						Data material_data;
						material->Save(material_data);
						material_data.SaveAsBinary(material_assets_path);
						if (!App->file_system->DirectoryExist(LIBRARY_MATERIALS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_MATERIALS_FOLDER_PATH);
						material_data.SaveAsBinary(material_library_path);
						material->CreateMeta();
					}
				}
			}

			SaveMeshToLibrary(*mesh);
			App->resources->AddMesh(mesh);

			GameObject* go = new GameObject(parent);
			ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)go->AddComponent(Component::CompMeshRenderer);
			mesh_renderer->SetMesh(mesh);
			mesh->DecreaseUsedCount();
			mesh_renderer->SetMaterial(material);
			material->DecreaseUsedCount();
			ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
			aiVector3D position;
			aiQuaternion rotation_quat;
			aiVector3D scale;
			node->mTransformation.Decompose(scale, rotation_quat, position);
			math::Quat math_quat(rotation_quat.x, rotation_quat.y, rotation_quat.z, rotation_quat.w);
			float3 rotation = math::RadToDeg(math_quat.ToEulerXYZ());
			transform->SetPosition({ position.x, position.y, position.z });
			transform->SetRotation(rotation);
			transform->SetScale({ scale.x, scale.y, scale.z });
			go->SetName(mesh->GetName());
		}
		root = parent;
	}


	for (int i = 0; i < node->mNumChildren; i++)
	{
		LoadMeshNode(root, node->mChildren[i], scene, path);
	}

	return ret;
}

void ModuleMeshImporter::GetDummyTransform(aiNode & node, aiVector3D & pos, aiQuaternion & rot, aiVector3D & scale)
{
	if (node.mChildren)
	{
		std::string s_node_name(node.mName.C_Str());
		if (s_node_name.find("$AssimpFbx$_PreRotation") != std::string::npos || s_node_name.find("$AssimpFbx$_Rotation") != std::string::npos ||
			s_node_name.find("$AssimpFbx$_PostRotation") != std::string::npos || s_node_name.find("$AssimpFbx$_Scaling") != std::string::npos ||
			s_node_name.find("$AssimpFbx$_Translation") != std::string::npos)
		{
			aiVector3D node_pos;
			aiQuaternion node_quat;
			aiVector3D node_scale;
			node.mTransformation.Decompose(node_scale, node_quat, node_pos);
			pos += node_pos;
			rot = rot * node_quat;
			scale = aiVector3D(scale * node_scale);
			node = *node.mChildren[0];
			GetDummyTransform(node, pos, rot, scale);
		}
	}
}

void ModuleMeshImporter::LoadMaterial(Material & material, const aiMaterial& ai_material)
{
	aiColor3D diffuse;
	aiColor3D specular;
	aiColor3D ambient;
	aiColor3D emissive;
	aiColor3D transparent;
	aiColor3D reflective;
	aiString texture_path;
	bool wireframe = false;
	bool two_sided = false;
	int shading_model = 0;
	int blend_mode = 0;
	float opacity = 1;
	float shininess = 0;
	float shininess_strength = 1;
	float refraction = 1;
	float reflectivity = 0;
	float bump_scaling = 1;


	//COLOR
	ai_material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	ai_material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
	ai_material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	ai_material.Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
	ai_material.Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
	ai_material.Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);

	material.SetDiffuseColor(diffuse.r, diffuse.g, diffuse.b);
	material.SetSpecularColor(specular.r, specular.g, specular.b);
	material.SetAmbientColor(ambient.r, ambient.g, ambient.b);
	material.SetEmissiveColor(emissive.r, emissive.g, emissive.b);
	material.SetTransparentColor(transparent.r, transparent.g, transparent.b);
	material.SetReflectiveColor(reflective.r, reflective.g, reflective.b);

	//TEXTURES
	for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; i++)
	{
		for (int j = 0; j < ai_material.GetTextureCount((aiTextureType)i); j++)
		{
			ai_material.GetTexture((aiTextureType)i, j, &texture_path);
			Texture* texture = CreateTexture(texture_path.C_Str());
			material.SetDiffuseTexture(texture);
			App->resources->AddTexture(texture);
		}
	}

	//PROPERTIES
	ai_material.Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
	ai_material.Get(AI_MATKEY_TWOSIDED, two_sided);
	ai_material.Get(AI_MATKEY_SHADING_MODEL, shading_model);
	ai_material.Get(AI_MATKEY_BLEND_FUNC, blend_mode);
	ai_material.Get(AI_MATKEY_OPACITY, opacity);
	ai_material.Get(AI_MATKEY_SHININESS, shininess);
	ai_material.Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);
	ai_material.Get(AI_MATKEY_REFRACTI, refraction);
	ai_material.Get(AI_MATKEY_COLOR_REFLECTIVE, reflectivity);
	ai_material.Get(AI_MATKEY_BUMPSCALING, bump_scaling);

	material.SetWireframe(wireframe);
	material.SetTwoSided(two_sided);
	material.SetShadingModel(shading_model);
	material.SetBlendMode(blend_mode);
	material.SetOpacity(opacity);
	material.SetShininess(shininess);
	material.SetShininessStrength(shininess_strength);
	material.SetRefraction(refraction);
	material.SetReflectivity(reflectivity);
	material.SetBumpScaling(bump_scaling);

}

Texture* ModuleMeshImporter::CreateTexture(std::string mat_texture_name)
{
	Texture* material_texture = nullptr;
	if (mat_texture_name.length() > 0)
	{
		std::string full_texture_path = App->file_system->StringToPathFormat(ASSETS_TEXTURES_FOLDER + App->file_system->GetFileName(mat_texture_name));
		std::string texture_name = App->file_system->GetFileNameWithoutExtension(mat_texture_name);
		material_texture = App->resources->GetTexture(texture_name);
		if (!material_texture)
		{
			std::string library_path = App->texture_importer->ImportTexture(full_texture_path);

			if (!library_path.empty())
			{
				material_texture = App->texture_importer->LoadTextureFromLibrary(library_path);
				if (material_texture)
				{
					material_texture->SetAssetsPath(full_texture_path);
				}
			}
		}
	}

	if (!material_texture) material_texture = new Texture();

	return material_texture;
}

Mesh * ModuleMeshImporter::LoadMeshFromLibrary(std::string path)
{
	Mesh* mesh = nullptr;

	std::ifstream file(path, std::ifstream::binary);
	if (file.is_open())
	{
		// get length of file:
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		char * buffer = new char[length];
		file.read(buffer, length);

		if (file)
		{
			CONSOLE_DEBUG("Mesh library file %s read successfully", App->file_system->GetFileNameWithoutExtension(path.c_str()).c_str());

			char* cursor = buffer;
			mesh = new Mesh();
			// amount of indices / vertices / colors / normals / texture_coords
			uint ranges[2];
			uint bytes = sizeof(ranges);
			memcpy(ranges, cursor, bytes);

			mesh->num_indices = ranges[0];
			mesh->num_vertices = ranges[1];

			// Load indices
			cursor += bytes;
			bytes = sizeof(uint) * mesh->num_indices;
			mesh->indices = new uint[mesh->num_indices];
			memcpy(mesh->indices, cursor, bytes);

			// Load vertices_data
			cursor += bytes;
			bytes = sizeof(float) * mesh->num_vertices * 13;
			mesh->vertices_data = new float[mesh->num_vertices * 13];
			memcpy(mesh->vertices_data, cursor, bytes);

			// AABB
			cursor += bytes;
			bytes = sizeof(AABB);
			memcpy(&mesh->box.minPoint.x, cursor, bytes);

			RELEASE_ARRAY(buffer);

			mesh->CreateVerticesFromData();
			mesh->SetLibraryPath(path);
		}
		else
		{
			CONSOLE_DEBUG("Mesh library file %s read failed", App->file_system->GetFileNameWithoutExtension(path.c_str()).c_str());
		}
		file.close();
	}

	return mesh;
}

void ModuleMeshImporter::SaveMeshToLibrary(Mesh& mesh)
{
	
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[2] = {
		mesh.num_indices,
		mesh.num_vertices,
	};

	uint size = sizeof(ranges);
	size += sizeof(uint) * mesh.num_indices;
	size += sizeof(float) * mesh.num_vertices * 13;
	size += sizeof(AABB);

	// allocate and fill
	char* data = new char[size];
	char* cursor = data;

	// First store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	// Store indices
	cursor += bytes;
	bytes = sizeof(uint) * mesh.num_indices;
	memcpy(cursor, mesh.indices, bytes);

	// Store vertices
	cursor += bytes;
	bytes = sizeof(float) * mesh.num_vertices * 13;
	memcpy(cursor, mesh.vertices_data, bytes);

	// Store AABB
	cursor += bytes;
	bytes = sizeof(AABB);
	memcpy(cursor, &mesh.box.minPoint.x, bytes);

	std::string mesh_name = mesh.GetName();
	if (App->resources->CheckResourceName(mesh_name))
	{
		mesh.SetName(mesh_name);
	}
	std::string library_path = LIBRARY_MESHES_FOLDER + mesh.GetName() + ".mesh";
	std::ofstream outfile(library_path.c_str(), std::ofstream::binary);
	outfile.write(data, size);
	outfile.close();

	mesh.SetLibraryPath(library_path);

	delete[] data;
	data = nullptr;
}

void Callback(const char* message, char* c) {
	CONSOLE_DEBUG("ASSIMP: %s", message);
}
