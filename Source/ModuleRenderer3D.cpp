#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "OpenGL.h"
#include "ModuleEditor.h"
#include "Primitive.h"
#include "Data.h"
#include "ComponentMeshRenderer.h"
#include "PerformanceWindow.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "RenderTexture.h"
#include "ModuleCamera3D.h"
#include "ComponentCanvas.h"
#include "Mesh.h"
#include "Material.h"
#include "ModuleScene.h"
#include "RenderTextureMSAA.h"
#include "CubeMap.h"
#include "SceneWindow.h"
#include "ModuleResources.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "Glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	use_vsync = true;
	is_using_depth_test = false;
	is_using_cull_test = false;
	is_using_fog = false;
	testing_light = false;
	name = "Renderer";
	editor_camera = nullptr;
	game_camera = nullptr;
	use_skybox = true;
	lights_count = 0;
	debug_draw = new DebugDraw();
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(Data* editor_config)
{
	CONSOLE_DEBUG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		CONSOLE_DEBUG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	glewInit();
	
	if(ret == true)
	{
		//Get Config Data
		int MSAA_level = -1;
		if (editor_config->EnterSection("Renderer_Config"))
		{
			use_vsync = editor_config->GetBool("Vsync");
			MSAA_level = editor_config->GetInt("MSAA_Level");
			editor_config->LeaveSection();
		}
		
		if (MSAA_level < 0) MSAA_level = 2;

		//Use Vsync
		if(use_vsync && SDL_GL_SetSwapInterval(1) < 0)
			CONSOLE_DEBUG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		App->camera->CreateEditorCamera();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glClearDepth(1.0f);
		
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize clear color
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		is_using_depth_test = true;
		is_using_cull_test = true;

		glEnable(GL_MULTISAMPLE);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
		}
	}
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	ms_timer.Start();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (editor_camera != nullptr && editor_camera->GetViewportTexture() != nullptr)
	{
		editor_camera->GetViewportTexture()->Bind();
		DrawEditorScene();
	}

	for (std::list<ComponentCamera*>::iterator it = rendering_cameras.begin(); it != rendering_cameras.end(); it++)
	{
		DrawSceneCameras(*it);
	}
	
	ResetRender();

	//Assert polygon mode is fill before render gui
	GLint polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	App->editor->DrawEditor();

	//reset polygonmode to previous one
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	App->editor->performance_window->AddModuleData(this->name, ms_timer.ReadMs());

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::DrawEditorScene()
{
	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		//App->scene->DrawSkyBox(editor_camera->camera_frustum.pos);
		glEnable(GL_DEPTH_TEST);
	}

	pPlane pl(0, 1, 0, 0);
	pl.SetPos(editor_camera->camera_frustum.pos);
	pl.color = { 1,1,1,1 };
	//pl.Render();
	DrawSceneGameObjects(editor_camera, true);
}

void ModuleRenderer3D::DrawSceneCameras(ComponentCamera * camera)
{
	if (camera == nullptr || camera->GetViewportTexture() == nullptr) return;

	camera->GetViewportTexture()->Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		//App->scene->DrawSkyBox(camera->camera_frustum.pos);
		glEnable(GL_DEPTH_TEST);
	}

	DrawSceneGameObjects(camera, false);
}

void ModuleRenderer3D::DrawDebugCube(ComponentMeshRenderer * mesh, ComponentCamera * active_camera)
{
	if (mesh->GetMesh() != nullptr)
	{
		AABB aabb = mesh->GetMesh()->box;
		float3 size = aabb.Size();
		float3 pos = aabb.CenterPoint();
		Quat rot = Quat::identity;
		float4x4 trans = float4x4::FromTRS(pos, rot, size);

		ShaderProgram* program = App->resources->GetShaderProgram("default_shader_program");

		UseShaderProgram(program->GetProgramID());

		SetUniformMatrix(program->GetProgramID(), "view", active_camera->GetViewMatrix());
		SetUniformMatrix(program->GetProgramID(), "projection", active_camera->GetProjectionMatrix());
		SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());

		SetUniformBool(program->GetProgramID(), "has_texture", false);
		SetUniformBool(program->GetProgramID(), "has_material_color", true);
		SetUniformVector4(program->GetProgramID(), "material_color", float4(1.0f, 0.5f, 0.0f, 1.0f));

		Mesh* cube = App->resources->GetMesh("PrimitiveCube");
		if (cube->id_indices == 0) cube->LoadToMemory();

		//set wireframe before render 
		GLint polygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
		SetWireframeMode();

		BindVertexArrayObject(cube->id_vao);
		glDrawElements(GL_TRIANGLES, cube->num_indices, GL_UNSIGNED_INT, NULL);
		UnbindVertexArrayObject();

		//restore previous polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	}
}

void ModuleRenderer3D::DrawCanvas(ComponentCamera* camera, bool editor_camera)
{
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (std::list<ComponentCanvas*>::iterator cv = canvas_to_draw.begin(); cv != canvas_to_draw.end(); ++cv)
	{
		std::vector<CanvasDrawElement> to_draw = (*cv)->GetDrawElements();

		ShaderProgram* program = App->resources->GetShaderProgram("default_shader_program");

		UseShaderProgram(program->GetProgramID());

		for (std::vector<CanvasDrawElement>::iterator it = to_draw.begin(); it != to_draw.end(); ++it)
		{
			// WORLD
			if (editor_camera || (*cv)->GetRenderMode() == CanvasRenderMode::RENDERMODE_WORLD_SPACE)
			{
				SetUniformMatrix(program->GetProgramID(), "view", camera->GetViewMatrix());
				SetUniformMatrix(program->GetProgramID(), "projection", camera->GetProjectionMatrix());
				SetUniformMatrix(program->GetProgramID(), "Model", (*it).GetTransform().Transposed().ptr());
			}

			// ORTHO
			else
			{
				float ortho_projection[4][4] =
				{
				{ 2.0f / App->window->GetWidth(), 0.0f,                             0.0f, 0.0f },
				{ 0.0f,                           2.0f / -App->window->GetHeight(), 0.0f, 0.0f },
				{ 0.0f,                           0.0f,                            -1.0f, 0.0f },
				{ -1.0f,                          1.0f,                             0.0f, 1.0f },
				};

				SetUniformMatrix(program->GetProgramID(), "view", camera->GetViewMatrix());
				SetUniformMatrix(program->GetProgramID(), "projection", &ortho_projection[0][0]);
				SetUniformMatrix(program->GetProgramID(), "Model", (*it).GetOrtoTransform().Transposed().ptr());
			}

			SetUniformBool(program->GetProgramID(), "has_texture", (*it).GetTextureId() > 0);
			SetUniformBool(program->GetProgramID(), "has_material_color", true);
			SetUniformVector4(program->GetProgramID(), "material_color", (*it).GetColour());

			if ((*it).GetPlane()->id_indices == 0) (*it).GetPlane()->LoadToMemory();

			BindVertexArrayObject((*it).GetPlane()->id_vao);

			glBindTexture(GL_TEXTURE_2D, (*it).GetTextureId());

			glDrawElements(GL_TRIANGLES, (*it).GetPlane()->num_indices, GL_UNSIGNED_INT, NULL);

			glBindTexture(GL_TEXTURE_2D, 0);
			UnbindVertexArrayObject();
		}
	}

	// DeActivate
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

void ModuleRenderer3D::DrawSceneGameObjects(ComponentCamera* active_camera, bool is_editor_camera)
{
	std::vector<std::string> layer_masks = active_camera->GetAllLayersToDraw();

	std::list<ComponentMeshRenderer*> static_instersects;
	App->scene->GetOctreeIntersects(static_instersects, active_camera->camera_frustum.MinimalEnclosingAABB());

	for (std::list<ComponentMeshRenderer*>::iterator it = static_instersects.begin(); it != static_instersects.end(); it++)
	{
		if (!is_editor_camera)
		{
			if (std::find(layer_masks.begin(), layer_masks.end(), (*it)->GetGameObject()->GetLayer()) == layer_masks.end()) continue;
		}
		else
		{
			if ((*it)->GetGameObject()->IsSelected())
			{
				DrawDebugCube(*it, active_camera);
			}
		}
		DrawMesh(*it, active_camera);
	}

	for (std::list<ComponentMeshRenderer*>::iterator it = dynamic_mesh_to_draw.begin(); it != dynamic_mesh_to_draw.end(); it++)
	{
		if (!is_editor_camera)
		{
			if (active_camera->GetGameObject() && (*it)->GetMesh())
			{
				if (active_camera->ContainsGameObjectAABB((*it)->GetMesh()->box))
				{
					if (std::find(layer_masks.begin(), layer_masks.end(), (*it)->GetGameObject()->GetLayer()) == layer_masks.end()) continue;
					DrawMesh(*it, active_camera);
				}
			}
		}
		else
		{
			DrawMesh(*it, active_camera);
			if ((*it)->GetGameObject()->IsSelected())
			{
				DrawDebugCube(*it, active_camera);
			}
		}
	}

	if (is_editor_camera)
	{
		for (std::list<ComponentCamera*>::iterator it = rendering_cameras.begin(); it != rendering_cameras.end(); it++)
		{
			if ((*it)->GetGameObject()->IsSelected())
			{
				DebugFrustum frustum((*it)->camera_frustum);
				frustum.color = { 0,1,0,1 };
				//frustum.Render();
			}
		}
	}

	if (App->scene->draw_octree)
	{
		//App->scene->octree.DebugDraw();
	}
	
	DrawCanvas(active_camera, true);

	// Debug Draw render
	debug_draw->Render(editor_camera);
	debug_draw->Clear();

	active_camera->GetViewportTexture()->Render();
	active_camera->GetViewportTexture()->Unbind();
}

void ModuleRenderer3D::DrawMesh(ComponentMeshRenderer * mesh, ComponentCamera* active_camera)
{
	if (mesh == nullptr || mesh->GetMesh() == nullptr) return;
	if (mesh->GetMesh()->id_indices == 0) mesh->GetMesh()->LoadToMemory();
	
	Material* material = mesh->GetMaterial();

	uint program = 0;
	if (material != nullptr)
	{
		program = material->GetShaderProgramID();
		UseShaderProgram(program);
		material->LoadToMemory();
	}

	SetUniformMatrix(program, "view", active_camera->GetViewMatrix());
	SetUniformMatrix(program, "projection", active_camera->GetProjectionMatrix());
	SetUniformMatrix(program, "Model", mesh->GetGameObject()->GetGlobalTransfomMatrix().Transposed().ptr());

	BindVertexArrayObject(mesh->GetMesh()->id_vao);
	glDrawElements(GL_TRIANGLES, mesh->GetMesh()->num_indices, GL_UNSIGNED_INT, NULL);
	UnbindVertexArrayObject();
}

void ModuleRenderer3D::AddMeshToDraw(ComponentMeshRenderer * mesh)
{
	dynamic_mesh_to_draw.push_back(mesh);
}

void ModuleRenderer3D::AddCanvasToDraw(ComponentCanvas * canvas)
{
	canvas_to_draw.push_back(canvas);
}

void ModuleRenderer3D::ResetRender()
{
	dynamic_mesh_to_draw.clear();
	debug_primitive_to_draw.clear();
	rendering_cameras.clear();
	debug_draw->Clear();

	for (std::list<ComponentCanvas*>::iterator cv = canvas_to_draw.begin(); cv != canvas_to_draw.end(); ++cv)
		(*cv)->ClearDrawElements();
	
	canvas_to_draw.clear();
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_DEBUG("Destroying 3D Renderer");
	SDL_GL_DeleteContext(context);
	rendering_cameras.clear();

	debug_draw->Clear();
	RELEASE(debug_draw);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height, ComponentCamera* camera)
{

	int msaa_level = camera->GetViewportTexture()->GetCurrentMSAALevel();
	camera->GetViewportTexture()->Destroy();
	camera->GetViewportTexture()->Create(width, height, msaa_level);

	float ratio = (float)width / (float)height;
	camera->SetAspectRatio(ratio);
	glViewport(0, 0, width, height);
}

void ModuleRenderer3D::SetWireframeMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ModuleRenderer3D::SaveData(Data * data)
{
	data->CreateSection("Renderer_Config");
	data->AddBool("Vsync", use_vsync);
	if(editor_camera->GetViewportTexture() == nullptr)
		data->AddInt("MSAA_Level", 2);
	else data->AddInt("MSAA_Level", editor_camera->GetViewportTexture()->GetCurrentMSAALevel());
	

	data->CloseSection();
}

void ModuleRenderer3D::SetActiveDepthTest(bool active)
{
	is_using_depth_test = active;

	if (active)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void ModuleRenderer3D::SetActiveCullTest(bool active)
{
	is_using_cull_test = active;

	if (active)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void ModuleRenderer3D::SetActiveFog(bool active)
{
	is_using_fog = active;

	if (active)
	{
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}
}

bool ModuleRenderer3D::GetActiveDepthTest() const
{
	return is_using_depth_test;
}

bool ModuleRenderer3D::GetActiveCullTest() const
{
	return is_using_cull_test;
}

bool ModuleRenderer3D::GetActiveFog() const
{
	return is_using_fog;
}

void ModuleRenderer3D::EnableTestLight()
{
	lights[7].ref = GL_LIGHT7;
	lights[7].Active(true);
	for (uint i = 1; i < lights_count - 1; ++i)
		lights[i].Active(false);
	testing_light = true;
}

void ModuleRenderer3D::DisableTestLight()
{
	lights[7].Active(false);
	for (uint i = 1; i < lights_count - 1; ++i)
		lights[i].Active(true);
	lights[7].ref = 0;
	testing_light = false;
}

void ModuleRenderer3D::ActiveSkybox(bool active)
{
	use_skybox = active;
}

bool ModuleRenderer3D::IsUsingSkybox() const
{
	return use_skybox;
}

void ModuleRenderer3D::BindArrayBuffer(uint id) const
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::BindElementArrayBuffer(uint id) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindArraybuffer() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error unbind array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindElementArrayBuffer() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR ("Error unbind buffer: %s\n", gluErrorString(error));
	}
}

DebugDraw * ModuleRenderer3D::GetDebugDraw()
{
	return debug_draw;
}

// ------------- Shaders -------------------------

uint ModuleRenderer3D::GenVertexArrayObject() const
{
	uint ret = 0;
	glGenVertexArrays(1, (GLuint*)&ret);
	return ret;
}

void ModuleRenderer3D::BindVertexArrayObject(uint id) const
{
	glBindVertexArray(id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error bind vertex array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindVertexArrayObject() const
{
	glBindVertexArray(0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error unbind array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::DeleteVertexArrayObject(uint vao)
{
	glDeleteVertexArrays(1, &vao);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error delete array buffer: %s\n", gluErrorString(error));
	}
}

uint ModuleRenderer3D::CreateVertexShader(const char * source)
{
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &source, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		CONSOLE_ERROR("Shader compilation error:\n %s", infoLog);
		glDeleteShader(vertexShader);
		return 0;
	}
	return vertexShader;
}

uint ModuleRenderer3D::CreateFragmentShader(const char * source)
{
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &source, NULL);
	glCompileShader(fragmentShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		CONSOLE_ERROR("Shader compilation error:\n %s", infoLog);
		glDeleteShader(fragmentShader);
		return 0;
	}
	return fragmentShader;
}

void ModuleRenderer3D::DeleteShader(uint shader_id)
{
	if (shader_id != 0)
	{
		glDeleteShader(shader_id);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_ERROR("Error deleting shader %s\n", gluErrorString(error));
		}
	}
}

uint ModuleRenderer3D::GetProgramBinary(uint program_id, uint buff_size, char * buff) const
{
	uint ret = 0;

	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);

	glGetProgramBinary(program_id, buff_size, (GLsizei*)&ret, (GLenum*)binaryFormats, buff);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error getting shader program %d binary! %s\n", program_id, gluErrorString(error));
	}

	RELEASE_ARRAY(binaryFormats);

	return ret;
}

int ModuleRenderer3D::GetProgramSize(uint program_id) const
{
	int ret = 0;

	glGetProgramiv(program_id, GL_PROGRAM_BINARY_LENGTH, &ret);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error getting shader program %d size! %s\n", program_id, gluErrorString(error));

	}

	return ret;
}

void ModuleRenderer3D::LoadProgramFromBinary(uint program_id, uint buff_size, const char * buff)
{
	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);

	glProgramBinary(program_id, (GLenum)binaryFormats, buff, buff_size);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error loading shader program binary %s\n", gluErrorString(error));

	}
}

void ModuleRenderer3D::EnableVertexAttributeArray(uint id)
{
	glEnableVertexAttribArray(id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error enabling vertex attribute Pointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::DisableVertexAttributeArray(uint id)
{
	glDisableVertexAttribArray(id);

	//Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error disabling vertex attributePointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetVertexAttributePointer(uint id, uint element_size, uint elements_gap, uint infogap)
{
	glVertexAttribPointer(id, element_size, GL_FLOAT, GL_FALSE, elements_gap * sizeof(GLfloat), (void*)(infogap * sizeof(GLfloat)));
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting vertex attributePointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UseShaderProgram(uint id)
{
	glUseProgram(id);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error at use shader program: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformMatrix(uint program, const char * name, float * data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, data);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform matrix %s: %s\n", name, gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformBool(uint program, const char * name, bool data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform1i(modelLoc, data);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform float %s: %s\n", name, gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformFloat(uint program, const char * name, float data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform1f(modelLoc, data);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform float %s: %s\n", name, gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformVector4(uint program, const char * name, float4 data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform4f(modelLoc, data.x, data.y, data.z, data.w);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform float %s: %s\n", name, gluErrorString(error));
	}
}

uint ModuleRenderer3D::CreateShaderProgram()
{
	uint ret = glCreateProgram();
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error creating shader program %s\n", gluErrorString(error));
	}
	return ret;
}

void ModuleRenderer3D::AttachShaderToProgram(uint program_id, uint shader_id)
{
	if (program_id != 0 && shader_id != 0)
	{
		glAttachShader(program_id, shader_id);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_ERROR("Error attaching shader %s\n", gluErrorString(error));
		}
	}
}

bool ModuleRenderer3D::LinkProgram(uint program_id)
{
	bool ret = true;

	if (program_id != 0)
	{
		glLinkProgram(program_id);

		GLint success;
		GLint valid;
		glGetProgramiv(program_id, GL_LINK_STATUS, &success);
		glGetProgramiv(program_id, GL_VALIDATE_STATUS, &valid);
		if (!success || !valid) {
			GLchar infoLog[512];
			glGetProgramInfoLog(program_id, 512, NULL, infoLog);
			CONSOLE_ERROR("Shader link error: %s", infoLog);
			ret = false;
		}
	}
	else ret = false;

	return ret;
}

void ModuleRenderer3D::DeleteProgram(uint program_id)
{
	if (program_id != 0)
	{
		glDeleteProgram(program_id);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_ERROR("Error deleting shader program %s\n", gluErrorString(error));
		}
	}
}
// ------------------------------------------------
