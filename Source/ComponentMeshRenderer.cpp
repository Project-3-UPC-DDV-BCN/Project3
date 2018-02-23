#include "ComponentMeshRenderer.h"
#include "GameObject.h"
#include "ModuleResources.h"
#include "Application.h"
#include "Mesh.h"
#include "Material.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Mesh_Renderer");
	SetType(ComponentType::CompMeshRenderer);
	SetGameObject(attached_gameobject);
	mesh = nullptr;
	material = App->resources->GetMaterial("default_material");
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{

}

Mesh* ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

void ComponentMeshRenderer::SetMesh(Mesh * mesh)
{
	if(mesh != nullptr)mesh->UnloadFromMemory();
	this->mesh = mesh;
	if (mesh != nullptr)mesh->LoadToMemory();
	UpdateBoundingBox();
}

Material * ComponentMeshRenderer::GetMaterial() const
{
	return material;
}

void ComponentMeshRenderer::SetMaterial(Material * material)
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

void ComponentMeshRenderer::UpdateBoundingBox()
{
	if (GetMesh() != nullptr)
	{
		GetMesh()->box.SetNegativeInfinity();
		GetMesh()->box.Enclose((float3*)GetMesh()->vertices, GetMesh()->num_vertices);
		math::OBB obb = GetMesh()->box.Transform(GetGameObject()->GetGlobalTransfomMatrix());
		GetMesh()->box = obb.MinimalEnclosingAABB();
	}
}

void ComponentMeshRenderer::LoadToMemory()
{
	if (mesh) mesh->LoadToMemory();
	/*if (material) material->LoadToMemory();*/
}

void ComponentMeshRenderer::UnloadFromMemory()
{
	if (mesh) mesh->UnloadFromMemory();
	if (material) material->UnloadFromMemory();
}


void ComponentMeshRenderer::Save(Data & data) const
{


	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.CreateSection("Mesh");
	if(mesh) mesh->Save(data);
	data.CloseSection();
	data.CreateSection("Material");
	if(material)material->Save(data);
	data.CloseSection();
}

void ComponentMeshRenderer::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	data.EnterSection("Mesh");
	uint mesh_uid = data.GetUInt("UUID");
	if (mesh_uid != 0)
	{
		mesh = App->resources->GetMesh(mesh_uid);
		if (!mesh)
		{
			mesh = new Mesh();
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


