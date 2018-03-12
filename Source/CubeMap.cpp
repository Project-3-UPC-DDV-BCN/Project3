#include "CubeMap.h"
#include "Application.h"
#include "ModuleTextureImporter.h"
#include "OpenGL.h"
#include "Primitive.h"
#include "Texture.h"
#include "MathGeoLib\MathGeoLib.h"
#include "ShaderProgram.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "Mesh.h"
#include "ModuleResources.h"

CubeMap::CubeMap(float size) : size(size)
{
	textures[0] = textures[1] = textures[2] = textures[3] = textures[4] = textures[5] = nullptr;
	cube_map_id = 0;
}

CubeMap::~CubeMap()
{
	DeleteCubeMap();
}

void CubeMap::CreateCubeMap()
{
	glGenTextures(1, &cube_map_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_id);

	for (int i = 0; i < 6; i++)
	{
		textures[i] = App->texture_importer->LoadTextureFromLibrary(textures_path[i]);

		if (textures[i] != nullptr)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, textures[i]->GetWidth(), textures[i]->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[i]->GetImageData());
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
}

void CubeMap::RenderCubeMap(float3 position, ComponentCamera* active_camera)
{
	float4x4 trans = float4x4::FromTRS(position, Quat::identity, float3(size, size, size));

	ShaderProgram* program = App->resources->GetShaderProgram("cubemap_shader_program");
	App->renderer3D->UseShaderProgram(program->GetProgramID());

	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "view", active_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "projection", active_camera->GetProjectionMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());

	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_id);

	Mesh* cube = App->resources->GetMesh("PrimitiveCube");
	if (cube->id_indices == 0) cube->LoadToMemory();

	glDisable(GL_CULL_FACE);

	App->renderer3D->BindVertexArrayObject(cube->id_vao);
	glDrawElements(GL_TRIANGLES, cube->num_indices, GL_UNSIGNED_INT, NULL);
	App->renderer3D->UnbindVertexArrayObject();

	glEnable(GL_CULL_FACE);
}

void CubeMap::DeleteCubeMap()
{
	/*for (int i = 0; i < 6; i++)
	{
		glDeleteTextures(6, )
	}*/
}

void CubeMap::SetCubeMapTopTexture(std::string texture_path)
{
	textures_path[3] = texture_path;
}

void CubeMap::SetCubeMapLeftTexture(std::string texture_path)
{
	textures_path[1] = texture_path;
}

void CubeMap::SetCubeMapFrontTexture(std::string texture_path)
{
	textures_path[4] = texture_path;
}

void CubeMap::SetCubeMapRightTexture(std::string texture_path)
{
	textures_path[0] = texture_path;
}

void CubeMap::SetCubeMapBackTexture(std::string texture_path)
{
	textures_path[5] = texture_path;
}

void CubeMap::SetCubeMapBottomTexture(std::string texture_path)
{
	textures_path[2] = texture_path;
}
