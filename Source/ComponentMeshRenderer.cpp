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
	interior_material = App->resources->GetMaterial("default_material");
	mesh_type = NormalMesh;

	material_indices_number = 0;
	material_indices_start = 0;
	interior_material_indices_number = 0;
	interior_material_indices_start = 0;
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
		//math::OBB obb = GetMesh()->box.Transform(GetGameObject()->GetGlobalTransfomMatrix());
		GetMesh()->box.TransformAsAABB(GetGameObject()->GetGlobalTransfomMatrix());
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
	if (mesh) data.AddString("mesh_name", mesh->GetName());
	data.CloseSection();
	data.CreateSection("Material");
	if(material) data.AddString("Name", material->GetName());
	data.CloseSection();
	data.CreateSection("Interior_Material");
	if (interior_material) data.AddUInt("UUID", interior_material->GetUID());
	data.CloseSection();
	//data.AddInt("Mesh_Type", mesh_type);
	data.AddInt("Mat_Indices_Number", material_indices_number);
	data.AddInt("Mat_Indices_Start", material_indices_start);
	data.AddInt("Int_Mat_Indices_Number", interior_material_indices_number);
	data.AddInt("Int_Mat_Indices_Start", interior_material_indices_start);
}

void ComponentMeshRenderer::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	data.EnterSection("Mesh");
	std::string mesh_name = data.GetString("mesh_name");
	mesh = App->resources->GetMesh(mesh_name);
	data.LeaveSection();
	data.EnterSection("Material");
	std::string material_name = data.GetString("Name");
	material = App->resources->GetMaterial(material_name);
	data.LeaveSection();
	data.EnterSection("Interior_Material");
	uint int_material_uid = data.GetUInt("UUID");
	if (int_material_uid != 0)
	{
		interior_material = App->resources->GetMaterial(int_material_uid);
	}
	data.LeaveSection();
	//mesh_type = (MeshType)data.GetInt("Mesh_Type");
	material_indices_number = data.GetInt("Mat_Indices_Number");
	material_indices_start = data.GetInt("Mat_Indices_Start");
	interior_material_indices_number = data.GetInt("Int_Mat_Indices_Number");
	interior_material_indices_start = data.GetInt("Int_Mat_Indices_Start");
	if (mesh)
	{
		UpdateBoundingBox();
	}
}

Material * ComponentMeshRenderer::GetInteriorMaterial() const
{
	return interior_material;
}

void ComponentMeshRenderer::SetInteriorMaterial(Material * material)
{
	interior_material = material;
}

void ComponentMeshRenderer::SetMeshType(MeshType type)
{
	mesh_type = type;
}

ComponentMeshRenderer::MeshType ComponentMeshRenderer::GetMeshType() const
{
	return mesh_type;
}


