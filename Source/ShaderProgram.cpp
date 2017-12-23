#include "ShaderProgram.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Data.h"

ShaderProgram::ShaderProgram()
{
	vertex_shader = nullptr;
	fragment_shader = nullptr;
	program_id = 0;
}

ShaderProgram::~ShaderProgram()
{
	SaveToLibray();
}

void ShaderProgram::SetVertexShader(Shader * shader)
{
	if (shader != vertex_shader)
	{
		vertex_shader = shader;
	}
}

void ShaderProgram::SetFragmentShader(Shader * shader)
{
	if (shader != fragment_shader)
	{
		fragment_shader = shader;
	}
}

void ShaderProgram::SetShaders(Shader * vert, Shader * frag)
{
	if (vert != vertex_shader)
	{
		vertex_shader = vert;
	}
	if (frag != fragment_shader)
	{
		fragment_shader = frag;
	}
}

Shader * ShaderProgram::GetVertexShader() const
{
	return vertex_shader;
}

Shader * ShaderProgram::GetFragmentShader() const
{
	return fragment_shader;
}

void ShaderProgram::UseProgram()
{
	App->renderer3D->UseShaderProgram(program_id);
}

void ShaderProgram::SaveToLibray() const
{
	uint size = App->renderer3D->GetProgramSize(program_id);
	char* buff = new char[size];
	uint ret_size = App->renderer3D->GetProgramBinary(program_id, size, buff);

	if (ret_size > size) //something went wrong
	{
		RELEASE_ARRAY(buff);
		return;
	}
	else
	{
		if (ret_size < size) size = ret_size;

		std::string library_path = LIBRARY_SHADERS_FOLDER  "ShaderProgram_" + vertex_shader->GetName() + "_" + fragment_shader->GetName() + ".shprog";
		std::ofstream outfile(library_path.c_str(), std::ofstream::binary);
		outfile.write(buff, size);
		outfile.close();
	}

}

void ShaderProgram::LoadFromLibrary(const char* path)
{
	std::ifstream file(path, std::ifstream::binary);
	if (file.is_open())
	{
		// get length of file:
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		char * buffer = new char[length];
		file.read(buffer, length);

		if (file)
		{
			if (program_id != 0)
				App->renderer3D->DeleteProgram(program_id);

			program_id = App->renderer3D->CreateShaderProgram();
			App->renderer3D->LoadProgramFromBinary(program_id, length, buffer);
		}
	}
}

void ShaderProgram::LinkShaderProgram()
{
	if (program_id != 0)
	{
		App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}

	program_id = App->renderer3D->CreateShaderProgram();
	App->renderer3D->AttachShaderToProgram(program_id, vertex_shader->GetID());
	App->renderer3D->AttachShaderToProgram(program_id, fragment_shader->GetID());
	if (App->renderer3D->LinkProgram(program_id) == false)
	{
		CONSOLE_ERROR("Error while linking shader program, check errors above.");
		App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}
	else CONSOLE_ERROR("Shader Program %d created :)", program_id);
}

uint ShaderProgram::GetProgramID() const
{
	return program_id;
}

void ShaderProgram::Save(Data & data) const
{
}

bool ShaderProgram::Load(Data & data)
{
	return false;
}

void ShaderProgram::CreateMeta() const
{
}

void ShaderProgram::LoadToMemory()
{
}

void ShaderProgram::UnloadFromMemory()
{
}
