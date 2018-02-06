#include "ModuleBlastMeshImporter.h"
#include <vector>
#include "Nvidia/Blast/Include/toolkit/NvBlastTkAsset.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxAsset.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtPxSerialization.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtSerialization.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtTkSerialization.h"
#include "Nvidia/Blast/Include/extensions/assetutils/NvBlastExtAssetUtils.h"
#include "Nvidia/Blast/Include/extensions/shaders/NvBlastExtDamageShaders.h"
#include "Nvidia/Blast/Include/extensions/exporter/NvBlastExtExporter.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Data.h"
#include <fstream>
#include "ModuleFileSystem.h"
#include "ModuleBlast.h"
#include "BlastModel.h"
#include "Mesh.h"
#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "ModuleResources.h"
#include "ModuleMeshImporter.h"

#if _DEBUG
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtAssetUtilsDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtPxSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtTkSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastTkDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtShadersDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtExporterDEBUG_x86.lib")
#else
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtAssetUtils_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtPxSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtTkSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastTk_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtShaders_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtExporter_x86.lib")
#endif

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

std::string ModuleBlastMeshImporter::ImportMesh(std::string path)
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
				model->chunks.resize(reader->getBoneCount());
				for (uint bone_index = 0; bone_index < reader->getBoneCount(); ++bone_index)
				{
					GameObject* go = new GameObject();
					if (bone_index == 0)
					{
						go->SetName(name_without_extension);
					}
					else
					{
						go->SetName(name_without_extension + "_chunk_" + std::to_string(bone_index));
						go->SetActive(false);
					}
					ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)go->AddComponent(Component::CompMeshRenderer);
					Mesh* mesh = new Mesh();
					mesh->SetName(name_without_extension + "_chunk_" + std::to_string(bone_index));
					mesh->num_vertices = reader->getVerticesCount();
					mesh->vertices_data = new float[mesh->num_vertices * 13];
					physx::PxVec3* vertice_pos = reader->getPositionArray();
					physx::PxVec2* vertice_uv = reader->getUvArray();
					physx::PxVec3* vertice_normals = reader->getNormalsArray();
					for (int i = 0, j = 0; i < mesh->num_vertices; i++, j += 13)
					{
						mesh->vertices_data[j] = vertice_pos[i].x;
						mesh->vertices_data[j + 1] = vertice_pos[i].y;
						mesh->vertices_data[j + 2] = vertice_pos[i].z;
						mesh->vertices_data[j + 3] = vertice_uv[i].x;
						mesh->vertices_data[j + 4] = vertice_uv[i].y;
						mesh->vertices_data[j + 5] = 0;
						mesh->vertices_data[j + 6] = vertice_normals[i].x;
						mesh->vertices_data[j + 7] = vertice_normals[i].y;
						mesh->vertices_data[j + 8] = vertice_normals[i].z;
						mesh->vertices_data[j + 9] = 1;
						mesh->vertices_data[j + 10] = 1;
						mesh->vertices_data[j + 11] = 1;
						mesh->vertices_data[j + 12] = 1;
					}
					mesh->num_indices = reader->getIndicesCount();
					mesh->indices = new uint[mesh->num_indices];
					uint* indice = reader->getIndexArray();
					std::vector<uint> material_indices;
					std::vector<uint> interior_material_indices;
					for (uint i = 0, j = 0; i < mesh->num_indices; i += 3, j++)
					{
						int mat_index = reader->getMaterialIds()[j];
						if (mat_index == 0)
						{
							material_indices.push_back(indice[i]);
							material_indices.push_back(indice[i + 1]);
							material_indices.push_back(indice[i + 2]);
						}
						else
						{
							interior_material_indices.push_back(indice[i]);
							interior_material_indices.push_back(indice[i + 1]);
							interior_material_indices.push_back(indice[i + 2]);
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
					}
					if (interior_material)
					{
						mesh_renderer->SetMaterial(interior_material);
					}
					mesh_renderer->SetMeshType(ComponentMeshRenderer::BlastMesh);
					mesh_renderer->SetMesh(mesh);
					model->chunks[bone_index] = go;
					App->mesh_importer->SaveMeshToLibrary(*mesh);
				}
				model->SetAssetsPath(path);
				model->SetLibraryPath(library_path);
				model->SetName(name_without_extension);
				Data data;
				model->Save(data);
				data.SaveAsBinary(LIBRARY_BMODEL_FOLDER + name_without_extension + ".bmodel");
				RELEASE(model);
			}
		}
	}
	else
	{
		CONSOLE_ERROR("Associated .fbx file for %s not found! Place the .fbx created using Blast Editor in the same folder", name_without_extension.c_str());
	}
	
	return library_path;
}

BlastModel * ModuleBlastMeshImporter::LoadModelFromLibrary(std::string path)
{
	BlastModel* model = nullptr;

	std::string model_name = App->file_system->GetFileNameWithoutExtension(path);
	Data data;
	if (data.LoadBinary(LIBRARY_BMODEL_FOLDER + model_name + ".bmodel"))
	{
		model = new BlastModel();

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
				App->blast->CreateFamily(model);
			}
		}
		model->Load(data);
	}

	return model;
}
