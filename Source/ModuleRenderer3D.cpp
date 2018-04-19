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
#include "ComponentLight.h"
#include "RenderTexture.h"
#include "ModuleCamera3D.h"
#include "ComponentCanvas.h"
#include "Mesh.h"
#include "Material.h"
#include "ModuleScene.h"
#include "RenderTextureMSAA.h"
#include "CubeMap.h"
#include "SceneWindow.h"
#include "ModulePhysics.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentBillboard.h"
#include "ShaderProgram.h"
#include "DebugDraw.h"
#include "GameWindow.h"
#include "ComponentParticleEmmiter.h"
#include "ModuleMeshImporter.h"
#include "ComponentRectTransform.h"
#include "ModuleInput.h"
#include "ComponentRigidBody.h"
#include "ModuleResources.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "glmath.h"
#include "OpenGL.h"
#include "Brofiler\Brofiler.h"
#include <algorithm> 

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

	for (uint i = 0; i < MAX_DIR_LIGHT; ++i)
		dir_lights[i] = nullptr;

	for (uint i = 0; i < MAX_SPO_LIGHT; ++i)
		spo_lights[i] = nullptr;

	for (uint i = 0; i < MAX_POI_LIGHT; ++i)
		poi_lights[i] = nullptr;

	directional_light_count = 0;
	point_light_count = 0;
	spot_light_count = 0;
	current_shader_program = 0;
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
		if(SDL_GL_SetSwapInterval(use_vsync) < 0)
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

	//Set Up Depth Map

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ResetRender();
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer  PostUpdate", Profiler::Color::Red);
	ms_timer.Start();

	// Regular rendering
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
	BROFILER_CATEGORY("Renderer Draw Editor Scene", Profiler::Color::OrangeRed);

	//Skybox shader
	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		App->scene->DrawSkyBox(editor_camera->camera_frustum.Pos(), editor_camera);
		glEnable(GL_DEPTH_TEST);
	}


	//Grid shader
	DrawGrid(editor_camera);

	// Default shader
	// Debug Draw render
	debug_draw->Render(editor_camera);

	DrawSceneGameObjects(editor_camera, true);
}

void ModuleRenderer3D::DrawSceneCameras(ComponentCamera * camera)
{
	BROFILER_CATEGORY("Renderer Draw Scene Cameras", Profiler::Color::PaleVioletRed);
	if (camera == nullptr || camera->GetViewportTexture() == nullptr); 
	if (camera != game_camera)
	{
		if (!camera->IsActive() || !camera->GetGameObject()->IsActive()) return;
		if (!App->IsGame() && !camera->GetGameObject()->IsSelected()) return;
	}

	camera->GetViewportTexture()->Bind();
	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		App->scene->DrawSkyBox(camera->camera_frustum.Pos(), camera);
		glEnable(GL_DEPTH_TEST);
	}
	
	DrawSceneGameObjects(camera, false);
}

void ModuleRenderer3D::DrawDebugCube(AABB& aabb, ComponentCamera * active_camera)
{
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
		SetUniformVector4(program->GetProgramID(), "material_color", float4(1.0f, 0.0f, 0.0f, 1.0f));

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

void ModuleRenderer3D::DrawCanvas(ComponentCamera* camera, bool editor_camera)
{
	if (canvas_to_draw.empty()) return;
	BROFILER_CATEGORY("Draw Canvas", Profiler::Color::FloralWhite);
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
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (std::vector<ComponentCanvas*>::iterator cv = canvas_to_draw.begin(); cv != canvas_to_draw.end(); ++cv)
	{
		ComponentCanvas* canvas = (*cv);

		ComponentRectTransform* last_rect_trans = canvas->GetLastRectTransform();
		ComponentRectTransform* last_last_rect_trans = canvas->GetLastLastRectTransform();

		std::vector<CanvasDrawElement> to_draw = canvas->GetDrawElements();

		uint program = App->resources->GetShaderProgram("default_shader_program")->GetProgramID();

		UseShaderProgram(program);

		LineSegment segment = App->camera->GetUIMouseRay(*cv);

		CanvasDrawElement* top_element = nullptr;
		int highest_layer = -9999999999;

		for (std::vector<CanvasDrawElement>::iterator it = to_draw.begin(); it != to_draw.end(); ++it)
		{
			ComponentRectTransform* rect_trans = (ComponentRectTransform*)(*it).GetComponent()->GetGameObject()->GetComponent(Component::CompRectTransform);

			if (rect_trans != nullptr)
			{
				if (!editor_camera)
				{					
					if (rect_trans->GetInteractable() && rect_trans->GetGameObject()->IsActive() && (*it).CheckRay(segment, canvas->GetRenderMode()))
					{
						top_element = &(*it);
					}
				}
				// -----------
			}
			// WORLD
			if (editor_camera || (*cv)->GetRenderMode() == CanvasRenderMode::RENDERMODE_WORLD_SPACE)
			{
				if ((*cv)->GetRenderMode() == CanvasRenderMode::RENDERMODE_WORLD_SPACE)
				{
					SetUniformBool(program, "is_ui", false);
					SetUniformBool(program, "has_opacity", false);
					SetUniformBool(program, "has_light", true);
					//SetUniformBool(program, "own_uvs_normalmap", false); don't need to send this
					SetUniformBool(program, "has_normalmap", false);
				}
				else
					SetUniformBool(program, "is_ui", true);

				SetUniformMatrix(program, "view", camera->GetViewMatrix());
				SetUniformMatrix(program, "projection", camera->GetProjectionMatrix());
				SetUniformMatrix(program, "Model", (*it).GetTransform().Transposed().ptr());
			}

			// ORTHO
			else
			{
				SetUniformBool(program, "is_ui", true);

				float2 win_size = App->editor->game_window->GetSize();

				float far_p = -1000;
				float near_p = 1000;

				float ortho_projection[4][4] =
				{
				{ 2.0f / win_size.x,       0.0f,               0.0f,			   0.0f },
				{ 0.0f,                    2.0f / win_size.y,  0.0f,			   0.0f },
				{ 0.0f,                    0.0f,              -2/(far_p - near_p), -((far_p + near_p)/(far_p - near_p)) },
				{ -1.0f,                   1.0f,               0.0f,			   1.0f },
				};

				float4x4 ide = float4x4::identity;
				SetUniformMatrix(program, "view", (float*)ide.Transposed().v);
				SetUniformMatrix(program, "projection", &ortho_projection[0][0]);
				SetUniformMatrix(program, "Model", (*it).GetOrtoTransform().Transposed().ptr());
			}

			bool has_texture = ((*it).GetTextureId() > 0);

			SetUniformBool(program, "has_material_color", !has_texture);
			SetUniformVector4(program, "material_color", (*it).GetColour());
			SetUniformBool(program, "has_texture", has_texture);
			SetUniformBool(program, "has_texture2", false);
			SetUniformBool(program, "own_uvs_diffuse", false);
			
			//SetUniformBool(program, "own_uvs_diffuse2", false); don't need to send this

			SetUniformInt(program, "Tex_Diffuse", 0);
			SetUniformInt(program, "Tex_Diffuse2", 3);
			SetUniformInt(program, "Tex_NormalMap", 1);
			SetUniformInt(program, "Tex_Opacity", 2);


			if ((*it).GetPlane()->id_indices == 0) (*it).GetPlane()->LoadToMemory();

			BindVertexArrayObject((*it).GetPlane()->id_vao);

			//SendLight(program);

			glBindTexture(GL_TEXTURE_2D, (*it).GetTextureId());

			glDrawElements(GL_TRIANGLES, (*it).GetPlane()->num_indices, GL_UNSIGNED_INT, NULL);

			glBindTexture(GL_TEXTURE_2D, 0);
			UnbindVertexArrayObject();
		}

		// Event managing

		if (!editor_camera)
		{
			ComponentRectTransform* rect_trans = nullptr;

			if (top_element != nullptr)
				rect_trans = (ComponentRectTransform*)top_element->GetComponent()->GetGameObject()->GetComponent(Component::CompRectTransform);

			if (last_last_rect_trans != nullptr)
			{
				if (last_last_rect_trans->GetOnMouseOut())
					last_last_rect_trans->SetOnMouseOut(false);
			}

			if (last_rect_trans != nullptr)
			{
				if (last_rect_trans->GetOnClickDown())
					last_rect_trans->SetOnClickDown(false);

				if (last_rect_trans->GetOnClickUp())
					last_rect_trans->SetOnClickUp(false);

				if (last_rect_trans->GetOnMouseEnter())
					last_rect_trans->SetOnMouseEnter(false);

				if (last_rect_trans != rect_trans)
				{
					last_rect_trans->SetOnMouseOut(true);
					last_rect_trans->SetOnMouseOver(false);

					last_rect_trans->SetOnClickDown(false);
					last_rect_trans->SetOnClickUp(false);
					last_rect_trans->SetOnClick(false);
				}
			}

			if (top_element != nullptr)
			{
				if (!rect_trans->GetOnMouseOver())
					rect_trans->SetOnMouseEnter(true);

				rect_trans->SetOnMouseOver(true);

				if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
				{
					rect_trans->SetOnClickUp(false);
					rect_trans->SetOnClickDown(true);
					rect_trans->SetOnClick(true);
				}

				if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
				{
					rect_trans->SetOnClickUp(true);
					rect_trans->SetOnClickDown(false);
					rect_trans->SetOnClick(false);
				}

			}

			canvas->SetLastLastRectTransform(last_rect_trans);
			canvas->SetLastRectTransform(rect_trans);
		}
	}

	// DeActivate
	UseShaderProgram(last_program);
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

void ModuleRenderer3D::OrderByMaterials(ComponentMeshRenderer* mesh)
{
	bool mesh_included = false;
	bool material_match = false;

	// This one is for materials with nullptr.
	if (ordering_by_materials.size() == 0)
	{
		std::vector<ComponentMeshRenderer*> first_mat;
		ordering_by_materials.push_back(first_mat);
	}

	while (material_match == false)
	{
		// If it's nullptr, it's added to the first chunk of the list.
		if (mesh->GetMaterial() == nullptr)
		{
			ordering_by_materials[0].push_back(mesh);
			material_match = false;
			continue;
		}
		// If it's not, we need to find it.
		for (uint ord_i = 0; ord_i < ordering_by_materials.size(); ++ord_i)
		{
			// If it's a match we check it's distance and place it properly
			if (material_match == false && ordering_by_materials[ord_i].size() > 0 && ordering_by_materials[ord_i][0] != nullptr && mesh->GetMaterial() == ordering_by_materials[ord_i][0]->GetMaterial())
			{
				material_match = true;
				ordering_by_materials[ord_i].push_back(mesh);

				break;
			}
		}
		// If we are at the end of the vector and we didn't get a match we create a slot and proceed to add it.
		if (material_match == false)
		{
			std::vector<ComponentMeshRenderer*> mat;
			ordering_by_materials.push_back(mat);
			ordering_by_materials[ordering_by_materials.size() - 1].push_back(mesh);
			material_match = true;
		}
	}
}

void ModuleRenderer3D::DrawDebugOBB(OBB& obb, ComponentCamera * active_camera)
{
	float3 size = obb.Size();
	float3 pos = obb.CenterPoint();
	float3x3 rot_mat = obb.LocalToWorld().RotatePart(); 
	Quat rot = rot_mat.ToQuat(); 

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


void ModuleRenderer3D::DrawZBuffer()
{
	uint program = 0;
	ShaderProgram* shader = App->resources->GetShaderProgram("depthdebug_shader_program");
	program = shader->GetProgramID();
	UseShaderProgram(program);

	SetUniformFloat(program, "near_plane", near_plane);
	SetUniformFloat(program, "far_plane", far_plane);

	SetUniformInt(program, "depthMap", 0);

	float4x4 trans = float4x4::FromTRS(float3(0, 0, 0), Quat::identity, float3(1, 1, 1));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_map);

	SetUniformMatrix(program, "view", App->camera->GetCamera()->GetViewMatrix());
	SetUniformMatrix(program, "projection", App->camera->GetCamera()->GetProjectionMatrix());
	SetUniformMatrix(program, "Model", trans.Transposed().ptr());
}

void ModuleRenderer3D::DrawGrid(ComponentCamera * camera)
{
	BROFILER_CATEGORY("Draw Grid", Profiler::Color::DarkBlue);
	float4x4 trans = float4x4::FromTRS(float3(0, 0, 0), Quat::identity, float3(10, 1, 10));

	ShaderProgram* program = App->resources->GetShaderProgram("grid_shader_program");
	UseShaderProgram(program->GetProgramID());

	SetUniformMatrix(program->GetProgramID(), "view", camera->GetViewMatrix());
	SetUniformMatrix(program->GetProgramID(), "projection", camera->GetProjectionMatrix());
	SetUniformMatrix(program->GetProgramID(), "Model", trans.Transposed().ptr());

	SetUniformVector4(program->GetProgramID(), "line_color", float4(1.0f, 1.0f, 1.0f, 1.0f));

	Mesh* plane = App->resources->GetMesh("PrimitivePlane");
	if (plane->id_indices == 0)plane->LoadToMemory();

	BindVertexArrayObject(plane->id_vao);
	glDrawElements(GL_TRIANGLES, plane->num_indices, GL_UNSIGNED_INT, NULL);
	UnbindVertexArrayObject();
}


float4x4 ModuleRenderer3D::OrthoProjection( float left, float right, float bottom, float top, float near_plane, float far_plane)
{
	float a = 2.0f / (right - left);
	float b = 2.0f / (top - bottom);
	float c = -2.0f / (far_plane - near_plane);

	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far_plane + near_plane) / (far_plane - near_plane);

	float4x4 n_projection(
		a, 0.0f, 0.0f, tx,
		0.0f, b, 0.0f, ty,
		0.0f, 0.0f, c, tz,
		0.0f, 0.0f, 0.0f, 1.0f);

	return n_projection;
}

void ModuleRenderer3D::DrawSceneGameObjects(ComponentCamera* active_camera, bool is_editor_camera)
{
	BROFILER_CATEGORY("Draw Scene GameObjects", Profiler::Color::CornflowerBlue);
	std::vector<std::string> layer_masks = active_camera->GetAllLayersToDraw();

	std::vector<ComponentMeshRenderer*> static_instersects;
	App->scene->GetOctreeIntersects(static_instersects, active_camera->camera_frustum.MinimalEnclosingAABB());

	for (std::vector<ComponentMeshRenderer*>::iterator it = static_instersects.begin(); it != static_instersects.end(); it++)
	{
		if (!is_editor_camera)
		{
			//if (std::find(layer_masks.begin(), layer_masks.end(), (*it)->GetGameObject()->GetLayer()) == layer_masks.end()) continue;
		}
		else
		{
			if ((*it)->GetGameObject()->IsSelected())
			{
				if ((*it)->GetMesh() != nullptr)
					cubes_to_draw.push_back((*it)->bounding_box);
			}
		}
		OrderByMaterials(*it);
		//DrawMesh(*it, active_camera);
	}

	for (std::vector<ComponentMeshRenderer*>::iterator it2 = dynamic_mesh_to_draw.begin(); it2 != dynamic_mesh_to_draw.end(); ++it2)
	{
		if (!is_editor_camera)
		{
			if (active_camera->GetGameObject() && (*it2)->GetMesh())
			{
				if (active_camera->ContainsGameObjectAABB((*it2)->bounding_box))
				{
					//if (std::find(layer_masks.begin(), layer_masks.end(), (*it)->GetGameObject()->GetLayer()) == layer_masks.end()) continue;
					OrderByMaterials(*it2);
					//DrawMesh(*it2, active_camera);
				}
			}
		}
		else
		{
			OrderByMaterials(*it2);
			//DrawMesh(*it2, active_camera);
			if ((*it2)->GetGameObject()->IsSelected())
			{
				if ((*it2)->GetMesh() != nullptr)
					cubes_to_draw.push_back((*it2)->bounding_box);
			}
		}
	}

	//Draw Meshes
	uint program = App->resources->GetShaderProgram("default_shader_program")->GetProgramID();
	UseShaderProgram(program);
	SendLight(program);
	SetUniformMatrix(program, "view", active_camera->GetViewMatrix());
	SetUniformMatrix(program, "projection", active_camera->GetProjectionMatrix());
	SetUniformBool(program, "is_ui", false);

	glEnable(GL_BLEND); // this enables opacity map so yeah, don't comment it again or ya'll will hear me >:(
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (uint i = 0; i < ordering_by_materials.size(); ++i)
	{
		if (ordering_by_materials[i].empty()) continue;

		if (ordering_by_materials[i][0]->GetMaterial() != nullptr)
			current_material = ordering_by_materials[i][0]->GetMaterial();

		DrawMesh(ordering_by_materials[i], active_camera);
		//changed_material = false;
		//changed_material = true;
	}

	for (uint i = 0; i < cubes_to_draw.size(); ++i)
	{
		DrawDebugCube(cubes_to_draw[i], active_camera);
	}


	//Draw Particles
	for (std::vector<ComponentParticleEmmiter*>::iterator it = particles_to_draw.begin(); it != particles_to_draw.end(); it++)
	{
		if (*it == nullptr)
			continue;

		(*it)->AddaptEmmitAreaAABB();

		if ((*it)->ShowEmmisionArea())
		{
			DrawDebugCube((*it)->emmit_area, active_camera);
		}

		(*it)->DrawParticles(active_camera); 

	}

	//if (is_editor_camera)
	//{
	//	for (std::list<ComponentCamera*>::iterator it = rendering_cameras.begin(); it != rendering_cameras.end(); it++)
	//	{
	//		if ((*it)->GetGameObject()->IsSelected())
	//		{
	//			DebugFrustum frustum((*it)->camera_frustum);
	//			frustum.color = { 0,1,0,1 };
	//			//frustum.Render();
	//		}
	//	}
	//}
	if (App->scene->draw_octree)
	{
		//App->scene->octree.DebugDraw();
	}
	
	if(active_camera == game_camera)
		DrawCanvas(active_camera, false);
	else if(active_camera == editor_camera)
		DrawCanvas(active_camera, true);

	// Debug Draw render
	/*if(is_editor_camera)
		debug_draw->Render(editor_camera);*/

	active_camera->GetViewportTexture()->Render();
	active_camera->GetViewportTexture()->Unbind();
}

void ModuleRenderer3D::DrawMesh(std::vector<ComponentMeshRenderer*> meshes, ComponentCamera* active_camera)
{
	if (current_material != nullptr)
	{
		current_material->LoadToMemory();
		uint program = 0;
		program = current_material->GetShaderProgramID();
		UseShaderProgram(program);

		bool has_light = true;
		SetUniformBool(program, "has_light", true);

		uint current_vao = meshes[0]->GetMesh()->id_vao;

		BindVertexArrayObject(meshes[0]->GetMesh()->id_vao);

		for (ComponentMeshRenderer* mesh : meshes)
		{
			if (mesh == nullptr || mesh->GetMesh() == nullptr) return;
			if (mesh->GetMesh()->id_indices == 0) mesh->GetMesh()->LoadToMemory();

			ComponentBillboard* billboard = (ComponentBillboard*)mesh->GetGameObject()->GetComponent(Component::CompBillboard);
			if (billboard != nullptr)
			{
				billboard->RotateObject();
			}

			if (has_light != mesh->has_light)
			{
				SetUniformBool(program, "has_light", mesh->has_light);
				has_light = mesh->has_light;
			}
			
			SetUniformMatrix(program, "Model", mesh->GetGameObject()->GetGlobalTransfomMatrix().Transposed().ptr());


			//TODO: Calculate Normal Matrix here and send it as uniform.
			// We need this because transitioning to World Space gives a non accurate Normal.
			// Reminder: NormalMat = mat3(transpose(inverse(Model))) * normals;

			if (mesh->GetMesh()->id_vao != current_vao)
			{
				UnbindVertexArrayObject();
				BindVertexArrayObject(mesh->GetMesh()->id_vao);
				current_vao = mesh->GetMesh()->id_vao;
			}

			glDrawElements(GL_TRIANGLES, mesh->GetMesh()->num_indices, GL_UNSIGNED_INT, NULL);
		}

		UnbindVertexArrayObject();
	}
}

void ModuleRenderer3D::AddMeshToDraw(ComponentMeshRenderer * mesh)
{
	BROFILER_CATEGORY("Rendered Add Mesh To Draw", Profiler::Color::Wheat);
	dynamic_mesh_to_draw.push_back(mesh);
}

void ModuleRenderer3D::AddCanvasToDraw(ComponentCanvas * canvas)
{
	canvas_to_draw.push_back(canvas);
}

void ModuleRenderer3D::RemoveCanvasToDraw(ComponentCanvas * canvas)
{
	for (std::vector<ComponentCanvas*>::iterator cv = canvas_to_draw.begin(); cv != canvas_to_draw.end(); ++cv)
	{
		if ((*cv == canvas))
		{
			canvas_to_draw.erase(cv);
			break;
		}
	}
}

std::vector<ComponentCanvas*> ModuleRenderer3D::GetCanvasToDraw() const
{
	return canvas_to_draw;
}

void ModuleRenderer3D::ResetRender()
{
	BROFILER_CATEGORY("reset renderer", Profiler::Color::Beige);

	dynamic_mesh_to_draw.clear();
	debug_draw->Clear();

	for (std::vector<ComponentCanvas*>::iterator cv = canvas_to_draw.begin(); cv != canvas_to_draw.end(); ++cv)
		(*cv)->ClearDrawElements();
	
	canvas_to_draw.clear();
	debug_primitive_to_draw.clear();
	particles_to_draw.clear();
	ordering_by_materials.clear();
	cubes_to_draw.clear();
	using_program = false;
	changed_material = true;
	current_material = nullptr;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_DEBUG("Destroying 3D Renderer");
	rendering_cameras.clear();

	debug_draw->Clear();
	RELEASE(debug_draw);
	
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height, ComponentCamera* camera)
{
	BROFILER_CATEGORY("Renderer On Resize", Profiler::Color::Violet);
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
	if (editor_camera)
	{
		if (editor_camera->GetViewportTexture() == nullptr)
			data->AddInt("MSAA_Level", 2);
		else data->AddInt("MSAA_Level", editor_camera->GetViewportTexture()->GetCurrentMSAALevel());
	}
	

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

DebugDraw * ModuleRenderer3D::GetDebugDraw() const
{
	return debug_draw;
}

void ModuleRenderer3D::AddLight(ComponentLight * light)
{
	if (light != nullptr)
	{
		switch (light->GetLightType())
		{
		case DIRECTIONAL_LIGHT:

			for (uint i = 0; i < MAX_DIR_LIGHT; ++i)
			{
				if (dir_lights[i] == nullptr)
				{
					dir_lights[i] = light;
					directional_light_count++;
					return;
				}
			}
			break;
		case SPOT_LIGHT:
			for (uint i = 0; i < MAX_SPO_LIGHT; ++i)
			{
				if (spo_lights[i] == nullptr)
				{
					spo_lights[i] = light;
					spot_light_count++;
					return;
				}
			}
			break;
		case POINT_LIGHT:
			for (uint i = 0; i < MAX_POI_LIGHT; ++i)
			{
				if (poi_lights[i] == nullptr)
				{
					poi_lights[i] = light;
					point_light_count++;
					return;
				}
			}
			break;
		}
	}
}
void ModuleRenderer3D::RemoveLight(ComponentLight * light)
{
	if (light != nullptr)
	{
		switch (light->GetLightType())
		{
		case DIRECTIONAL_LIGHT:
			for (uint i = 0; i < MAX_DIR_LIGHT; ++i)
			{
				if (dir_lights[i] == light)
				{
					dir_lights[i] = nullptr;
					directional_light_count--;
					return;
				}
			}
			break;
		case SPOT_LIGHT:
			for (uint i = 0; i < MAX_SPO_LIGHT; ++i)
			{
				if (spo_lights[i] == light)
				{
					spo_lights[i] = nullptr;
					spot_light_count--;
					return;
				}
			}
			break;
		case POINT_LIGHT:
			for (uint i = 0; i < MAX_POI_LIGHT; ++i)
			{
				if (poi_lights[i] == light)
				{
					poi_lights[i] = nullptr;
					point_light_count--;
					return;
				}
			}
			break;
		}
	}
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
		//CONSOLE_ERROR("Error loading shader program binary %s\n", gluErrorString(error));

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
	if (using_program == true && id == current_shaderprogram) return;

	using_program = true;
	current_shaderprogram = id;

	glUseProgram(current_shaderprogram);
	
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error at use shader program: %s\n", gluErrorString(error));
	}
	else
	{
		current_shader_program = id;
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

void ModuleRenderer3D::SetUniformUInt(uint program, const char * name, uint data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform1ui(modelLoc, data);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform matrix %s: %s\n", name, gluErrorString(error));
	}
}


void ModuleRenderer3D::SetUniformInt(uint program, const char * name, int data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform1i(modelLoc, data);

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

void ModuleRenderer3D::SetUniformVector2(uint program, const char * name, float2 data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform2f(modelLoc, data.x, data.y);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("Error Setting uniform float %s: %s\n", name, gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformVector3(uint program, const char * name, float3 data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	if (modelLoc != -1)
		glUniform3f(modelLoc, data.x, data.y, data.z);
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

void ModuleRenderer3D::AddParticleToDraw(ComponentParticleEmmiter * particle)
{
	BROFILER_CATEGORY("Renderer Add Prticle to Draw", Profiler::Color::Brown);
	particles_to_draw.push_back(particle);
}

void ModuleRenderer3D::SendLight(uint program)
{
	// ----------------------------------------
	// ---------- SEND LIGHTING ---------------
	// ----------------------------------------
	// First send Camera Position, just once.

	if (game_camera == nullptr || game_camera->GetGameObject() == nullptr)
		return;

	ComponentTransform * c_trans = (ComponentTransform*)game_camera->GetGameObject()->GetComponent(Component::CompTransform);

	SetUniformVector3(program, "viewPos", c_trans->GetGlobalPosition());

	std::string plstr, tmp;

	// Directional Lights
	for (uint i = 0; i < MAX_DIR_LIGHT; ++i)
	{
		plstr = "dirLights[" + std::to_string(i) + "].";
		if (dir_lights[i] == nullptr || dir_lights[i]->IsActive() == false)
		{
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), false);
		}
		else
		{
			ComponentTransform* trans = nullptr;
			trans = (ComponentTransform*)dir_lights[i]->GetGameObject()->GetComponent(Component::CompTransform);

			tmp = plstr + "direction";
			SetUniformVector3(program, tmp.c_str(), trans->GetMatrix().WorldZ());
			tmp = plstr + "ambient";
			SetUniformVector3(program, tmp.c_str(), float3(dir_lights[i]->GetAmbient(), dir_lights[i]->GetAmbient(), dir_lights[i]->GetAmbient()));
			tmp = plstr + "diffuse";
			SetUniformVector3(program, tmp.c_str(), float3(dir_lights[i]->GetDiffuse(), dir_lights[i]->GetDiffuse(), dir_lights[i]->GetDiffuse()));
			tmp = plstr + "specular";
			SetUniformVector3(program, tmp.c_str(), float3(dir_lights[i]->GetSpecular(), dir_lights[i]->GetSpecular(), dir_lights[i]->GetSpecular()));
			tmp = plstr + "color";
			SetUniformVector4(program, tmp.c_str(), dir_lights[i]->GetColorAsFloat4());
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), true);

		}
	}

	// Spot Lights
	for (uint i = 0; i < MAX_SPO_LIGHT; ++i)
	{
		plstr = "spotLights[" + std::to_string(i) + "].";
		if (spo_lights[i] == nullptr || spo_lights[i]->IsActive() == false)
		{
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), false);
		}
		else
		{

			ComponentTransform* trans = nullptr;
			trans = (ComponentTransform*)spo_lights[i]->GetGameObject()->GetComponent(Component::CompTransform);


			tmp = plstr + "position";
			SetUniformVector3(program, tmp.c_str(), trans->GetGlobalPosition()); 
			tmp = plstr + "direction";
			SetUniformVector3(program, tmp.c_str(), trans->GetMatrix().WorldZ()); 
			tmp = plstr + "constant";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "linear";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "quadratic";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "ambient";
			SetUniformVector3(program, tmp.c_str(), float3(spo_lights[i]->GetAmbient(), spo_lights[i]->GetAmbient(), spo_lights[i]->GetAmbient()));
			tmp = plstr + "diffuse";
			SetUniformVector3(program, tmp.c_str(), float3(spo_lights[i]->GetDiffuse(), spo_lights[i]->GetDiffuse(), spo_lights[i]->GetDiffuse()));
			tmp = plstr + "specular";
			SetUniformVector3(program, tmp.c_str(), float3(spo_lights[i]->GetSpecular(), spo_lights[i]->GetSpecular(), spo_lights[i]->GetSpecular()));
			tmp = plstr + "color";
			SetUniformVector4(program, tmp.c_str(), spo_lights[i]->GetColorAsFloat4());
			tmp = plstr + "cutOff";
			SetUniformFloat(program, tmp.c_str(), cos(spo_lights[i]->GetCutOff()*DEGTORAD));
			tmp = plstr + "outercutOff";
			SetUniformFloat(program, tmp.c_str(), cos(spo_lights[i]->GetOuterCutOff()*DEGTORAD));
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), true);
		}
	}

	//Point Lights
	for (uint i = 0; i < MAX_POI_LIGHT; ++i)
	{
		plstr = "pointLights[" + std::to_string(i) + "].";
		if (poi_lights[i] == nullptr || poi_lights[i]->IsActive() == false)
		{
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), false);
		}
		else
		{

			ComponentTransform* trans = nullptr;
			trans = (ComponentTransform*)poi_lights[i]->GetGameObject()->GetComponent(Component::CompTransform);

			tmp = plstr + "position";
			SetUniformVector3(program, tmp.c_str(), trans->GetGlobalPosition()); //light->GetLightPosition());
			tmp = plstr + "constant";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "linear";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "quadratic";
			SetUniformFloat(program, tmp.c_str(), 1.0f);
			tmp = plstr + "ambient";
			SetUniformVector3(program, tmp.c_str(), float3(poi_lights[i]->GetAmbient(), poi_lights[i]->GetAmbient(), poi_lights[i]->GetAmbient()));
			tmp = plstr + "diffuse";
			SetUniformVector3(program, tmp.c_str(), float3(poi_lights[i]->GetDiffuse(), poi_lights[i]->GetDiffuse(), poi_lights[i]->GetDiffuse()));
			tmp = plstr + "specular";
			SetUniformVector3(program, tmp.c_str(), float3(poi_lights[i]->GetSpecular(), poi_lights[i]->GetSpecular(), poi_lights[i]->GetSpecular()));
			tmp = plstr + "color";
			SetUniformVector4(program, tmp.c_str(), poi_lights[i]->GetColorAsFloat4());
			tmp = plstr + "active";
			SetUniformBool(program, tmp.c_str(), true);
		}
	}

}

int ModuleRenderer3D::GetDirectionalLightCount() const
{
	return directional_light_count;
}
int ModuleRenderer3D::GetSpotLightCount() const
{
	return spot_light_count;
}
int ModuleRenderer3D::GetPointLightCount() const
{
	return point_light_count;
}
void ModuleRenderer3D::SetDepthMap()
{
	glGenFramebuffers(1, &depth_mapFBO);

	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_mapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	// Not rendering any color so:
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ModuleRenderer3D::DrawFromLightForShadows()
{
	if (dir_lights[0] != nullptr)
	{
	
	near_plane = 1.0f, far_plane = 750.0f;

	float4x4 light_projection = OrthoProjection(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	ComponentTransform* trans = (ComponentTransform*)dir_lights[0]->GetGameObject()->GetComponent(Component::CompTransform);

	float4x4 light_view = float4x4::LookAt(float3(0,0,1), trans->GetMatrix().WorldZ(), float3(0,1,0), trans->GetMatrix().WorldY());

	float4x4 light_space_mat = light_projection * light_view;

	std::list<GameObject*> scene_gos = App->scene->scene_gameobjects;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_mapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (std::list<GameObject*>::iterator it = scene_gos.begin(); it != scene_gos.end(); it++)
	{
		ComponentMeshRenderer* mesh = (ComponentMeshRenderer*) (*it)->GetComponent(Component::CompMeshRenderer);
		if (mesh != nullptr)
			SendObjectToDepthShader(mesh, light_space_mat);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//DrawZBuffer();
	}
}

void ModuleRenderer3D::SendObjectToDepthShader(ComponentMeshRenderer* mesh, float4x4 lightSpaceMat)
{
	if (mesh == nullptr || mesh->GetMesh() == nullptr) return;
	if (mesh->GetMesh()->id_indices == 0) mesh->GetMesh()->LoadToMemory();

	uint program = 0;
	ShaderProgram* shader = App->resources->GetShaderProgram("depth_shader_program");
	program = shader->GetProgramID();
	UseShaderProgram(program);

	SetUniformMatrix(program, "model", mesh->GetGameObject()->GetGlobalTransfomMatrix().Transposed().ptr());
	SetUniformMatrix(program, "lightSpaceMatrix", lightSpaceMat.ptr());
}
// -----------------------------------------------
