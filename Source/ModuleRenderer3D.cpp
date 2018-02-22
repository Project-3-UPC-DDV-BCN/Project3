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
#include "Mesh.h"
#include "Material.h"
#include "ModuleScene.h"
#include "RenderTextureMSAA.h"
#include "CubeMap.h"
#include "SceneWindow.h"
#include "ModuleResources.h"
#include "ShaderProgram.h"


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

	for (uint i = 0; i < MAX_DIR_LIGHT; ++i)
		dir_lights[i] = nullptr;

	for (uint i = 0; i < MAX_SPO_LIGHT; ++i)
		spo_lights[i] = nullptr;

	for (uint i = 0; i < MAX_POI_LIGHT; ++i)
		poi_lights[i] = nullptr;

	directional_light_count = 0;
	point_light_count = 0;
	spot_light_count = 0;
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

	//Set Up Depth Map
	SetDepthMap();
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	ms_timer.Start();

	// Render from light's point of view
	DrawFromLightForShadows();


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

	dynamic_mesh_to_draw.clear();

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
		//glDisable(GL_DEPTH_TEST);
		//App->scene->DrawSkyBox(editor_camera->camera_frustum.pos);
		//glEnable(GL_DEPTH_TEST);
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
	//TODO: Calculate Normal Matrix here and send it as uniform.
	// We need this because transitioning to World Space gives a non accurate Normal.
	// Reminder: NormalMat = mat3(transpose(inverse(Model))) * normals;

	SendLight(program);

	BindVertexArrayObject(mesh->GetMesh()->id_vao);
	glDrawElements(GL_TRIANGLES, mesh->GetMesh()->num_indices, GL_UNSIGNED_INT, NULL);
	UnbindVertexArrayObject();
}

void ModuleRenderer3D::AddMeshToDraw(ComponentMeshRenderer * mesh)
{
	dynamic_mesh_to_draw.push_back(mesh);
}

void ModuleRenderer3D::ResetRender()
{
	dynamic_mesh_to_draw.clear();
	debug_primitive_to_draw.clear();
	rendering_cameras.clear();
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	CONSOLE_DEBUG("Destroying 3D Renderer");
	SDL_GL_DeleteContext(context);
	rendering_cameras.clear();
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
void ModuleRenderer3D::SendLight(uint program)
{
	// ----------------------------------------
	// ---------- SEND LIGHTING ---------------
	// ----------------------------------------
	// First send Camera Position, just once.

	SetUniformVector3(program, "viewPos", App->camera->GetPosition());


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
			/*ComponentLight* light = nullptr;
			light = (ComponentLight*)spo_lights[i]->GetGameObject()->GetComponent(Component::CompLight);*/
			ComponentTransform* light = nullptr;
			light = (ComponentTransform*)spo_lights[i]->GetGameObject()->GetComponent(Component::CompTransform);


			tmp = plstr + "position";
			SetUniformVector3(program, tmp.c_str(), light->GetGlobalPosition()); //light->GetLightPosition());
			tmp = plstr + "direction";
			SetUniformVector3(program, tmp.c_str(), light->GetMatrix().WorldZ()); //light->GetLightDirection());
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
			/*ComponentLight* light = nullptr;
			light = (ComponentLight*)poi_lights[i]->GetGameObject()->GetComponent(Component::CompLight);*/
			ComponentTransform* light = nullptr;
			light = (ComponentTransform*)poi_lights[i]->GetGameObject()->GetComponent(Component::CompTransform);

			tmp = plstr + "position";
			SetUniformVector3(program, tmp.c_str(), light->GetGlobalPosition()); //light->GetLightPosition());
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
	
	float near_plane = 1.0f, far_plane = 7.5f;

	float4x4 light_projection = float4x4::OpenGLPerspProjRH(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT);

	ComponentTransform* trans = (ComponentTransform*)dir_lights[0]->GetGameObject()->GetComponent(Component::CompTransform);

	float4x4 light_view = float4x4::LookAt(dir_lights[0]->view.front, trans->GetMatrix().WorldZ(), dir_lights[0]->view.up, trans->GetMatrix().WorldY());

	float4x4 light_space_mat = light_projection * light_view;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_mapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	std::list<GameObject*> scene_gos = App->scene->scene_gameobjects;
	for (std::list<GameObject*>::iterator it = scene_gos.begin(); it != scene_gos.end(); it++)
	{
		ComponentMeshRenderer* mesh = (ComponentMeshRenderer*) (*it)->GetComponent(Component::CompMeshRenderer);
		if (mesh != nullptr)
			SendObjectToDepthShader(mesh);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ModuleRenderer3D::SendObjectToDepthShader(ComponentMeshRenderer* mesh)
{
	if (mesh == nullptr || mesh->GetMesh() == nullptr) return;
	if (mesh->GetMesh()->id_indices == 0) mesh->GetMesh()->LoadToMemory();

	Material* material = mesh->GetMaterial();

	uint program = 0;
	if (material != nullptr)
	{
		ShaderProgram* shader = App->resources->GetShaderProgram("depth_shader_program");
		program = shader->GetProgramID();
		UseShaderProgram(program);
	}

	/*SetUniformMatrix(program, "view", active_camera->GetViewMatrix());
	SetUniformMatrix(program, "projection", active_camera->GetProjectionMatrix());
	SetUniformMatrix(program, "Model", mesh->GetGameObject()->GetGlobalTransfomMatrix().Transposed().ptr());*/

/*	BindVertexArrayObject(mesh->GetMesh()->id_vao);
	glDrawElements(GL_TRIANGLES, mesh->GetMesh()->num_indices, GL_UNSIGNED_INT, NULL);
	UnbindVertexArrayObject();*/
}
// ------------------------------------------------
