#include "ModuleBlastMeshImporter.h"
#include <vector>
#include "Nvidia/Blast/Include/toolkit/NvBlastTkFramework.h"
#include "Nvidia/Blast/Include/toolkit/NvBlastTkAsset.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxAsset.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtPxSerialization.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtSerialization.h"
#include "Nvidia/Blast/Include/extensions/serialization/NvBlastExtTkSerialization.h"
#include "Nvidia/Blast/Include/extensions/assetutils/NvBlastExtAssetUtils.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Data.h"
#include <fstream>
#include "ModuleFileSystem.h"

ModuleBlastMeshImporter::ModuleBlastMeshImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Blast_Mesh_Importer";
}

ModuleBlastMeshImporter::~ModuleBlastMeshImporter()
{
}

bool ModuleBlastMeshImporter::Init(Data * editor_config)
{
	serialization = NvBlastExtSerializationCreate();
	/*if (serialization != nullptr && App->physics->GetPhysXPhysics() != nullptr && App->physics->GetPhysXCooking() != nullptr && framework != nullptr)
	{
		NvBlastExtTkSerializerLoadSet(*framework, *serialization);
		NvBlastExtPxSerializerLoadSet(*framework, *physx_physics, *cooking, *serialization);
		serialization->setSerializationEncoding(NVBLAST_FOURCC('C', 'P', 'N', 'B'));
	}*/
	return false;
}

std::string ModuleBlastMeshImporter::ImportMesh(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_MESHES_FOLDER + file_name;
	App->file_system->Copy(path, library_path);
	return library_path;
}

BlastMesh * ModuleBlastMeshImporter::LoadMeshFromLibrary(std::string path)
{
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
		CONSOLE_ERROR("Can't load .bmesh file.");
	}
	else
	{
		Nv::Blast::ExtPxAsset* m_pxAsset;
		if (objectTypeID == Nv::Blast::ExtPxObjectTypeID::Asset)
		{
			const NvcVec3 inputScale = { 1, 1, 1 };
			m_pxAsset = reinterpret_cast<Nv::Blast::ExtPxAsset*>(asset);
			const Nv::Blast::TkAsset& tkAsset = m_pxAsset->getTkAsset();
			NvBlastAsset* llasset = const_cast<NvBlastAsset*>(tkAsset.getAssetLL());
			NvBlastExtAssetTransformInPlace(llasset, &inputScale, nullptr, nullptr);
			Nv::Blast::ExtPxSubchunk* subchunks = const_cast<Nv::Blast::ExtPxSubchunk*>(m_pxAsset->getSubchunks());
			for (uint32_t i = 0; i < m_pxAsset->getSubchunkCount(); ++i)
			{
				subchunks[i].geometry.scale.scale = physx::PxVec3(1);
			}
		}
	}
	return nullptr;
}
