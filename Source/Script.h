#pragma once
#include "Resource.h"
#include "MathGeoLib/float3.h"
#include "MathGeoLib/float2.h"
#include "MathGeoLib/float4.h"
#include <vector>
#include <map>

class Texture;
class GameObject;

struct ScriptField {
	enum PropertyType {
		Integer, Decimal, Float, Bool, String, GameObject, Animation, Vector2, Vector3, Vector4, Texture, Audio, None
	};
	std::string fieldName;
	PropertyType propertyType = None;
};

class Script :
	public Resource
{
public:
	Script();
	~Script();

	enum ScriptType
	{
		CsScript, LuaScript, UnknownScript
	};

	virtual bool LoadScript(std::string script_path) = 0;
	virtual void SetAttachedGameObject(GameObject* gameobject) = 0;
	virtual void InitScript() = 0;
	virtual void StartScript() = 0;
	virtual void UpdateScript() = 0;
	virtual void OnCollisionEnter(GameObject* gameobject) = 0;
	virtual void OnCollisionStay(GameObject* gameobject) = 0;
	virtual void OnCollisionExit(GameObject* gameobject) = 0;
	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;

	virtual void SetIntProperty(const char* propertyName, int value) = 0;
	virtual int GetIntProperty(const char* propertyName) = 0;
	virtual void SetDoubleProperty(const char* propertyName, double value) = 0;
	virtual double GetDoubleProperty(const char* propertyName) = 0;
	virtual void SetFloatProperty(const char* propertyName, float value) = 0;
	virtual float GetFloatProperty(const char* propertyName) = 0;
	virtual void SetBoolProperty(const char* propertyName, bool value) = 0;
	virtual bool GetBoolProperty(const char* propertyName) = 0;
	virtual void SetStringProperty(const char* propertyName, const char* value) = 0;
	virtual std::string GetStringProperty(const char* propertyName) = 0;
	virtual void SetGameObjectProperty(const char* propertyName, GameObject* value) = 0;
	virtual GameObject* GetGameObjectProperty(const char* propertyName) = 0;
	virtual void SetVec2Property(const char* propertyName, float2 value) = 0;
	virtual float2 GetVec2Property(const char* propertyName) = 0;
	virtual void SetVec3Property(const char* propertyName, float3 value) = 0;
	virtual float3 GetVec3Property(const char* propertyName) = 0;
	virtual void SetVec4Property(const char* propertyName, float4 value) = 0;
	virtual float4 GetVec4Property(const char* propertyName) = 0;
	virtual void SetTextureProperty(const char* propertyName, Texture* value) = 0;
	virtual Texture* GetTextureProperty(const char* propertyName) = 0;

	virtual std::vector<ScriptField*> GetScriptFields() = 0;

	virtual void SetScriptType(ScriptType type);
	virtual ScriptType GetScriptType();

	void Save(Data& data) const = 0;
	bool Load(Data& data) = 0;
	void CreateMeta() const = 0;
	virtual void FillSavingData() = 0;

public:
	//Saving Data
	std::map<std::string, int> int_fields;
	std::map<std::string, double> double_fields;
	std::map<std::string, float> float_fields;
	std::map<std::string, bool> bool_fields;
	std::map<std::string, std::string> string_fields;
	std::map<std::string, GameObject*> gameobject_fields;
	std::map<std::string, float2> vec2_fields;
	std::map<std::string, float3> vec3_fields;
	std::map<std::string, float4> vec4_fields;
	
private:
	ScriptType script_type;
};

