#include "Mesh.h"
#include "OpenGL.h"
#include "Data.h"
#include "Application.h"
#include "ModuleMeshImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"

Mesh::Mesh()
{
	id_vertices_data = 0;
	num_indices = 0;
	indices = nullptr;

	id_indices = 0;
	num_vertices = 0;

	box.minPoint = { 0,0,0 };
	box.maxPoint = { 0,0,0 };

	SetType(Resource::MeshResource);

}

Mesh::~Mesh()
{
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vertices_data);
	RELEASE_ARRAY(vertices);

	UnloadFromMemory();
}

void Mesh::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("mesh_name", GetName());
	data.AddUInt("UUID", GetUID());
}

bool Mesh::Load(Data & data)
{
	bool ret = true;

	std::string library_path = data.GetString("library_path");
	Mesh* mesh = App->mesh_importer->LoadMeshFromLibrary(library_path);
	if (!mesh)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			App->resources->CreateLibraryFile(Resource::MeshResource, assets_path);
			Load(data);
		}
		else
		{
			std::string name = data.GetString("mesh_name");
			CONSOLE_ERROR("Mesh %s not found! fbx is missing!", name.c_str());
			ret = false;
		}
	}
	else
	{
		SetUID(data.GetUInt("UUID"));
		SetAssetsPath(data.GetString("assets_path"));
		SetLibraryPath(data.GetString("library_path"));
		SetName(data.GetString("mesh_name"));

		num_indices = mesh->num_indices;
		indices = mesh->indices;
		num_vertices = mesh->num_vertices;
		vertices_data = mesh->vertices_data;
		App->resources->AddMesh(this);
	}
	CreateVerticesFromData();

	return ret;
}

void Mesh::CreateMeta() const
{

}

void Mesh::LoadToMemory()
{
	if (id_vertices_data == 0)
	{
		glGenBuffers(1, &id_vertices_data);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 19, vertices_data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &id_indices);
		glBindBuffer(GL_ARRAY_BUFFER, id_indices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		InitializeMesh();
	}

	IncreaseUsedCount();
}

void Mesh::UnloadFromMemory()
{
	DecreaseUsedCount();

	if (GetUsedCount() == 0)
	{
		glDeleteBuffers(1, &id_vertices_data);

		id_vertices_data = 0;
	}
}

void Mesh::CreateVerticesFromData()
{
	if (vertices == nullptr && vertices_data != nullptr)
	{
		vertices = new float[num_vertices * 3];

		for (int i = 0; i < num_vertices; ++i)
		{
			memcpy(vertices + i * 3, vertices_data + i * 19, sizeof(float) * 3);
		}
	}
}

void Mesh::InitializeMesh()
{
	id_vao = App->renderer3D->GenVertexArrayObject();

	App->renderer3D->BindVertexArrayObject(id_vao);

	App->renderer3D->BindArrayBuffer(id_vertices_data);

	//vertices
	App->renderer3D->SetVertexAttributePointer(0, 3, 19, 0);
	App->renderer3D->EnableVertexAttributeArray(0);
	//texture coords
	App->renderer3D->SetVertexAttributePointer(1, 3, 19, 3);
	App->renderer3D->EnableVertexAttributeArray(1);
	//normals
	App->renderer3D->SetVertexAttributePointer(2, 3, 19, 6);
	App->renderer3D->EnableVertexAttributeArray(2);
	//colors
	App->renderer3D->SetVertexAttributePointer(3, 4, 19, 9);
	App->renderer3D->EnableVertexAttributeArray(3);
	//tangents
	App->renderer3D->SetVertexAttributePointer(4, 3, 19, 13);
	App->renderer3D->EnableVertexAttributeArray(4);
	//bitangents
	App->renderer3D->SetVertexAttributePointer(5, 3, 19, 16);
	App->renderer3D->EnableVertexAttributeArray(5);

	App->renderer3D->BindElementArrayBuffer(id_indices);

	App->renderer3D->UnbindVertexArrayObject();
}
