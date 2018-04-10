#include "DebugDraw.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ShaderProgram.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "Mesh.h"
#include "MathGeoLib\float4x4.h"
#include "MathGeoLib\Quat.h"
#include "MathGeoLib/float4x4.h"
#include "MathGeoLib/Quat.h"
#include "ComponentCamera.h"

DebugShape::DebugShape(uint _num_vertices, float * _vertices, uint _num_indices, uint * _indices)
{
	if (_num_vertices > 0)
	{
		// Vertices
		vertices = new float[_num_vertices * 3];
		memcpy(vertices, _vertices, sizeof(float) * _num_vertices * 3);
		num_vertices = _num_vertices;

		if (_num_indices > 0)
		{
			// Indices
			indices = new uint[_num_indices];
			memcpy(indices, _indices, sizeof(uint) * _num_indices);
			num_indices = _num_indices;
		}
	}

	mode = GL_TRIANGLES;
	transform = float4x4::identity;
	colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
}

void DebugShape::CleanUp()
{
	num_vertices = 0;
	num_vertices = 0;

	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
}

void DebugShape::SetTransform(float4x4 _transform)
{
	transform = _transform;
}

void DebugShape::SetColour(float4 _colour)
{
	colour = _colour;
}

void DebugShape::SetMode(int _mode)
{
	mode = _mode;
}

void DebugShape::SetStroke(uint _stroke)
{
	stroke = _stroke;
}

uint DebugShape::GetNumVertices()
{
	return num_vertices;
}

float * DebugShape::GetVertices()
{
	return vertices;
}

uint DebugShape::GetNumIndices()
{
	return num_indices;
}

uint * DebugShape::GetIndices()
{
	return indices;
}

float4 DebugShape::GetColour()
{
	return colour;
}

float4x4 DebugShape::GetTransform()
{
	return transform;
}

int DebugShape::GetMode()
{
	return mode;
}

uint DebugShape::GetStroke()
{
	return stroke;
}

DebugDraw::DebugDraw()
{
	SetLineStroke(2);
}

void DebugDraw::Line(float3 start, float3 end, float4 colour)
{
	uint num_vertices = 2;
	uint num_indices = 2;

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

	DebugShape shape(num_vertices, vertices, num_indices, indices);
	shape.SetMode(GL_LINES);
	shape.SetColour(colour);
	shape.SetStroke(line_stroke);

	AddShape(shape);

	delete[] vertices;
	delete[] indices;
}

void DebugDraw::Quad(float3 center, float2 size, float4 colour)
{
	uint num_vertices = 4;
	uint num_indices = 8;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	float3 up_left = float3(center.x - (size.x/2), center.y + (size.y/2), center.z);
	float3 down_left = float3(center.x - (size.x/2), center.y - (size.y/2), center.z);
	float3 down_right = float3(center.x + (size.x / 2), center.y - (size.y / 2), center.z);
	float3 up_right = float3(center.x + (size.x / 2), center.y + (size.y / 2), center.z);

	vertices[0] = up_left.x;
	vertices[1] = up_left.y;
	vertices[2] = up_left.z;

	vertices[3] = down_left.x;
	vertices[4] = down_left.y;
	vertices[5] = down_left.z;

	vertices[6] = down_right.x;
	vertices[7] = down_right.y;
	vertices[8] = down_right.z;

	vertices[9] = up_right.x;
	vertices[10] = up_right.y;
	vertices[11] = up_right.z;

	indices[0] = 0;
	indices[1] = 1;

	indices[2] = 1;
	indices[3] = 2;

	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 3;
	indices[7] = 0;

	DebugShape shape(num_vertices, vertices, num_indices, indices);
	shape.SetMode(GL_LINES);
	shape.SetColour(colour);

	shape.SetStroke(line_stroke);

	AddShape(shape);

	delete[] vertices;
	delete[] indices;
}

void DebugDraw::Quad(float4x4 transform, float2 size, float4 colour)
{
	uint num_vertices = 4;
	uint num_indices = 8;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	float3 up_left = float3(-(size.x / 2), (size.y / 2), 0);
	float3 down_left = float3(-(size.x / 2), -(size.y / 2), 0);
	float3 down_right = float3((size.x / 2), -(size.y / 2), 0);
	float3 up_right = float3((size.x / 2), (size.y / 2), 0);

	vertices[0] = up_left.x;
	vertices[1] = up_left.y;
	vertices[2] = up_left.z;

	vertices[3] = down_left.x;
	vertices[4] = down_left.y;
	vertices[5] = down_left.z;

	vertices[6] = down_right.x;
	vertices[7] = down_right.y;
	vertices[8] = down_right.z;

	vertices[9] = up_right.x;
	vertices[10] = up_right.y;
	vertices[11] = up_right.z;

	indices[0] = 0;
	indices[1] = 1;

	indices[2] = 1;
	indices[3] = 2;

	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 3;
	indices[7] = 0;

	DebugShape shape(num_vertices, vertices, num_indices, indices);
	shape.SetMode(GL_LINES);
	shape.SetTransform(transform);
	shape.SetColour(colour);
	shape.SetStroke(line_stroke);

	AddShape(shape);

	delete[] vertices;
	delete[] indices;
}

void DebugDraw::Circle(float4x4 transform, float rad, float4 colour, uint resolution)
{
	int slices = resolution;

	float angle_slice = 360 / slices;

	uint num_vertices = slices;
	uint num_indices = slices*2;

	float* vertices = new float[num_vertices * 3];
	uint* indices = new uint[num_indices];

	float curr_angle = 0;
	for(int i = 0; i < slices; i++)
	{
		vertices[(i * 3)] = cos(curr_angle * DEGTORAD) * rad;
		vertices[(i * 3) + 1] = sin(curr_angle * DEGTORAD) * rad;
		vertices[(i * 3) + 2] = 0;

		curr_angle += angle_slice;

		indices[i*2] = i;

		if (i+1 < slices)
			indices[(i * 2) + 1] = i + 1;
		else
			indices[(i * 2) + 1] = 0;
	}

	DebugShape shape(num_vertices, vertices, num_indices, indices);
	shape.SetMode(GL_LINES);
	shape.SetTransform(transform);
	shape.SetColour(colour);
	shape.SetStroke(line_stroke);

	AddShape(shape);

	delete[] vertices;
	delete[] indices;
}

void DebugDraw::SetLineStroke(uint stroke)
{
	line_stroke = stroke;
}

uint DebugDraw::GetLineStroke() const
{
	return line_stroke;
}

void DebugDraw::Render(ComponentCamera* camera)
{
	BROFILER_CATEGORY("Debug Render", Profiler::Color::CadetBlue);
	// Activate
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render ----

	ShaderProgram* program = App->resources->GetShaderProgram("default_debug_program");
	glUseProgram(program->GetProgramID());

	glLineWidth(2);

	for (std::vector<DebugShape>::iterator it = shapes.begin(); it != shapes.end(); ++it)
	{
		glLineWidth((*it).GetStroke());

		uint id_vertices_data = 0;
		uint id_indices = 0;

		float4x4 trans = (*it).GetTransform();
		float4 colour = (*it).GetColour();
		int mode = (*it).GetMode();

		App->renderer3D->SetUniformMatrix(program->GetProgramID(), "view", camera->GetViewMatrix());
		App->renderer3D->SetUniformMatrix(program->GetProgramID(), "projection", camera->GetProjectionMatrix());
		App->renderer3D->SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());

		App->renderer3D->SetUniformVector4(program->GetProgramID(), "debug_color", colour);

		uint vao = App->renderer3D->GenVertexArrayObject();

		glGenBuffers(1, &id_vertices_data);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices_data);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*it).GetNumVertices() * 3, (*it).GetVertices(), GL_DYNAMIC_DRAW);

		App->renderer3D->BindVertexArrayObject(vao);
		App->renderer3D->SetVertexAttributePointer(0, 3, 0, 0);
		App->renderer3D->EnableVertexAttributeArray(0);

		glGenBuffers(1, &id_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * (*it).GetNumIndices(), (*it).GetIndices(), GL_DYNAMIC_DRAW);

		glDrawElements(mode, (*it).GetNumIndices(), GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &id_vertices_data);
		glDeleteBuffers(1, &id_indices);

		App->renderer3D->UnbindVertexArrayObject();
		App->renderer3D->DeleteVertexArrayObject(vao);
	}

	// -----------

	// DeActivate
	glLineWidth(1);
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindSampler(0, last_sampler);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void DebugDraw::Clear()
{
	for (std::vector<DebugShape>::iterator it = shapes.begin(); it != shapes.end(); ++it)
	{
		(*it).CleanUp();
	}

	shapes.clear();
}

void DebugDraw::AddShape(DebugShape shape)
{
	shapes.push_back(shape);
}
