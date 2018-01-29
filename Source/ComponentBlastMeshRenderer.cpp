#include "ComponentBlastMeshRenderer.h"
#include "GameObject.h"
#include "ModuleResources.h"
#include "Application.h"
#include "BlastMesh.h"
#include "Material.h"
#include "MathGeoLib/Geometry/OBB.h"

ComponentBlastMeshRenderer::ComponentBlastMeshRenderer(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Blast_Mesh_Renderer");
	SetType(ComponentType::CompBlastMeshRenderer);
	SetGameObject(attached_gameobject);
	mesh = nullptr;
	material = nullptr;
	interior_material = nullptr;
}

ComponentBlastMeshRenderer::~ComponentBlastMeshRenderer()
{

}

BlastMesh* ComponentBlastMeshRenderer::GetMesh() const
{
	return mesh;
}

void ComponentBlastMeshRenderer::SetMesh(BlastMesh * mesh)
{
	if (mesh != nullptr)mesh->UnloadFromMemory();
	this->mesh = mesh;
	if (mesh != nullptr)mesh->LoadToMemory();
	UpdateBoundingBox();
}

Material * ComponentBlastMeshRenderer::GetMaterial() const
{
	return material;
}

void ComponentBlastMeshRenderer::SetMaterial(Material * material)
{
	if (this->material != material)
	{
		if (this->material != nullptr)
		{
			this->material->DecreaseUsedCount();
			this->material->DecreaseUsedTexturesCount();
		}
		this->material = material;
		if (this->material != nullptr)
		{
			this->material->IncreaseUsedCount();
			this->material->IncreaseUsedTexturesCount();
		}
	}
}

void ComponentBlastMeshRenderer::UpdateBoundingBox()
{
	if (GetMesh() != nullptr)
	{
		GetMesh()->box.SetNegativeInfinity();
		GetMesh()->box.Enclose((float3*)GetMesh()->vertices, GetMesh()->num_vertices);
		math::OBB obb = GetMesh()->box.Transform(GetGameObject()->GetGlobalTransfomMatrix());
		GetMesh()->box = obb.MinimalEnclosingAABB();
	}
}

void ComponentBlastMeshRenderer::LoadToMemory()
{
	if (mesh) mesh->LoadToMemory();
	/*if (material) material->LoadToMemory();*/
}

void ComponentBlastMeshRenderer::UnloadFromMemory()
{
	if (mesh) mesh->UnloadFromMemory();
	if (material) material->UnloadFromMemory();
}


void ComponentBlastMeshRenderer::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.CreateSection("Mesh");
	if (mesh) mesh->Save(data);
	data.CloseSection();
	data.CreateSection("Material");
	if (material)material->Save(data);
	data.CloseSection();
}

void ComponentBlastMeshRenderer::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	data.EnterSection("Mesh");
	uint mesh_uid = data.GetUInt("UUID");
	if (mesh_uid != 0)
	{
		mesh = App->resources->GetBlastMesh(mesh_uid);
		if (!mesh)
		{
			mesh = new BlastMesh();
			mesh->Load(data);
		}
	}
	data.LeaveSection();
	data.EnterSection("Material");
	uint material_uid = data.GetUInt("UUID");
	if (material_uid != 0)
	{
		material = App->resources->GetMaterial(material_uid);
		if (!material)
		{
			material = new Material();
			material->Load(data);
		}
	}
	data.LeaveSection();
}
