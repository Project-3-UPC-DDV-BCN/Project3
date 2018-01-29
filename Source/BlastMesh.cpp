#include "BlastMesh.h"
#include "OpenGL.h"
#include "Data.h"
#include "Application.h"
#include "ModuleBlastMeshImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

BlastMesh::BlastMesh()
{
	id_vertices = 0;
	num_indices = 0;
	indices = nullptr;

	id_indices = 0;
	num_vertices = 0;
	vertices = nullptr;

	id_normals = 0;
	normals = nullptr;

	id_colors = 0;
	colors = nullptr;

	id_texture_coords = 0;
	texture_coords = nullptr;

	interior_material = nullptr;

	SetType(Resource::BlastMeshResource);
}

BlastMesh::~BlastMesh()
{
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(colors);
	RELEASE_ARRAY(texture_coords);
	RELEASE(interior_material);

	UnloadFromMemory();
}

void BlastMesh::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("mesh_name", GetName());
	data.AddUInt("UUID", GetUID());
}

bool BlastMesh::Load(Data & data)
{
	bool ret = true;

	std::string library_path = data.GetString("library_path");
	BlastMesh* mesh = App->blast_mesh_importer->LoadMeshFromLibrary(library_path);
	if (!mesh)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			App->resources->CreateLibraryFile(Resource::BlastMeshResource, assets_path);
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
		vertices = mesh->vertices;
		normals = mesh->normals;
		colors = mesh->colors;
		texture_coords = mesh->texture_coords;
		App->resources->AddMesh(this);
	}

	return ret;
}

void BlastMesh::CreateMeta() const
{

}

void BlastMesh::LoadToMemory()
{
	glGenBuffers(1, &id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (normals != nullptr)
	{
		glGenBuffers(1, &(id_normals));
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (colors != nullptr)
	{
		glGenBuffers(1, &(id_colors));
		glBindBuffer(GL_ARRAY_BUFFER, id_colors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, colors, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (texture_coords != nullptr)
	{
		glGenBuffers(1, &(id_texture_coords));
		glBindBuffer(GL_ARRAY_BUFFER, id_texture_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, texture_coords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	IncreaseUsedCount();
}

void BlastMesh::UnloadFromMemory()
{
	DecreaseUsedCount();

	if (GetUsedCount() == 0)
	{
		glDeleteBuffers(1, &id_vertices);
		glDeleteBuffers(1, &id_indices);
		glDeleteBuffers(1, &id_normals);
		glDeleteBuffers(1, &id_colors);
		glDeleteBuffers(1, &id_texture_coords);

		id_vertices = 0;
		id_indices = 0;
		id_normals = 0;
		id_colors = 0;
		id_texture_coords = 0;
	}
}
