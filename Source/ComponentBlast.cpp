#include "ComponentBlast.h"
#include "Material.h"
#include "Application.h"
#include "ModuleResources.h"
#include "BlastModel.h"
#include "ModuleBlast.h"
#include "ModuleBlastMeshImporter.h"
#include "GameObject.h"
#include <NvBlastExtPxFamily.h>
#include <NvBlastExtPxActor.h>
#include <PxRigidDynamic.h>
#include "Nvidia/PhysX/Include/foundation/PxMat44.h"
#include <NvBlastExtPxFamily.h>

ComponentBlast::ComponentBlast(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Blast");
	SetType(ComponentType::CompBlast);
	SetGameObject(attached_gameobject);
	interior_material = nullptr;
	blast_model = nullptr;
}

ComponentBlast::~ComponentBlast()
{
	if (blast_model && App->blast)
	{
		App->blast->CleanFamily(blast_model->family);
	}
}

void ComponentBlast::SetTransform(float* transform)
{
	if (blast_model != nullptr)
	{
		if (blast_model->family_created)
		{
			uint actors_num = blast_model->family->getActorCount();
			Nv::Blast::ExtPxActor* actors;
			uint written = blast_model->family->getActors(&actors, actors_num);
			physx::PxMat44 mat(transform);
			physx::PxTransform phys_transform(mat);
			for (int i = 0; i < written; i++)
			{
				actors->getPhysXActor().setGlobalPose(phys_transform);
			}
		}
	}
}

void ComponentBlast::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	if (interior_material)
	{
		data.CreateSection("IntMaterial");
		data.AddInt("UUID", interior_material->GetUID());
		data.CloseSection();
	}
	data.CreateSection("BlastModel");
	data.AddString("model_path", blast_model->GetLibraryPath());
	data.CloseSection();
}

void ComponentBlast::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	if (data.EnterSection("IntMaterial"))
	{
		int mat_id = data.GetUInt("UUID");
		interior_material = App->resources->GetMaterial(mat_id);
		data.LeaveSection();
	}
	data.EnterSection("BlastModel");
	if (!blast_model)
	{
		blast_model = App->blast_mesh_importer->LoadModelFromLibrary(data.GetString("model_path"), true);
		if (!blast_model)
		{
			CONSOLE_ERROR("model '%s' is null", data.GetString("model_path").c_str());
		}
	}
	if (blast_model)
	{
		if (!GetGameObject()->childs.empty())
		{
			blast_model->root = GetGameObject();
			int i = 0;
			blast_model->chunks.resize(blast_model->root->childs.size());
			for (GameObject* go : blast_model->root->childs)
			{
				blast_model->chunks[i] = go;
				i++;
			}
			App->blast->CreateFamily(blast_model);
			App->blast->SpawnFamily(blast_model, true);
			blast_model->SetLibraryPath(data.GetString("model_path"));
		}
	}
	data.LeaveSection();
}
