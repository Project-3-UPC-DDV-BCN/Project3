#pragma once

#include "Script.h"
#include <map>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>

class Data;
class GameObject;

class CSScript :
	public Script
{
public:
	CSScript();
	~CSScript();

	bool LoadScript(std::string script_path);
	void SetAttachedGameObject(GameObject* gameobject);
	void InitScript();
	void StartScript();
	void UpdateScript();
	void OnCollisionEnter(GameObject* other_collider);
	void OnCollisionStay(GameObject* other_collider);
	void OnCollisionExit(GameObject* other_collider);
	void OnEnable();
	void OnDisable();
	void OnComplete();
	void OnFail();

	void SetIntProperty(const char* propertyName, int value);
	int GetIntProperty(const char* propertyName);
	void SetDoubleProperty(const char* propertyName, double value);
	double GetDoubleProperty(const char* propertyName);
	void SetFloatProperty(const char* propertyName, float value);
	float GetFloatProperty(const char* propertyName);
	void SetBoolProperty(const char* propertyName, bool value);
	bool GetBoolProperty(const char* propertyName);
	void SetStringProperty(const char* propertyName, const char* value);
	std::string GetStringProperty(const char* propertyName);
	void SetGameObjectProperty(const char* propertyName, GameObject* value);
	GameObject* GetGameObjectProperty(const char* propertyName);
	void SetVec2Property(const char* propertyName, float2 value);
	float2 GetVec2Property(const char* propertyName);
	void SetVec3Property(const char* propertyName, float3 value);
	float3 GetVec3Property(const char* propertyName);
	void SetVec4Property(const char* propertyName, float4 value);
	float4 GetVec4Property(const char* propertyName);
	void SetTextureProperty(const char* propertyName, Texture* value);
	Texture* GetTextureProperty(const char* propertyName);

	std::vector<ScriptField*> GetScriptFields();

	void SetDomain(MonoDomain* mono_domain);
	void SetImage(MonoImage* mono_image);
	void SetClass(MonoClass* mono_class);
	void SetAssembly(MonoAssembly* mono_assembly);
	void SetNameSpace(std::string name_space);
	void SetClassName(std::string class_name);

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

	//COMPONENT
	MonoObject* AddComponent(MonoObject* object, MonoReflectionType* type);
	MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type);

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

	MonoObject* ToQuaternion(MonoObject * object);

	//TIME
	void SetTimeScale(MonoObject* object, float scale);
	float GetTimeScale();
	float GetDeltaTime();

	//TIMER
	void CreateTimer(MonoObject* object, float time);
	float ReadTime(MonoObject* object);
	void ResetTime(MonoObject* object);

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
	void SetVolume(int volume);
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

	//GOAPAGENT
	mono_bool GetBlackboardVariableB(MonoString* name) const;
	float GetBlackboardVariableF(MonoString* name);
	int GetNumGoals();
	MonoString* GetGoalName(int index);
	MonoString* GetGoalConditionName(int index);
	void SetGoalPriority(int index, int priority);
	int GetGoalPriority(int index);
	void CompleteAction();
	void FailAction();
	void SetBlackboardVariable(MonoString* name, float value);
	void SetBlackboardVariable(MonoString* name, bool value);
	


private:
	MonoMethod* GetFunction(const char* functionName, int parameters);
	void CallFunction(MonoMethod* function, void** parameter);
	void ConvertMonoType(MonoType* type, ScriptField& script_field);
	void CreateSelfGameObject();

	void FillSavingData();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	bool GameObjectIsValid();
	bool MonoObjectIsValid(MonoObject* object);
	MonoObject* FindGameObject(GameObject* go);

private:
	MonoDomain* mono_domain;
	MonoAssembly* mono_assembly;
	MonoClass* mono_class;
	MonoImage* mono_image;
	MonoObject* mono_object;
	MonoObject* mono_self_object;
	GameObject* attached_gameobject;
	std::string name_space;
	std::string class_name;
	uint32_t handle;

	MonoMethod* init;
	MonoMethod* start;
	MonoMethod* update;
	MonoMethod* on_collision_enter;
	MonoMethod* on_collision_stay;
	MonoMethod* on_collision_exit;
	MonoMethod* on_enable;
	MonoMethod* on_disable;
	MonoMethod* on_complete;
	MonoMethod* on_fail;

	std::vector<ScriptField*> script_fields;
	bool modifying_self;
	std::map<MonoObject*, GameObject*> created_gameobjects;
	//std::map<MonoObject*, PerfTimer*> created_timers;
	GameObject* active_gameobject;
	bool inside_function;

};

