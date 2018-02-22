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
	void OnCollisionEnter();
	void OnCollisionStay();
	void OnCollisionExit();
	void OnEnable();
	void OnDisable();

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

	//AUDIO
	bool IsMuted();
	void SetMute(bool set);

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

	std::vector<ScriptField*> script_fields;
	bool modifying_self;
	std::map<MonoObject*, GameObject*> created_gameobjects;
	GameObject* active_gameobject;
	bool inside_function;

};

