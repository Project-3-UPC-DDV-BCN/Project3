#include "Shader.h"
#include "Application.h"
#include "Data.h"
#include "ModuleShaderImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"

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
	UpdateShader();
}

std::string Shader::GetContent() const
{
	return shader_text;
}

void Shader::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("shader_text", shader_text);
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
		SetName(data.GetString("shader_text"));
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

void Shader::UpdateShader()
{
	if (CompileShader())
		App->resources->OnShaderUpdate(this);
}

bool Shader::CompileShader()
{
	bool ret = false;
	switch (shader_type)
	{
	case ST_VERTEX:
		shader_id = App->renderer3D->CreateVertexShader(shader_text.c_str());
		break;
	case ST_FRAGMENT:
		shader_id = App->renderer3D->CreateFragmentShader(shader_text.c_str());
		break;
	}
	if (shader_id != 0)
	{
		CONSOLE_DEBUG("Shader compilation Success :)");
		ret = true;
	}
	return ret;
}
