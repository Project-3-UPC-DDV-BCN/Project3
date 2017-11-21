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
#include "Mesh.h"
#include "Material.h"
#include "ModuleScene.h"
#include "RenderTextureMSAA.h"
#include "CubeMap.h"
#include "SceneWindow.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "Glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	use_vsync = true;
	is_using_lightning = false;
	is_using_depth_test = false;
	is_using_cull_test = false;
	is_using_color_material = false;
	is_using_texture2D = false;
	is_using_fog = false;
	testing_light = false;
	name = "Renderer";
	editor_camera = nullptr;
	game_camera = nullptr;
	use_skybox = true;
	lights_count = 0;
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

		////Initialize Projection Matrix
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		////Initialize Modelview Matrix
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			CONSOLE_DEBUG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
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
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.2f, 0.2f, 0.2f, 1);
		lights[0].diffuse.Set(1, 1, 1, 1);
		lights[0].specular.Set(1, 1, 1, 1);
		lights[0].SetPos(-1.0f, 1.0f, 1.0f);
		lights[0].Init();
		lights_count++;

		lights[1].ref = GL_LIGHT1;
		lights[1].ambient.Set(0.2f, 0.2f, 0.2f, 1);
		lights[1].diffuse.Set(1, 1, 1, 1);
		lights[1].specular.Set(1, 1, 1, 1);
		lights[1].SetPos(-1.0f, 1.0f, 1.0f);
		lights[1].Init();
		lights_count++;

		GLfloat LightModelAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		lights[1].Active(true);
		glEnable(GL_COLOR_MATERIAL);
		is_using_lightning = true;
		is_using_depth_test = true;
		is_using_cull_test = true;
		is_using_color_material = true;
		is_using_texture2D = true;

		glEnable(GL_MULTISAMPLE);
	}
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	ms_timer.Start();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glEnable(GL_LIGHTING);

	if (editor_camera != nullptr && editor_camera->GetViewportTexture() != nullptr)
	{
		editor_camera->GetViewportTexture()->Bind();
		editor_camera->UpdateProjection();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(editor_camera->GetViewMatrix());
	}

	if (is_using_lightning)
	{
		if (testing_light)
		{
			for (uint i = 1; i < lights_count; ++i)
				lights[i].Active(false);
			lights[7].Active(true);
		}
		else
		{
			for (uint i = 1; i < lights_count; ++i)
				lights[i].Active(true);
		}
		for (uint i = 0; i < lights_count; ++i)
			lights[i].Render();
	}

	DrawEditorScene();

	for (std::list<ComponentCamera*>::iterator it = rendering_cameras.begin(); it != rendering_cameras.end(); it++)
	{
		DrawSceneCameras(*it);
	}
	
	dynamic_mesh_to_draw.clear();

	//EditorUI can't be drawn in wireframe mode!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Disable Lighting before draw editor or shadows will appear in menu bars and ligth will affect editor colors.
	glDisable(GL_LIGHTING);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	App->editor->DrawEditor();

	App->editor->performance_window->AddModuleData(this->name, ms_timer.ReadMs());

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::DrawEditorScene()
{
	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		App->scene->DrawSkyBox(editor_camera->camera_frustum.pos);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
	}

	pPlane pl(0, 1, 0, 0);
	pl.SetPos(editor_camera->camera_frustum.pos);
	pl.color = { 1,1,1,1 };
	pl.Render();
	glEnable(GL_LIGHTING);
	DrawSceneGameObjects(editor_camera, true);
}

void ModuleRenderer3D::DrawSceneCameras(ComponentCamera * camera)
{
	if (camera == nullptr || camera->GetViewportTexture() == nullptr) return;

	camera->GetViewportTexture()->Bind();

	camera->UpdateProjection();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());

	if (use_skybox)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		App->scene->DrawSkyBox(camera->camera_frustum.pos);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
	}

	DrawSceneGameObjects(camera, false);
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
				DebugAABB aabb((*it)->GetMesh()->box);
				aabb.color = { 0,1,0,1 };
				aabb.Render();
			}
		}
		DrawMesh(*it);
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
					DrawMesh(*it);
				}
			}
		}
		else
		{
			DrawMesh(*it);
			if ((*it)->GetGameObject()->IsSelected())
			{
				DebugAABB aabb((*it)->GetMesh()->box);
				aabb.color = { 0,1,0,1 };
				aabb.Render();
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
				frustum.Render();
			}
		}
	}

	if (App->scene->draw_octree)
	{
		App->scene->octree.DebugDraw();
	}
	
	active_camera->GetViewportTexture()->Render();
	active_camera->GetViewportTexture()->Unbind();

}

void ModuleRenderer3D::DrawMesh(ComponentMeshRenderer * mesh)
{
	if (mesh == nullptr || mesh->GetMesh() == nullptr) return;
	if (mesh->GetMesh()->id_indices == 0) mesh->GetMesh()->LoadToMemory();

	glPushMatrix();
	glMultMatrixf(mesh->GetGameObject()->GetOpenGLMatrix());

	Material* material = mesh->GetMaterial();
	if (material != nullptr)
	{
		material->LoadToMemory();
	}
	//VERTICES
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->GetMesh()->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//NORMALS
	if (mesh->GetMesh()->id_normals > 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->GetMesh()->id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}
	//TEXTURE_COORDS
	if (mesh->GetMesh()->id_texture_coords > 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->GetMesh()->id_texture_coords);
		glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	}
	//COLORS
	if (mesh->GetMesh()->id_colors > 0)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->GetMesh()->id_colors);
		glColorPointer(3, GL_FLOAT, 0, NULL);
	}
	//INDICES
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetMesh()->id_indices);
	glDrawElements(GL_TRIANGLES, mesh->GetMesh()->num_indices, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (material != nullptr)
	{
		material->UnloadFromMemory();
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();

	glEnable(GL_COLOR_MATERIAL);

	mesh->GetGameObject()->UpdateGlobalMatrix();
}

void ModuleRenderer3D::AddMeshToDraw(ComponentMeshRenderer * mesh)
{
	dynamic_mesh_to_draw.push_back(mesh);
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_DEBUG("Destroying 3D Renderer");
	SDL_GL_DeleteContext(context);

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

void ModuleRenderer3D::SetActiveLighting(bool active)
{
	is_using_lightning = active;

	if (active)
	{
		for (uint i = 0; i < lights_count; ++i)
			lights[i].Active(true);
	}
	else 
	{
		for (uint i = 0; i < lights_count; ++i)
			lights[i].Active(false);
	}
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

void ModuleRenderer3D::SetActiveColorMaterial(bool active)
{
	is_using_color_material = active;

	if (active)
	{
		glEnable(GL_COLOR_MATERIAL);
	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);
	}
}

void ModuleRenderer3D::SetActiveTexture2D(bool active)
{
	is_using_texture2D = active;

	if (active)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
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

bool ModuleRenderer3D::GetActiveLighting() const
{
	return is_using_lightning;
}

bool ModuleRenderer3D::GetActiveDepthTest() const
{
	return is_using_depth_test;
}

bool ModuleRenderer3D::GetActiveCullTest() const
{
	return is_using_cull_test;
}

bool ModuleRenderer3D::GetActiveColorMaterial() const
{
	return is_using_color_material;
}

bool ModuleRenderer3D::GetActiveTexture2D() const
{
	return is_using_texture2D;
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


