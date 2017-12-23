#include "ModuleShaderImporter.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "Data.h"
#include "Shader.h"

ModuleShaderImporter::ModuleShaderImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Shader_Importer";
}

ModuleShaderImporter::~ModuleShaderImporter()
{
}

std::string ModuleShaderImporter::ImportShader(std::string path)
{
	std::string ret = "";

	if (!CompileShader(path, ret))
	{
		CONSOLE_ERROR("Can't compile %s. Please check for code errors.", path.c_str());
	}

	return ret;
}

Shader * ModuleShaderImporter::LoadShaderFromLibrary(std::string path)
{
	if (App->file_system->FileExist(path))
	{
		Data data;
		if (data.LoadBinary(path))
		{
			std::string extension = App->file_system->GetFileExtension(path);
			Shader* shader = new Shader();
			Shader::ShaderType type = Shader::ShaderType::ST_NULL;
			if (extension == ".vshader") type = Shader::ShaderType::ST_VERTEX;
			if (extension == ".fshader") type = Shader::ShaderType::ST_FRAGMENT;
			shader->SetShaderType(type);
			shader->SetContent(data.GetString("shader_text"));
			return shader;
		}
	}
	
	return nullptr;
}

bool ModuleShaderImporter::CompileShader(std::string path, std::string& library)
{
	std::string script_name = App->file_system->GetFileNameWithoutExtension(path);
	std::string extension = App->file_system->GetFileExtension(path);

	std::ifstream t(path.c_str());
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	if (str.empty()) return false;

	Data data;
	data.AddString("shader_text", str);
	data.SaveAsBinary(LIBRARY_SHADERS_FOLDER + script_name + extension);
	library = LIBRARY_SHADERS_FOLDER + script_name + extension;

	return true;
}
