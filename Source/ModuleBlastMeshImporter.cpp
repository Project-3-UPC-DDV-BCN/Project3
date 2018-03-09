#include "ModuleBlastMeshImporter.h"
#include <vector>
#include <NvBlastTkAsset.h>
#include <NvBlastExtPxAsset.h>
#include <NvBlastExtPxSerialization.h>
#include <NvBlastExtSerialization.h>
#include <NvBlastExtTkSerialization.h>
#include <NvBlastExtAssetUtils.h>
#include <NvBlastExtDamageShaders.h>
#include <NvBlastExtExporter.h>
#include "Application.h"
#include "ModulePhysics.h"
#include "Data.h"
#include <fstream>
#include "ModuleFileSystem.h"
#include "ModuleBlast.h"
#include "ComponentBlast.h"
#include "Mesh.h"
#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ModuleResources.h"
#include "ModuleMeshImporter.h"
#include "Material.h"
#include "ModuleScene.h"
#include "BlastModel.h"

ModuleBlastMeshImporter::ModuleBlastMeshImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Blast_Mesh_Importer";
	serialization = nullptr;
}


ModuleBlastMeshImporter::~ModuleBlastMeshImporter()
{
}

bool ModuleBlastMeshImporter::Init(Data * editor_config)
{
	serialization = NvBlastExtSerializationCreate();

	if (serialization != nullptr && App->physics->GetPhysXPhysics() != nullptr && App->physics->GetPhysXCooking() != nullptr && App->blast->GetFramework() != nullptr)
	{
		NvBlastExtTkSerializerLoadSet(*App->blast->GetFramework(), *serialization);
		NvBlastExtPxSerializerLoadSet(*App->blast->GetFramework(), *App->physics->GetPhysXPhysics(), *App->physics->GetPhysXCooking(), *serialization);
		serialization->setSerializationEncoding(NVBLAST_FOURCC('C', 'P', 'N', 'B'));
	}
	else
	{
		CONSOLE_DEBUG("Can't initialize blast importer");
	}
	return true;
}

bool ModuleBlastMeshImporter::CleanUp()
{
	serialization->release();
	return true;
}

std::string ModuleBlastMeshImporter::ImportModel(std::string path)
{
	std::string name_without_extension = App->file_system->GetFileNameWithoutExtension(path);
	std::string file_directory = App->file_system->GetFileDirectory(path);
	std::string fbx_path = file_directory + "\\" + name_without_extension + "_blast.fbx";
	std::string library_path;

	if (App->file_system->FileExist(fbx_path))
	{
		Nv::Blast::IFbxFileReader* reader = NvBlastExtExporterCreateFbxFileReader();

		if (reader)
		{
			reader->loadFromFile(fbx_path.c_str());
			if (reader->getBoneCount() != 0)
			{
				std::string file_name = App->file_system->GetFileName(path);
				App->file_system->Copy(path, LIBRARY_BMODEL_FOLDER + file_name);
				library_path = LIBRARY_BMODEL_FOLDER + file_name;

				BlastModel* model = new BlastModel();
				model->root = new GameObject();
				model->root->SetName(name_without_extension);
				model->chunks.resize(reader->getBoneCount());
				uint32_t* infl;
				reader->getBoneInfluences(infl);
				physx::PxVec3* vertice_pos = reader->getPositionArray();
				physx::PxVec2* vertice_uv = reader->getUvArray();
				physx::PxVec3* vertice_normals = reader->getNormalsArray();
				uint vertices_count = reader->getVerticesCount();
				std::vector<int32_t> indRemap(reader->getVerticesCount(), -1);
				uint indices_count = reader->getIndicesCount();
				uint* indices_array = reader->getIndexArray();

				for (uint bone_index = 0; bone_index < reader->getBoneCount(); ++bone_index)
				{
					std::fill(indRemap.begin(), indRemap.end(), -1);
					GameObject* go = new GameObject(model->root);
					go->SetName(name_without_extension + "_chunk_" + std::to_string(bone_index));
					go->SetActive(false);
					ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)go->AddComponent(Component::CompMeshRenderer);
					//mesh_renderer->SetMeshType(ComponentMeshRenderer::BlastMesh);
					//mesh_renderer->SetName("Blast_Mesh_Renderer");

					std::vector<float> vertex_data;
					for (int i = 0; i < vertices_count; i++)
					{
						if (bone_index == infl[i])
						{
							indRemap[i] = (int32_t)vertex_data.size() / 19;
							vertex_data.push_back(vertice_pos[i].x);
							vertex_data.push_back(vertice_pos[i].y);
							vertex_data.push_back(vertice_pos[i].z);
							vertex_data.push_back(vertice_uv[i].x);
							vertex_data.push_back(vertice_uv[i].y);
							vertex_data.push_back(0);
							vertex_data.push_back(vertice_normals[i].x);
							vertex_data.push_back(vertice_normals[i].y);
							vertex_data.push_back(vertice_normals[i].z);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
							vertex_data.push_back(1);
						}
					}

					int vertex_data_size = vertex_data.size();
					Mesh* mesh = new Mesh();
					mesh->SetName(name_without_extension + "_chunk_" + std::to_string(bone_index));
					mesh->num_vertices = vertex_data.size() / 19;
					mesh->vertices_data = new float[vertex_data_size];
					
					for (int i = 0; i < vertex_data_size; i += 19)
					{
						mesh->vertices_data[i] = vertex_data[i];
						mesh->vertices_data[i + 1] = vertex_data[i + 1];
						mesh->vertices_data[i + 2] = vertex_data[i + 2];
						mesh->vertices_data[i + 3] = vertex_data[i + 3];
						mesh->vertices_data[i + 4] = vertex_data[i + 4];
						mesh->vertices_data[i + 5] = vertex_data[i + 5];
						mesh->vertices_data[i + 6] = vertex_data[i + 6];
						mesh->vertices_data[i + 7] = vertex_data[i + 7];
						mesh->vertices_data[i + 8] = vertex_data[i + 8];
						mesh->vertices_data[i + 9] = vertex_data[i + 9];
						mesh->vertices_data[i + 10] = vertex_data[i + 10];
						mesh->vertices_data[i + 11] = vertex_data[i + 11];
						mesh->vertices_data[i + 12] = vertex_data[i + 12];
						mesh->vertices_data[i + 13] = vertex_data[i + 13];
						mesh->vertices_data[i + 14] = vertex_data[i + 14];
						mesh->vertices_data[i + 15] = vertex_data[i + 15];
						mesh->vertices_data[i + 16] = vertex_data[i + 16];
						mesh->vertices_data[i + 17] = vertex_data[i + 17];
						mesh->vertices_data[i + 18] = vertex_data[i + 18];
					}

					std::vector<int> indices_data;
					for (uint32_t j = 0; j < indices_count; j += 3)
					{
						for (int tv : { 0, 1, 2})
						{
							uint32_t oldIndex = indices_array[j + tv];
							int32_t newIndex = indRemap[oldIndex];
							if (newIndex >= 0)
							{
								indices_data.push_back(reader->getMaterialIds()[j]);
								indices_data.push_back(newIndex);
							}
						}
					}

					int indices_data_size = indices_data.size();
					mesh->num_indices = indices_data_size / 2;
					mesh->indices = new uint[mesh->num_indices];
					
					std::vector<uint> material_indices;
					std::vector<uint> interior_material_indices;
					for (uint i = 0; i < indices_data_size; i+=2)
					{
						if (indices_data[i] == 0)
						{
							material_indices.push_back(indices_data[i+1]);
						}
						else
						{
							interior_material_indices.push_back(indices_data[i+1]);
						}
					}

					mesh_renderer->material_indices_number = material_indices.size();
					for (int i = 0; i < material_indices.size(); i++)
					{
						mesh->indices[i] = material_indices[i];
					}
					mesh_renderer->interior_material_indices_number = interior_material_indices.size();
					mesh_renderer->interior_material_indices_start = material_indices.size();
					for (int i = 0, j = material_indices.size(); i < interior_material_indices.size(); i++, j++)
					{
						mesh->indices[j] = interior_material_indices[i];
					}

					mesh->CreateVerticesFromData();
					Material* material = App->resources->GetMaterial(reader->getMaterialName(0));
					Material* interior_material = App->resources->GetMaterial(reader->getMaterialName(1));
					if (material)
					{
						mesh_renderer->SetMaterial(material);
						material->UnloadFromMemory();
					}
					if (interior_material)
					{
						mesh_renderer->SetMaterial(interior_material);
						interior_material->UnloadFromMemory();
					}
					mesh_renderer->SetMeshType(ComponentMeshRenderer::NormalMesh);
					mesh_renderer->SetMesh(mesh);
					mesh->UnloadFromMemory();
					App->mesh_importer->SaveMeshToLibrary(*mesh);
				}
				model->SetAssetsPath(path);
				model->SetLibraryPath(library_path);
				Data data;
				model->Save(data);
				data.SaveAsBinary(LIBRARY_BMODEL_FOLDER + name_without_extension + ".bmodel");
				RELEASE(model);
				NVBLAST_FREE(infl);
			}
		}
	}
	else
	{
		CONSOLE_ERROR("Associated .fbx file for %s not found! Place the .fbx created using Blast Editor in the same folder", name_without_extension.c_str());
	}
	
	return library_path;
}

BlastModel * ModuleBlastMeshImporter::LoadModelFromLibrary(std::string path, bool loaded_from_scene)
{
	BlastModel* model = nullptr;

	std::string model_name = App->file_system->GetFileNameWithoutExtension(path);
	Data data;
	if (data.LoadBinary(LIBRARY_BMODEL_FOLDER + model_name + ".bmodel"))
	{
		model = new BlastModel();
		if (!loaded_from_scene)
		{
			model->Load(data);
			ComponentBlast* blast = (ComponentBlast*)model->root->AddComponent(Component::CompBlast);
			blast->blast_model = model;
		}

		std::ifstream stream(path.c_str(), std::ios::binary);
		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios::end);
		size = stream.tellg() - size;
		stream.seekg(0, std::ios::beg);
		std::vector<char> buffer(size);
		stream.read(buffer.data(), buffer.size());
		stream.close();
		uint32_t objectTypeID;
		void* asset = serialization->deserializeFromBuffer(buffer.data(), buffer.size(), &objectTypeID);
		if (asset == nullptr)
		{
			CONSOLE_ERROR("Can't load .blast file.");
		}
		else
		{
			if (objectTypeID == Nv::Blast::ExtPxObjectTypeID::Asset)
			{
				const NvcVec3 inputScale = { 1, 1, 1 };
				model->m_pxAsset = reinterpret_cast<Nv::Blast::ExtPxAsset*>(asset);
				const Nv::Blast::TkAsset& tkAsset = model->m_pxAsset->getTkAsset();
				NvBlastAsset* llasset = const_cast<NvBlastAsset*>(tkAsset.getAssetLL());
				NvBlastExtAssetTransformInPlace(llasset, &inputScale, nullptr, nullptr);
				Nv::Blast::ExtPxSubchunk* subchunks = const_cast<Nv::Blast::ExtPxSubchunk*>(model->m_pxAsset->getSubchunks());
				for (uint32_t i = 0; i < model->m_pxAsset->getSubchunkCount(); ++i)
				{
					subchunks[i].geometry.scale.scale = physx::PxVec3(1);
				}

				const auto& actorDesc = model->m_pxAsset->getDefaultActorDesc();
				if (actorDesc.initialBondHealths)
				{
					float m_bondHealthMax = FLT_MIN;
					const uint32_t bondCount = model->m_pxAsset->getTkAsset().getBondCount();
					for (uint32_t i = 0; i < bondCount; ++i)
					{
						m_bondHealthMax = std::max<float>(m_bondHealthMax, actorDesc.initialBondHealths[i]);
					}
				}
				else
				{
					float m_bondHealthMax = actorDesc.uniformInitialBondHealth;
				}

				if (actorDesc.initialSupportChunkHealths)
				{
					float m_supportChunkHealthMax = FLT_MIN;
					const uint32_t nodeCount = model->m_pxAsset->getTkAsset().getGraph().nodeCount;
					for (uint32_t i = 0; i < nodeCount; ++i)
					{
						m_supportChunkHealthMax = std::max<float>(m_supportChunkHealthMax, actorDesc.initialSupportChunkHealths[i]);
					}
				}
				else
				{
					float m_supportChunkHealthMax = actorDesc.uniformInitialLowerSupportChunkHealth;
				}

				model->dmg_accel = NvBlastExtDamageAcceleratorCreate(model->m_pxAsset->getTkAsset().getAssetLL(), 3);
			}
		}
	}
	
	return model;
}
