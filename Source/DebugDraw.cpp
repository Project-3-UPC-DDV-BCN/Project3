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
	uint num_vertices = 3;
	uint num_indices = 3;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	vertices[0] = -1;
	vertices[1] = -1;
	vertices[2] = 0;

	vertices[3] = 1;
	vertices[4] = -1;
	vertices[5] = 0;

	vertices[6] = 0;
	vertices[7] = 1;
	vertices[8] = 0;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	Draw(vertices, num_vertices, indices, num_indices);

	delete[] vertices;
	delete[] indices;
}

void Draw(float* vertices, uint num_vertices, uint* indices, uint num_indices)
{
	float4x4 trans = float4x4::FromTRS(float3(0, 0, 0), Quat::identity, float3(100, 100, 1));

	ShaderProgram* program = App->resources->GetShaderProgram("default_shader_program");

	glUseProgram(program->GetProgramID());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "view", App->renderer3D->editor_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "projection", App->renderer3D->editor_camera->GetProjectionMatrix());
	App->renderer3D->SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());
	App->renderer3D->SetUniformBool(program->GetProgramID(), "has_texture", false);
	App->renderer3D->SetUniformBool(program->GetProgramID(), "has_material_color", false);

	App->renderer3D->SetUniformBool(program->GetProgramID(), "has_material_color", true);
	App->renderer3D->SetUniformVector4(program->GetProgramID(), "material_color", float4(1.0f, 0.5f, 0.0f, 1.0f));

	Mesh* plane = App->resources->GetMesh("PrimitivePlane");

	uint id_vertices_data = 0;
	uint id_indices = 0;

	glGenBuffers(1, &id_vertices_data);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STREAM_DRAW);

	App->renderer3D->BindArrayBuffer(id_vertices_data);

	App->renderer3D->SetVertexAttributePointer(0, 3, 0, 0);
	App->renderer3D->EnableVertexAttributeArray(0);


	glGenBuffers(1, &id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STREAM_DRAW);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind buffer: %s\n", gluErrorString(error));
	}

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &id_vertices_data);
	glDeleteBuffers(1, &id_indices);

	App->renderer3D->UnbindVertexArrayObject();

}