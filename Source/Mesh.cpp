#include "Mesh.h"
#include "OpenGL.h"
#include "Data.h"
#include "Application.h"
#include "ModuleMeshImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

Mesh::Mesh()
{
	id_vertices_data = 0;
	num_indices = 0;
	indices = nullptr;

	id_indices = 0;
	num_vertices = 0;

	SetType(Resource::MeshResource);

}

Mesh::~Mesh()
{
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vertices_data);

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
			App->resources->CreateLibraryFile(Resource::TextureResource, assets_path);
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

	return ret;
}

void Mesh::CreateMeta() const
{

}

void Mesh::LoadToMemory()
{
	glGenBuffers(1, &id_vertices_data);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 13, vertices_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
			memcpy(vertices + i * 3, vertices_data + i * 13, sizeof(float) * 3);
		}
	}
}

void Mesh::InitializeMesh()
{

}
