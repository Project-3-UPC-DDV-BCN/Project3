#pragma once

#include "Script.h"
#include <map>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>
#include "Component.h"

class GameObject;
class Data;

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
	void OnTriggerEnter(GameObject* other_collider);
	void OnTriggerStay(GameObject* other_collider);
	void OnTriggerExit(GameObject* other_collider);
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


	MonoMethod* GetFunction(const char* functionName, int parameters);
	void CallFunction(MonoMethod* function, void** parameter);

	void AddFieldsToMonoObjectList();

private:
	void ConvertMonoType(MonoType* type, ScriptField& script_field);
	void CreateSelfGameObject();

	void FillSavingData();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

public:
	bool inside_function;
	MonoObject* mono_self_object;

private:
	MonoDomain* mono_domain;
	MonoAssembly* mono_assembly;
	MonoClass* mono_class;
	MonoImage* mono_image;
	MonoObject* mono_object;
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
	MonoMethod* on_trigger_enter;
	MonoMethod* on_trigger_stay;
	MonoMethod* on_trigger_exit;
	MonoMethod* on_enable;
	MonoMethod* on_disable;
	MonoMethod* on_complete;
	MonoMethod* on_fail;

	std::vector<ScriptField*> script_fields;


};

