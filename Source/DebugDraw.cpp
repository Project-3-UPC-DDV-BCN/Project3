#include "DebugDraw.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ShaderProgram.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "Mesh.h"
#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Math\Quat.h"

void DrawLine(float3 start, float3 end)
{
	uint num_vertices = 2;
	uint num_indices = 2;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	vertices[0] = -1;
	vertices[1] = -1;
	vertices[2] = 0;

	vertices[3] = 1;
	vertices[4] = -1;
	vertices[5] = 0;

	indices[0] = 0;
	indices[1] = 1;

	Draw(vertices, num_vertices, indices, num_indices, GL_LINES);

	delete[] vertices;
	delete[] indices;
}

void Draw(float* vertices, uint num_vertices, uint* indices, uint num_indices, int mode)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	float4x4 trans = float4x4::FromTRS(float3(0, 0, 0), Quat::identity, float3(10, 10, 1));

	ShaderProgram* program = App->resources->GetShaderProgram("default_debug_program");

	glUseProgram(program->GetProgramID());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "view", App->renderer3D->editor_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "projection", App->renderer3D->editor_camera->GetProjectionMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());

	App->renderer3D->SetUniformVector4(program->GetProgramID(), "debug_color", float4(1.0f, 0.5f, 0.0f, 1.0f));

	//Mesh* plane = App->resources->GetMesh("PrimitivePlane");

	uint id_vertices_data = 0;
	uint id_indices = 0;

	uint vao = App->renderer3D->GenVertexArrayObject();

	glGenBuffers(1, &id_vertices_data);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STREAM_DRAW);

	//Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting vertex attributePointer %s\n", gluErrorString(error));
	}

	App->renderer3D->BindVertexArrayObject(vao);
	App->renderer3D->SetVertexAttributePointer(0, 3, 0, 0);
	App->renderer3D->EnableVertexAttributeArray(0);

	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STREAM_DRAW);

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind buffer: %s\n", gluErrorString(error));
	}

	glDrawElements(mode, num_indices, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &id_vertices_data);
	glDeleteBuffers(1, &id_indices);

	App->renderer3D->UnbindVertexArrayObject();
	App->renderer3D->DeleteVertexArrayObject(vao);
}