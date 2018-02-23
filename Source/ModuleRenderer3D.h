#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include <list>

class ComponentMeshRenderer;
class ComponentLight;
class Primitive;
class ComponentCamera;

#define MAX_DIR_LIGHT 2
#define MAX_SPO_LIGHT 8
#define MAX_POI_LIGHT 8

#define SHADOW_HEIGHT 1024
#define SHADOW_WIDTH 1024

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleRenderer3D();

	bool Init(Data* editor_config = nullptr);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height, ComponentCamera* camera);
	void SetWireframeMode();
	void SaveData(Data* data);

	void SetActiveDepthTest(bool active);
	void SetActiveCullTest(bool active);
	void SetActiveFog(bool active);

	bool GetActiveDepthTest() const;
	bool GetActiveCullTest() const;
	bool GetActiveFog() const;

	void ActiveSkybox(bool active);
	bool IsUsingSkybox()const;

	void AddMeshToDraw(ComponentMeshRenderer* mesh);
	void ResetRender();

	void BindArrayBuffer(uint id) const;
	void BindElementArrayBuffer(uint id) const;

	void UnbindArraybuffer() const;
	void UnbindElementArrayBuffer() const;

	void AddLight(ComponentLight* light);
	void RemoveLight(ComponentLight* light);

	//Shaders
	uint GenVertexArrayObject() const;
	void BindVertexArrayObject(uint id) const;
	void UnbindVertexArrayObject() const;

	uint CreateVertexShader(const char* source);
	uint CreateFragmentShader(const char* source);
	void DeleteShader(uint shader_id);

	uint GetProgramBinary(uint program_id, uint buff_size, char* buff) const;
	int GetProgramSize(uint program_id) const;
	void LoadProgramFromBinary(uint program_id, uint buff_size, const char* buff);

	void EnableVertexAttributeArray(uint id);
	void DisableVertexAttributeArray(uint id);
	void SetVertexAttributePointer(uint id, uint element_size, uint elements_gap, uint infogap);

	void UseShaderProgram(uint id);
	
	void SetUniformBool(uint program, const char* name, bool data);
	void SetUniformFloat(uint program, const char* name, float data);
	void SetUniformVector3(uint program, const char* name, float3 data);
	void SetUniformVector4(uint program, const char* name, float4 data);
	void SetUniformMatrix(uint program, const char* name, float* data);
	void SetUniformUInt(uint program, const char* name, uint data);
	void SetUniformInt(uint program, const char* name, int data);

	uint CreateShaderProgram();
	void AttachShaderToProgram(uint program_id, uint shader_id);
	bool LinkProgram(uint program_id);
	void DeleteProgram(uint program_id);

	void SendLight(uint program_id);
	int GetDirectionalLightCount() const;
	int GetSpotLightCount() const;
	int GetPointLightCount() const;

	void SetDepthMap();
	void DrawFromLightForShadows();
	void SendObjectToDepthShader(ComponentMeshRenderer* mesh, float4x4 lightSpaceMat);
	void SendObjectToRenderDepth(ComponentMeshRenderer* mesh);
private:
	void DrawSceneGameObjects(ComponentCamera* active_camera, bool is_editor_camera);
	void DrawMesh(ComponentMeshRenderer* mesh, ComponentCamera* active_camera);
	void DrawEditorScene();
	void DrawSceneCameras(ComponentCamera* camera);
	void DrawDebugCube(ComponentMeshRenderer* mesh, ComponentCamera* active_camera);
public:

	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	std::list<ComponentCamera*> rendering_cameras;
	ComponentCamera* editor_camera;
	ComponentCamera* game_camera;

private:
	bool use_vsync;
	bool is_using_depth_test;
	bool is_using_cull_test;
	bool is_using_fog;

	bool testing_light;
	bool use_skybox;

	int lights_count;

	std::list<ComponentMeshRenderer*> dynamic_mesh_to_draw;

	ComponentLight* dir_lights[MAX_DIR_LIGHT];
	ComponentLight* poi_lights[MAX_POI_LIGHT];
	ComponentLight* spo_lights[MAX_SPO_LIGHT];

	int directional_light_count;
	int point_light_count;
	int spot_light_count;

	std::list<Primitive*> debug_primitive_to_draw;

	uint depth_map;
	uint depth_mapFBO;
};