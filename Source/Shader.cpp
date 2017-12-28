#include "Shader.h"
#include "Application.h"
#include "Data.h"
#include "ModuleShaderImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

Shader::Shader()
{
	shader_id = 0;
	SetType(Resource::ShaderResource);
	shader_type = ST_NULL;
}

Shader::~Shader()
{
}

void Shader::SetShaderType(ShaderType type)
{
	shader_type = type;
}

Shader::ShaderType Shader::GetShaderType() const
{
	return shader_type;
}

void Shader::SetID(uint id)
{
	shader_id = id;
}

uint Shader::GetID() const
{
	return shader_id;
}

void Shader::SetContent(std::string content)
{
	shader_text = content;
}

std::string Shader::GetContent() const
{
	return shader_text;
}

void Shader::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("texture_name", GetName());
	data.AddUInt("UUID", GetUID());
}

bool Shader::Load(Data & data)
{
	bool ret = true;
	std::string library_path = data.GetString("library_path");

	Shader* shader = App->shader_importer->LoadShaderFromLibrary(library_path);
	if (!shader)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			library_path = App->resources->CreateLibraryFile(Resource::TextureResource, assets_path);
			if (!library_path.empty())
			{
				Load(data);
			}
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		SetID(shader->GetID());
		shader_type = shader->shader_type;
		SetAssetsPath(data.GetString("assets_path"));
		SetLibraryPath(data.GetString("library_path"));
		SetName(data.GetString("texture_name"));
		SetUID(data.GetUInt("UUID"));
	}

	return ret;
}

void Shader::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.AddString("Name", GetName());
	data.AddUInt("Shader_Type", shader_type);

	data.SaveAsMeta(GetAssetsPath());
}

void Shader::LoadToMemory()
{
}

void Shader::UnloadFromMemory()
{
}

void Shader::SaveToPath(const char * path)
{
	std::string file_path = App->file_system->StringToPathFormat(path);


	std::ofstream outfile(file_path, std::ofstream::out);
	outfile << shader_text;
	outfile.close();
}
