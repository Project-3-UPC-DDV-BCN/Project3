#include "DebugDraw.h"
#include "Application.h"
#include "ModuleResources.h"
#include "Glew\include\glew.h"
#include "SDL\include\SDL_opengl.h"
#include "ShaderProgram.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "Mesh.h"
#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Math\Quat.h"

void DrawLine(float3 start, float3 end)
{
	uint num_vertices = 2;
	uint num_indices = 3;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	vertices[0] = start.x;
	vertices[1] = start.y;
	vertices[2] = start.z;

	vertices[3] = end.x;
	vertices[4] = end.y;
	vertices[5] = end.z;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 0;

	Draw(vertices, num_vertices, indices, num_indices);

	delete[] vertices;
	delete[] indices;
}

void Draw(float* vertices, uint num_vertices, uint* indices, uint num_indices)
{

	float4x4 trans = float4x4::FromTRS(float3(0, 0, 0), Quat::identity, float3(10, 10, 2));

	ShaderProgram* program = App->resources->GetShaderProgram("default_shader_program");

	glUseProgram(program->GetProgramID());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "view", App->renderer3D->editor_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "projection", App->renderer3D->editor_camera->GetProjectionMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());
	App->renderer3D->SetUniformBool(program->GetProgramID(), "has_texture", false);
	//App->renderer3D->SetUniformBool(program->GetProgramID(), "has_material_color", false);

	App->renderer3D->SetUniformBool(program->GetProgramID(), "has_material_color", true);
	App->renderer3D->SetUniformVector4(program->GetProgramID(), "material_color", float4(1.0f, 0.5f, 0.0f, 1.0f));

	//Mesh* plane = App->resources->GetMesh("PrimitivePlane");
	//
	//if (plane->id_indices == 0)
	//{
	//	plane->LoadToMemory();
	//}

	//App->renderer3D->BindVertexArrayObject(plane->id_vao);

	//glDrawElements(GL_TRIANGLES, plane->num_indices, GL_UNSIGNED_INT, NULL);

	//App->renderer3D->UnbindVertexArrayObject();

	uint id_vertices_data = 0;
	uint id_indices = 0;

	glGenBuffers(1, &id_vertices_data);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STREAM_DRAW);

	App->renderer3D->SetVertexAttributePointer(0, 3, 0, 0);
	App->renderer3D->EnableVertexAttributeArray(0);

	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*num_indices, indices, GL_STREAM_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind buffer: %s\n", gluErrorString(error));
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &id_vertices_data);
	glDeleteBuffers(1, &id_indices);
}