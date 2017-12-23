#include "ShaderProgram.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

ShaderProgram::ShaderProgram()
{
	vertex_shader = nullptr;
	fragment_shader = nullptr;
	program_id = 0;
}

ShaderProgram::~ShaderProgram()
{
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
	//App->renderer3D->UseShaderProgram(program_id);
}

void ShaderProgram::SaveToLibray() const
{
	
}

void ShaderProgram::LinkShaderProgram()
{
	if (program_id != 0)
	{
		//App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}

	/*program_id = App->renderer3D->CreateShaderProgram();
	App->renderer3D->AttachShaderToProgram(program_id, vertex_shader->GetID());
	App->renderer3D->AttachShaderToProgram(program_id, fragment_shader->GetID());
	if (App->renderer3D->LinkProgram(program_id) == false)
	{
		CONSOLE_ERROR("Error while linking shader program, check errors above.");
		App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}*/
	else CONSOLE_ERROR("Shader Program %d created :)", program_id);
}

uint ShaderProgram::GetProgramID() const
{
	return program_id;
}
