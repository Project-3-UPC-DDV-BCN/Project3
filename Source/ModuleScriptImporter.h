#pragma once
#include "Module.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <vector>
#include <map>
#include "Component.h"

class Script;
class CSScript;
class ComponentRectTransform;
class GameObject;

struct DLLMethodParamsInfo
{
	std::string name;
	std::string type;
	std::string full_type;
	std::string description;
	bool is_out = false;
	bool is_ref = false;
};

struct DLLMethodInfo
{
	std::string description;
	std::string returning_type;
	std::string method_name;
	std::string declaration;
	std::string params_description;
	bool is_static = false;
	std::vector<DLLMethodParamsInfo> params;
};

struct DLLPropertyInfo
{
	std::string description;
	std::string name;
	std::string returning_type;
	std::string declaration;
	bool is_static = false;
};

struct DLLFieldsInfo
{
	std::string description;
	std::string name;
	std::string returning_type;
	std::string declaration;
	bool is_static = false;
};

struct DLLClassInfo
{
	std::string name;
	std::vector<DLLPropertyInfo> properties;
	std::vector<DLLMethodInfo> methods;
	std::vector<DLLFieldsInfo> fields;
};

class NSScriptImporter
{
public:
	NSScriptImporter() { current_script = nullptr; };
	~NSScriptImporter() {};

	void AddCreatedGameObjectToList(MonoObject* object, GameObject* go);
	void AddCreatedComponentToList(MonoObject* object, Component* comp);
	GameObject* GetGameObjectFromMonoObject(MonoObject* object);
	Component* GetComponentFromMonoObject(MonoObject* object);
	MonoObject* GetMonoObjectFromGameObject(GameObject* go);
	MonoObject* GetMonoObjectFromComponent(Component* component);
	Component::ComponentType CsToCppComponent(std::string component_type);
	std::string CppComponentToCs(Component::ComponentType component_type);

	//GAMEOBJECT
	void SetGameObjectName(MonoObject * object, MonoString* name);
	MonoString* GetGameObjectName(MonoObject* object);
	void CreateGameObject(MonoObject * object);
	MonoObject* GetSelfGameObject();
	void SetGameObjectActive(MonoObject * object, mono_bool active);
	mono_bool GetGameObjectIsActive(MonoObject* object);
	void SetGameObjectTag(MonoObject * object, MonoString* tag);
	MonoString* GetGameObjectTag(MonoObject* object);
	void SetGameObjectLayer(MonoObject * object, MonoString* layer);
	MonoString* GetGameObjectLayer(MonoObject* object);
	void SetGameObjectStatic(MonoObject * object, mono_bool value);
	mono_bool GameObjectIsStatic(MonoObject* object);
	MonoObject* DuplicateGameObject(MonoObject* object);
	void SetGameObjectParent(MonoObject* object, MonoObject* parent);
	MonoObject* GetGameObjectChild(MonoObject* object, int index);
	MonoObject* GetGameObjectChildString(MonoObject* object, MonoString* name);
	int GetGameObjectChildCount(MonoObject* object);
	MonoObject* FindGameObject(MonoString* gameobject_name);
	MonoObject* GetGameObjectParent(MonoObject* object);
	MonoArray* GetSceneGameObjects(MonoObject* object);
	MonoArray* GetObjectsInFrustum(MonoObject * pos, MonoObject* front, MonoObject* up, float nearPlaneDist, float farPlaneDist);
	MonoArray* GetAllChilds(MonoObject* object);

	//COMPONENT
	void SetComponentActive(MonoObject* object, bool active); 
	MonoObject* AddComponent(MonoObject* object, MonoReflectionType* type);
	MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type, int index);
	void DestroyComponent(MonoObject* object, MonoObject* cmp);

	//TRANSFORM
	void SetPosition(MonoObject * object, MonoObject * vector3);
	MonoObject* GetPosition(MonoObject* object, mono_bool is_global);
	void SetRotation(MonoObject * object, MonoObject * vector3);
	MonoObject* GetRotation(MonoObject* object, mono_bool is_global);
	void SetScale(MonoObject * object, MonoObject * vector3);
	MonoObject* GetScale(MonoObject* object, mono_bool is_global);
	void LookAt(MonoObject * object, MonoObject * vector3);
	MonoObject* GetForward(MonoObject* object);
	MonoObject* GetRight(MonoObject* object);
	MonoObject* GetUp(MonoObject* object);
	void RotateAroundAxis(MonoObject* object, MonoObject* axis, float angle);
	void SetIncrementalRotation(MonoObject * object, MonoObject * vector3);
	void SetQuatRotation(MonoObject * object, MonoObject * quat);
	MonoObject* GetQuatRotation(MonoObject* object);

	//RECTTRANSFORM
	void SetRectPosition(MonoObject * object, MonoObject * vector3);
	MonoObject* GetRectPosition(MonoObject * object);
	void SetRectRotation(MonoObject * object, MonoObject * vector3);
	MonoObject* GetRectRotation(MonoObject * object);
	void SetRectSize(MonoObject * object, MonoObject * vector3);
	MonoObject* GetRectSize(MonoObject * object);
	void SetRectAnchor(MonoObject * object, MonoObject * vector3);
	MonoObject* GetRectAnchor(MonoObject * object);
	mono_bool GetOnClick(MonoObject * object);
	mono_bool GetOnClickDown(MonoObject * object);
	mono_bool GetOnClickUp(MonoObject * object);
	mono_bool GetOnMouseEnter(MonoObject * object);
	mono_bool GetOnMouseOver(MonoObject * object);
	mono_bool GetOnMouseOut(MonoObject * object);

	//TEXT
	void SetText(MonoObject * object, MonoString* t);
	MonoString* GetText(MonoObject * object);

	//PROGRESSBAR
	void SetPercentageProgress(MonoObject * object, float progress);
	float GetPercentageProgress(MonoObject * object);

	//RADAR
	void AddEntity(MonoObject * object, MonoObject * game_object);
	void RemoveEntity(MonoObject * object, MonoObject * game_object);
	void RemoveAllEntities(MonoObject * object);
	void SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString* marker_name);

	//FACTORY
	void StartFactory(MonoObject * object);
	MonoObject* Spawn(MonoObject* object);
	void SetSpawnPosition(MonoObject * object, MonoObject * vector3);
	void SetSpawnRotation(MonoObject * object, MonoObject * vector3);
	void SetSpawnScale(MonoObject * object, MonoObject * vector3);

	//DATA SAVE/LOAD
	void AddString(MonoString* name, MonoString* string);
	void AddInt(MonoString* name, int value);
	void AddFloat(MonoString* name, float value);
	MonoString* GetString(MonoString* name);
	int GetInt(MonoString* name);
	float GetFloat(MonoString* name);

	//VECTOR/QUATERNION
	MonoObject* ToQuaternion(MonoObject * object);
	MonoObject* ToEulerAngles(MonoObject * object);
	MonoObject* RotateTowards(MonoObject* current, MonoObject* target, float angle);

	//TIME
	void SetTimeScale(MonoObject* object, float scale);
	float GetTimeScale();
	float GetDeltaTime();
	float GetTimeSinceStart();

	//TIMER
	void Start(MonoObject* object, float time);
	float ReadTime(MonoObject* object);

	//INPUT
	mono_bool IsKeyDown(MonoString * key_name);
	mono_bool IsKeyUp(MonoString* key_name);
	mono_bool IsKeyRepeat(MonoString* key_name);
	mono_bool IsMouseDown(int mouse_button);
	mono_bool IsMouseUp(int mouse_button);
	mono_bool IsMouseRepeat(int mouse_button);
	MonoObject* GetMousePosition();
	int GetMouseXMotion();
	int GetMouseYMotion();
	int GetControllerJoystickMove(int pad, MonoString* axis);
	int GetControllerButton(int pad, MonoString* button);
	void RumbleController(int pad, float strength, int ms);

	//AUDIO
	bool IsMuted();
	void SetMute(bool set);
	int GetVolume();
	void SetVolume(MonoObject* obj, int volume);
	int GetPitch();
	void SetPitch(int pitch);
	void SetRTPCvalue(MonoString* name, float value);

	//AUDIOSOURCE
	bool Play(MonoObject * object, MonoString* name);
	bool Stop(MonoObject * object, MonoString* name);
	bool Send(MonoObject * object, MonoString* name);
	bool SetMyRTPCvalue(MonoObject * object, MonoString* name, float value);
	bool SetState(MonoObject* object, MonoString* group, MonoString* state);

	//PARTICLE EMMITER
	void PlayEmmiter(MonoObject * object);
	void StopEmmiter(MonoObject * object);

	//RIGIDBODY
	void SetLinearVelocity(MonoObject * object, float x, float y, float z);
	void SetAngularVelocity(MonoObject * object, float x, float y, float z); 
	void AddTorque(MonoObject* object, float x, float y, float z, int force_type);
	void SetKinematic(MonoObject * object, bool kinematic);
	void SetTransformGO(MonoObject * object, bool transform_go);
	bool IsKinematic(MonoObject * object);
	bool IsTransformGO(MonoObject * object);
	void SetRBPosition(MonoObject * object, float x, float y, float z);
	void SetRBRotation(MonoObject * object, float x, float y, float z);
	MonoObject* GetRBPosition(MonoObject* object);

	//GOAPAGENT
	mono_bool GetBlackboardVariableB(MonoObject * object, MonoString* name);
	float GetBlackboardVariableF(MonoObject * object, MonoString* name);
	int GetNumGoals(MonoObject * object);
	MonoString* GetGoalName(MonoObject * object, int index);
	MonoString* GetGoalConditionName(MonoObject * object, int index);
	void SetGoalPriority(MonoObject * object, int index, int priority);
	int GetGoalPriority(MonoObject * object, int index);
	void CompleteAction(MonoObject * object);
	void FailAction(MonoObject * object);
	void SetBlackboardVariable(MonoObject * object, MonoString* name, float value);
	void SetBlackboardVariable(MonoObject * object, MonoString* name, bool value);

	//RANDOM
	int RandomInt(MonoObject* object);
	float RandomFloat(MonoObject* object);
	float RandomRange(float min, float max);

	//APPLICATION
	void LoadScene(MonoString* scene_name);
	void Quit();

	//SCRIPT
	void SetBoolField(MonoObject* object, MonoString* field_name, bool value);
	bool GetBoolField(MonoObject* object, MonoString* field_name);
	void SetIntField(MonoObject* object, MonoString* field_name, int value);
	int GetIntField(MonoObject* object, MonoString* field_name);
	void SetFloatField(MonoObject* object, MonoString* field_name, float value);
	float GetFloatField(MonoObject* object, MonoString* field_name);
	void SetDoubleField(MonoObject* object, MonoString* field_name, double value);
	double GetDoubleField(MonoObject* object, MonoString* field_name);
	void SetStringField(MonoObject* object, MonoString* field_name, MonoString* value);
	MonoString* GetStringField(MonoObject* object, MonoString* field_name);
	void SetGameObjectField(MonoObject* object, MonoString* field_name, MonoObject* value);
	MonoObject* GetGameObjectField(MonoObject* object, MonoString* field_name);
	void SetVector3Field(MonoObject* object, MonoString* field_name, MonoObject* value);
	MonoObject* GetVector3Field(MonoObject* object, MonoString* field_name);
	void SetQuaternionField(MonoObject* object, MonoString* field_name, MonoObject* value);
	MonoObject* GetQuaternionField(MonoObject* object, MonoString* field_name);
	void CallFunction(MonoObject* object, MonoString* function_name);

	//RESOURCES
	MonoObject* LoadPrefab(MonoString* prefab_name);

	//PHYSICS
	void Explosion(MonoObject* world_pos, float radius, float explosive_impulse);
	MonoObject* PhysicsRayCast(MonoObject* origin, MonoObject* direction, float max_distance);
	MonoArray* PhysicsRayCastAll(MonoObject* origin, MonoObject* direction, float max_distance);

	//COLLIDER
	MonoObject* ColliderGetGameObject(MonoObject* object);
	MonoObject* ColliderGetRigidBody(MonoObject* object);
	bool ColliderIsTrigger(MonoObject* object);
	void ColliderSetTrigger(MonoObject* object, bool trigger);
	MonoObject* ClosestPoint(MonoObject* object, MonoObject* position);

	//DEBUG DRAW
	void DebugDrawLine(MonoObject* from, MonoObject* to, MonoObject* color);

	std::map<MonoObject*, GameObject*> created_gameobjects;
	std::map<MonoObject*, Component*> created_components;
	CSScript* current_script;

private:
	
	//std::map<MonoObject*, PerfTimer*> created_timers;
	/*GameObject* active_gameobject;
	Component* active_component;*/
};

class ModuleScriptImporter :
	public Module
{
public:
	ModuleScriptImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleScriptImporter();

	bool Init(Data* editor_config = nullptr);

	//Retuns the library path if created or an empty string
	std::string ImportScript(std::string path);
	Script* LoadScriptFromLibrary(std::string path);

	MonoDomain* GetDomain() const;
	MonoImage* GetEngineImage() const;

	std::string CompileScript(std::string assets_path, std::string& lib);
	/*void SetCurrentScript(CSScript* script);
	CSScript* GetCurrentSctipt() const;*/

	static NSScriptImporter* ns_importer;

	std::vector<DLLClassInfo> engine_dll_info;

	void AddGameObjectsInfoToMono(std::list<GameObject*> scene_objects_list);

private:
	CSScript* DumpAssemblyInfo(MonoAssembly* assembly);
	MonoClass* DumpClassInfo(MonoImage* image, std::string& class_name, std::string& name_space);

	void DumpEngineDLLInfo(MonoAssembly* assembly, MonoImage* image, const char* engine_xml_path);

	void RegisterAPI();

	//GAMEOBJECT
	static void SetGameObjectName(MonoObject * object, MonoString* name);
	static MonoString* GetGameObjectName(MonoObject* object);
	static void CreateGameObject(MonoObject * object);
	static MonoObject* GetSelfGameObject();
	static void SetGameObjectActive(MonoObject * object, mono_bool active);
	static mono_bool GetGameObjectIsActive(MonoObject* object);
	static void SetGameObjectTag(MonoObject * object, MonoString* tag);
	static MonoString* GetGameObjectTag(MonoObject* object);
	static void SetGameObjectLayer(MonoObject * object, MonoString* layer);
	static MonoString* GetGameObjectLayer(MonoObject* object);
	static void SetGameObjectStatic(MonoObject * object, mono_bool value);
	static mono_bool GameObjectIsStatic(MonoObject* object);
	static MonoObject* DuplicateGameObject(MonoObject* object);
	static void SetGameObjectParent(MonoObject* object, MonoObject* parent);
	static MonoObject* GetGameObjectParent(MonoObject* object);
	static MonoObject* GetGameObjectChild(MonoObject* object, int index);
	static MonoObject* GetGameObjectChildString(MonoObject* object, MonoString* name);
	static int GetGameObjectChildCount(MonoObject* object);
	static MonoObject* FindGameObject(MonoString* gameobject_name);
	static MonoArray* GetSceneGameObjects(MonoObject* object);
	static MonoArray* GetObjectsInFrustum(MonoObject * pos, MonoObject* front, MonoObject* up, float nearPlaneDist, float farPlaneDist);
	static MonoArray* GetAllChilds(MonoObject* object);

	//COMPONENT
	static void SetComponentActive(MonoObject* object, bool active); 
	static MonoObject* AddComponent(MonoObject* object, MonoReflectionType* type);
	static MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type, int index);
	static void DestroyComponent(MonoObject * object, MonoObject* cmp);

	//TRANSFORM
	static void SetPosition(MonoObject * object, MonoObject * vector);
	static MonoObject* GetPosition(MonoObject* object, mono_bool is_global);
	static void SetRotation(MonoObject * object, MonoObject * vector);
	static MonoObject* GetRotation(MonoObject* object, mono_bool is_global);
	static void SetScale(MonoObject * object, MonoObject * vector);
	static MonoObject* GetScale(MonoObject* object, mono_bool is_global);
	static void LookAt(MonoObject * object, MonoObject * vector);
	static MonoObject* GetForward(MonoObject* object);
	static MonoObject* GetRight(MonoObject* object);
	static MonoObject* GetUp(MonoObject* object);
	static void RotateAroundAxis(MonoObject* object, MonoObject* axis, float angle);
	static void SetIncrementalRotation(MonoObject * object, MonoObject * vector3);
	static void SetQuatRotation(MonoObject * object, MonoObject * quat);
	static MonoObject* GetQuatRotation(MonoObject* object);

	//RECTTRANSFORM
	static void SetRectPosition(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectPosition(MonoObject * object);
	static void SetRectRotation(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectRotation(MonoObject * object);
	static void SetRectSize(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectSize(MonoObject * object);
	static void SetRectAnchor(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectAnchor(MonoObject * object);
	static mono_bool GetOnClick(MonoObject * object);
	static mono_bool GetOnClickDown(MonoObject * object);
	static mono_bool GetOnClickUp(MonoObject * object);
	static mono_bool GetOnMouseEnter(MonoObject * object);
	static mono_bool GetOnMouseOver(MonoObject * object);
	static mono_bool GetOnMouseOut(MonoObject * object);

	//TEXT
	static void SetText(MonoObject * object, MonoString* text);
	static MonoString* GetText(MonoObject * object);

	//PROGRESSBAR
	static void SetPercentageProgress(MonoObject * object, float progress);
	static float GetPercentageProgress(MonoObject * object);

	//RADAR
	static void AddEntity(MonoObject * object, MonoObject * game_object);
	static void RemoveEntity(MonoObject * object, MonoObject * game_object);
	static void RemoveAllEntities(MonoObject * object);
	static void SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString* marker_name);

	//DATA SAVE/LOAD
	static void AddString(MonoString* name, MonoString* string);
	static void AddInt(MonoString* name, int value);
	static void AddFloat(MonoString* name, float value);
	static MonoString* GetString(MonoString* name);
	static int GetInt(MonoString* name);
	static float GetFloat(MonoString* name);

	//FACTORY
	static void StartFactory(MonoObject * object);
	static MonoObject* Spawn(MonoObject* object);
	static void SetSpawnPosition(MonoObject * object, MonoObject * vector3);
	static void SetSpawnRotation(MonoObject * object, MonoObject * vector3);
	static void SetSpawnScale(MonoObject * object, MonoObject * vector3);

	//VECTOR/QUATERNION
	static MonoObject* ToQuaternion(MonoObject * object);
	static MonoObject* ToEulerAngles(MonoObject * object);
	static MonoObject* RotateTowards(MonoObject* current, MonoObject* target, float angle);

	//TIME
	static void SetTimeScale(MonoObject* object, float scale);
	static float GetTimeScale();
	static float GetDeltaTime();
	static float GetTimeSinceStart();

	//INPUT
	static mono_bool IsKeyDown(MonoString * key_name);
	static mono_bool IsKeyUp(MonoString* key_name);
	static mono_bool IsKeyRepeat(MonoString* key_name);
	static mono_bool IsMouseDown(int mouse_button);
	static mono_bool IsMouseUp(int mouse_button);
	static mono_bool IsMouseRepeat(int mouse_button);
	static MonoObject* GetMousePosition();
	static int GetMouseXMotion();
	static int GetMouseYMotion();
	static int GetControllerJoystickMove(int pad, MonoString* axis);
	static int GetControllerButton(int pad, MonoString* button);
	static void RumbleController(int pad, float strength, int ms);

	//CONSOLE
	static void Log(MonoObject* object);
	static void Warning(MonoObject* object);
	static void Error(MonoObject* object);

	//AUDIO
	static bool IsMuted();
	static void SetMute(bool set);
	static int GetVolume();
	static int GetPitch();
	static void SetPitch(int pitch);
	static void SetRTPCvalue(MonoString* name, float value);

	static bool Play(MonoObject * object, MonoString * name);
	static bool Stop(MonoObject * object, MonoString * name);
	static bool Send(MonoObject * object, MonoString * name);
	static bool SetMyRTPCvalue(MonoObject * object, MonoString* name, float value);
	static void SetState(MonoObject* object, MonoString* group, MonoString* state);
	static void SetVolume(MonoObject* object, int value);

	//PARTICLE EMMITER
	static void PlayEmmiter(MonoObject * object);
	static void StopEmmiter(MonoObject * object);

	//RIGIDBODY
	static void SetLinearVelocity(MonoObject * object, float x, float y, float z);
	static void SetAngularVelocity(MonoObject * object, float x, float y, float z);
	static void AddTorque(MonoObject * object, float x, float y, float z, int force_type);
	static bool IsKinematic(MonoObject * object);
	static void SetKinematic(MonoObject * object, bool kinematic);
	static void SetTransformGO(MonoObject * object, bool transform_go);
	static bool IsTransformGO(MonoObject * object);
	static void SetRBPosition(MonoObject * object, float x, float y, float z);
	static void SetRBRotation(MonoObject * object, float x, float y, float z);
	static MonoObject* GetRBPosition(MonoObject* object);

	//GOAP AGENT
	static mono_bool GetBlackboardVariableB(MonoObject * object, MonoString* name);
	static float GetBlackboardVariableF(MonoObject * object, MonoString* name);
	static int GetNumGoals(MonoObject * object);
	static MonoString* GetGoalName(MonoObject * object, int index);
	static MonoString* GetGoalConditionName(MonoObject * object, int index);
	static void SetGoalPriority(MonoObject * object, int index, int priority);
	static int GetGoalPriority(MonoObject * object, int index);
	static void CompleteAction(MonoObject * object);
	static void FailAction(MonoObject * object);
	static void SetBlackboardVariable(MonoObject * object, MonoString* name, float value);
	static void SetBlackboardVariableB(MonoObject * object, MonoString* name, bool value);

	//RANDOM
	static int RandomInt(MonoObject* object);
	static float RandomFloat(MonoObject* object);
	static float RandomRange(float min, float max);

	//APPLICATION
	static void LoadScene(MonoString* scene_name);
	static void Quit();

	//SCRIPT
	static void SetBoolField(MonoObject* object, MonoString* field_name, bool value);
	static bool GetBoolField(MonoObject* object, MonoString* field_name);
	static void SetIntField(MonoObject* object, MonoString* field_name, int value);
	static int GetIntField(MonoObject* object, MonoString* field_name);
	static void SetFloatField(MonoObject* object, MonoString* field_name, float value);
	static float GetFloatField(MonoObject* object, MonoString* field_name);
	static void SetDoubleField(MonoObject* object, MonoString* field_name, double value);
	static double GetDoubleField(MonoObject* object, MonoString* field_name);
	static void SetStringField(MonoObject* object, MonoString* field_name, MonoString* value);
	static MonoString* GetStringField(MonoObject* object, MonoString* field_name);
	static void SetGameObjectField(MonoObject* object, MonoString* field_name, MonoObject* value);
	static MonoObject* GetGameObjectField(MonoObject* object, MonoString* field_name);
	static void SetVector3Field(MonoObject* object, MonoString* field_name, MonoObject* value);
	static MonoObject* GetVector3Field(MonoObject* object, MonoString* field_name);
	static void SetQuaternionField(MonoObject* object, MonoString* field_name, MonoObject* value);
	static MonoObject* GetQuaternionField(MonoObject* object, MonoString* field_name);
	static void CallFunction(MonoObject* object, MonoString* function_name);

	//RESOURCES
	static MonoObject* LoadPrefab(MonoString* prefab_name);

	//PHYSICS
	static void Explosion(MonoObject* world_pos, float radius, float explosive_impulse);
	static MonoObject* PhysicsRayCast(MonoObject* origin, MonoObject* direction, float max_distance);
	static MonoArray* PhysicsRayCastAll(MonoObject* origin, MonoObject* direction, float max_distance);

	//COLLIDER
	static MonoObject* ColliderGetGameObject(MonoObject* object);
	static MonoObject* ColliderGetRigidBody(MonoObject* object);
	static bool ColliderIsTrigger(MonoObject* object);
	static void ColliderSetTrigger(MonoObject* object, bool trigger);
	static MonoObject* ClosestPoint(MonoObject* object, MonoObject* position);

	//DEBUG DRAW
	static void DebugDrawLine(MonoObject* from, MonoObject* to, MonoObject* color);

private:
	std::string mono_path;
	MonoDomain* mono_domain;
	MonoImage* mono_engine_image;
	static CSScript* current_script;
	static bool inside_function;
	MonoImage* mono_compiler_image;
};

