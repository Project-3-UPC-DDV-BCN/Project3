#include "CSScript.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Data.h"
#include "ModuleScriptImporter.h"
#include <ctime>
#include "ModuleScene.h"
#include "ModuleResources.h"
#include <mono/metadata/reflection.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/exception.h>
#include "ComponentTransform.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ComponentFactory.h"
#include "ComponentFactory.h"
#include "ComponentRectTransform.h"
#include "ComponentProgressBar.h"
#include "ModuleAudio.h"
#include "ComponentParticleEmmiter.h"
#include "ComponentAudioSource.h"
#include "AudioEvent.h"
#include "ComponentText.h"
#include "ComponentRigidBody.h"
#include "ComponentGOAPAgent.h"
#include "GOAPGoal.h"
#include "ComponentScript.h"
#include "ComponentRadar.h"

#pragma comment (lib, "../EngineResources/mono/lib/mono-2.0-sgen.lib")

CSScript::CSScript()
{
	mono_domain = nullptr;
	mono_assembly = nullptr;
	mono_class = nullptr;
	mono_image = nullptr;

	init = nullptr;
	start = nullptr;
	update = nullptr; 
	on_collision_enter = nullptr;
	on_collision_stay = nullptr;
	on_collision_exit = nullptr;
	on_trigger_enter = nullptr;
	on_trigger_stay = nullptr;
	on_trigger_exit = nullptr;
	on_enable = nullptr;
	on_disable = nullptr;
	on_complete = nullptr;
	on_fail = nullptr;

	active_gameobject = nullptr;
	attached_gameobject = nullptr;
	inside_function = false;
	modifying_self = false;

	SetType(Resource::ScriptResource);
	SetScriptType(ScriptType::CsScript);
	SetName("No Script");
}

CSScript::~CSScript()
{
	for (MonoComponent* mono_comp : created_components)
	{
		RELEASE(mono_comp);
	}
}

bool CSScript::LoadScript(std::string script_path)
{
	bool ret = false;
	SetLibraryPath(script_path);

	mono_domain = App->script_importer->GetDomain();

	if (mono_class)
	{
		mono_object = mono_object_new(App->script_importer->GetDomain(), mono_class);
	}
	if (mono_object)
	{
		mono_runtime_object_init(mono_object);
		
		init = GetFunction("Init", 0);
		start = GetFunction("Start", 0);
		update = GetFunction("Update", 0);
		on_collision_enter = GetFunction("OnCollisionEnter", 1);
		on_collision_stay = GetFunction("OnCollisionStay", 1);
		on_collision_exit = GetFunction("OnCollisionExit", 1);
		on_enable = GetFunction("OnEnable", 0);
		on_disable = GetFunction("OnDisable", 0);
		on_complete = GetFunction("OnComplete", 0);
		on_fail = GetFunction("OnFail", 0);

		ret = true;
	}
	
	return ret;
}

void CSScript::SetAttachedGameObject(GameObject * gameobject)
{
	attached_gameobject = gameobject;
	CreateSelfGameObject();
}

void CSScript::InitScript()
{
	if (init != nullptr)
	{
		CallFunction(init, nullptr);
		inside_function = false;
	}
}

void CSScript::StartScript()
{
	if (start != nullptr)
	{
		CallFunction(start, nullptr);
		inside_function = false;
	}
}

void CSScript::UpdateScript()
{
	if (update != nullptr)
	{
		CallFunction(update, nullptr);
		inside_function = false;
	}
}

void CSScript::OnCollisionEnter(GameObject* other_collider)
{
	if (on_collision_enter != nullptr)
	{
		MonoObject* new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_collision_enter, &param);
		inside_function = false;
	}
}

void CSScript::OnCollisionStay(GameObject* other_collider)
{
	if (on_collision_stay != nullptr)
	{
		MonoObject* new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_collision_stay, &param);
		inside_function = false;
	}
}

void CSScript::OnCollisionExit(GameObject* other_collider)
{
	if (on_collision_exit != nullptr)
	{
		MonoObject* new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_collision_exit, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerEnter(GameObject * other_collider)
{
	if (on_trigger_enter != nullptr)
	{
		MonoObject * new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_trigger_enter, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerStay(GameObject * other_collider)
{
	if (on_trigger_stay != nullptr)
	{
		MonoObject * new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_trigger_stay, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerExit(GameObject * other_collider)
{
	if (on_trigger_exit != nullptr)
	{
		MonoObject * new_object = nullptr;
		bool exist = false;
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				new_object = it->first;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			MonoClass * c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					created_gameobjects[new_object] = other_collider;
				}
			}
		}

		void* param = new_object;
		CallFunction(on_trigger_exit, &param);
		inside_function = false;
	}
}

void CSScript::OnEnable()
{
	if (on_enable != nullptr)
	{
		CallFunction(on_enable, nullptr);
		inside_function = false;
	}
}

void CSScript::OnDisable()
{
	if (on_disable != nullptr)
	{
		CallFunction(on_disable, nullptr);
		inside_function = false;
	}
}

void CSScript::OnComplete()
{
	if (on_complete != nullptr)
	{
		CallFunction(on_complete, nullptr);
		inside_function = false;
	}
}

void CSScript::OnFail()
{
	if (on_fail != nullptr)
	{
		CallFunction(on_fail, nullptr);
		inside_function = false;
	}
}

void CSScript::SetIntProperty(const char * propertyName, int value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

int CSScript::GetIntProperty(const char * propertyName)
{
	int value = 0;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.Integer)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetIntProperty: Trying to get a non int field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetDoubleProperty(const char * propertyName, double value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

double CSScript::GetDoubleProperty(const char * propertyName)
{
	double value = 0.0;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.Decimal)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetDoubleProperty: Trying to get a non double field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetFloatProperty(const char * propertyName, float value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);
	
	if(field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float CSScript::GetFloatProperty(const char * propertyName)
{
	float value = 0.0f;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.Float)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetFloatProperty: Trying to get a non float field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetBoolProperty(const char * propertyName, bool value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

bool CSScript::GetBoolProperty(const char * propertyName)
{
	bool value = false;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.Bool)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetBoolProperty: Trying to get a non bool field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetStringProperty(const char * propertyName, const char * value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoString* params = mono_string_new(mono_domain, value);
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

std::string CSScript::GetStringProperty(const char * propertyName)
{
	MonoString* value = nullptr;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.String)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetStringProperty: Trying to get a non string field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	if (value)
	{
		return mono_string_to_utf8(value);
	}
	
	return "";
}

void CSScript::SetGameObjectProperty(const char * propertyName, GameObject * value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);
	if (field)
	{
		MonoObject* last_object = mono_field_get_value_object(mono_domain, field, mono_object);
		if (last_object)
		{
			std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.find(last_object);
			created_gameobjects.erase(it);
		}
		void* params = value;
		mono_field_set_value(mono_object, field, params);
		MonoObject* object = mono_field_get_value_object(mono_domain, field, mono_object);
		created_gameobjects[object] = value;
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

GameObject * CSScript::GetGameObjectProperty(const char * propertyName)
{
	GameObject* value = nullptr;
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);
	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		ScriptField property_field;
		ConvertMonoType(type, property_field);
		if (property_field.propertyType == property_field.GameObject)
		{
			mono_field_get_value(mono_object, field, &value);
		}
		else
		{
			CONSOLE_ERROR("GetGameObjectProperty: Trying to get a non GameObject field");
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec2Property(const char * propertyName, float2 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_class_get_element_class(mono_type_get_class(type));
		MonoArray* array_value = mono_array_new(mono_domain, eclass, 2);

		mono_array_set(array_value, float, 0, value.x);
		mono_array_set(array_value, float, 1, value.y);

		mono_field_set_value(mono_object, field, array_value);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float2 CSScript::GetVec2Property(const char * propertyName)
{
	float2 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoArray* array_value = nullptr;
		mono_field_get_value(mono_object, field, &array_value);

		if (array_value != nullptr)
		{
			value.x = mono_array_get(array_value, float, 0);
			value.y = mono_array_get(array_value, float, 1);
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec3Property(const char * propertyName, float3 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_class_get_element_class(mono_type_get_class(type));
		MonoArray* array_value = mono_array_new(mono_domain, eclass, 3);

		mono_array_set(array_value, float, 0, value.x);
		mono_array_set(array_value, float, 1, value.y);
		mono_array_set(array_value, float, 2, value.z);

		mono_field_set_value(mono_object, field, array_value);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float3 CSScript::GetVec3Property(const char * propertyName)
{
	float3 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoArray* array_value = nullptr;
		mono_field_get_value(mono_object, field, &array_value);

		if (array_value != nullptr)
		{
			value.x = mono_array_get(array_value, float, 0);
			value.y = mono_array_get(array_value, float, 1);
			value.z = mono_array_get(array_value, float, 2);
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec4Property(const char * propertyName, float4 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_class_get_element_class(mono_type_get_class(type));
		MonoArray* array_value = mono_array_new(mono_domain, eclass, 3);

		mono_array_set(array_value, float, 0, value.x);
		mono_array_set(array_value, float, 1, value.y);
		mono_array_set(array_value, float, 2, value.z);
		mono_array_set(array_value, float, 3, value.w);

		mono_field_set_value(mono_object, field, array_value);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float4 CSScript::GetVec4Property(const char * propertyName)
{
	float4 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoArray* array_value = nullptr;
		mono_field_get_value(mono_object, field, &array_value);

		if (array_value != nullptr)
		{
			value.x = mono_array_get(array_value, float, 0);
			value.y = mono_array_get(array_value, float, 1);
			value.z = mono_array_get(array_value, float, 2);
			value.w = mono_array_get(array_value, float, 3);
		}
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetTextureProperty(const char * propertyName, Texture * value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);
	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

Texture * CSScript::GetTextureProperty(const char * propertyName)
{
	Texture* value = nullptr;
	MonoClassField* Field = mono_class_get_field_from_name(mono_class, propertyName);
	if (Field)
	{
		mono_field_get_value(mono_object, Field, &value);
	}
	else
	{
		CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

std::vector<ScriptField*> CSScript::GetScriptFields()
{
	void* iter = nullptr;
	if (mono_class)
	{
		MonoClassField* field = mono_class_get_fields(mono_class, &iter);

		script_fields.clear();

		while (field != nullptr)
		{
			uint32_t flags = mono_field_get_flags(field);
			if ((flags & MONO_FIELD_ATTR_PUBLIC) && (flags & MONO_FIELD_ATTR_STATIC) == 0)
			{
				ScriptField* property_field = new ScriptField();
				property_field->fieldName = mono_field_get_name(field);
				MonoType* type = mono_field_get_type(field);
				ConvertMonoType(type, *property_field);
				script_fields.push_back(property_field);
			}
			field = mono_class_get_fields(mono_class, &iter);
		}
	}

	return script_fields;
}

void CSScript::SetDomain(MonoDomain * mono_domain)
{
	this->mono_domain = mono_domain;
}

void CSScript::SetImage(MonoImage * mono_image)
{
	this->mono_image = mono_image;
}

void CSScript::SetClass(MonoClass * mono_class)
{
	this->mono_class = mono_class;
}

void CSScript::SetAssembly(MonoAssembly * mono_assembly)
{
	this->mono_assembly = mono_assembly;
}

void CSScript::SetNameSpace(std::string name_space)
{
	this->name_space = name_space;
}

void CSScript::SetClassName(std::string class_name)
{
	this->class_name = class_name;
}

MonoMethod * CSScript::GetFunction(const char * functionName, int parameters_count)
{
	MonoMethod* method = nullptr;

	if (mono_class != nullptr)
	{
		method = mono_class_get_method_from_name(mono_class, functionName, parameters_count);
	}

	return method;
}

void CSScript::CallFunction(MonoMethod * function, void ** parameter)
{
	if (function != nullptr)
	{
		inside_function = true;
		MonoObject* exception = nullptr;
		MonoMarshalSpec* spec;

		MonoObject* obj = mono_runtime_invoke(function, mono_object, parameter, &exception);

		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}
	}
}

void CSScript::ConvertMonoType(MonoType * type, ScriptField& script_field)
{
	std::string name = mono_type_get_name(type);

	switch (mono_type_get_type(type))
	{
	case MONO_TYPE_I4:
		script_field.propertyType = ScriptField::Integer;
		break;
	case MONO_TYPE_R4:
		script_field.propertyType = ScriptField::Float;
		break;
	case MONO_TYPE_BOOLEAN:
		script_field.propertyType = ScriptField::Bool;
		break;
	case MONO_TYPE_STRING:
		script_field.propertyType = ScriptField::String;
		break;
	case MONO_TYPE_CLASS:
		if(name == "TheEngine.TheGameObject") script_field.propertyType = ScriptField::GameObject;
		else if (name == "TheEngine.Texture") script_field.propertyType = ScriptField::Texture;
		else if (name == "TheEngine.Animation") script_field.propertyType = ScriptField::Animation;
		else if (name == "TheEngine.Audio") script_field.propertyType = ScriptField::Audio;
		else if (name == "TheEngine.Text") script_field.propertyType = ScriptField::Text;
		else if (name == "TheEngine.TheVector3") script_field.propertyType = ScriptField::Vector3;
		else if (name == "TheEngine.TheQuaternion") script_field.propertyType = ScriptField::Vector4;
		break;
	case MONO_TYPE_SZARRAY:
		break;
	case MONO_TYPE_VALUETYPE:
		break;
	case MONO_TYPE_GENERICINST:
		break;
	case MONO_TYPE_CHAR:
		break;
	case MONO_TYPE_I1:
		break;
	case MONO_TYPE_U1:
		break;
	case MONO_TYPE_I2:
		break;
	case MONO_TYPE_U2:
		break;
	case MONO_TYPE_U4:
		break;
	case MONO_TYPE_I8:
		break;
	case MONO_TYPE_U8:
		break;
	default:
		break;
	}
}

void CSScript::CreateSelfGameObject()
{
	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			mono_self_object = new_object;
			created_gameobjects[mono_self_object] = attached_gameobject;
		}
	}
}

void CSScript::SetGameObjectName(MonoObject * object, MonoString* name)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}
	if (!GameObjectIsValid())
	{
		return;
	}

	if (name != nullptr)
	{
		const char* new_name = mono_string_to_utf8(name);
		active_gameobject->SetName(new_name);
		App->scene->RenameDuplicatedGameObject(active_gameobject);
	}
}

MonoString* CSScript::GetGameObjectName(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}
	if (!GameObjectIsValid())
	{
		return nullptr;
	}
	return mono_string_new(mono_domain, active_gameobject->GetName().c_str());
}

void CSScript::SetGameObjectTag(MonoObject * object, MonoString * tag)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}
	if (!GameObjectIsValid())
	{
		return;
	}
	if (tag != nullptr)
	{
		const char* new_tag = mono_string_to_utf8(tag);
		active_gameobject->SetName(new_tag);
	}
}

MonoString * CSScript::GetGameObjectTag(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}
	return mono_string_new(mono_domain, active_gameobject->GetTag().c_str());
}

void CSScript::SetGameObjectLayer(MonoObject * object, MonoString * layer)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}
	if (!GameObjectIsValid())
	{
		return;
	}
	if (layer != nullptr)
	{
		const char* new_layer = mono_string_to_utf8(layer);
		active_gameobject->SetLayer(new_layer);
	}
}

MonoString * CSScript::GetGameObjectLayer(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}
	return mono_string_new(mono_domain, active_gameobject->GetLayer().c_str());
}

void CSScript::SetGameObjectStatic(MonoObject * object, mono_bool value)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	active_gameobject->SetStatic(value);
}

mono_bool CSScript::GameObjectIsStatic(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}
	return active_gameobject->IsStatic();
}

MonoObject * CSScript::DuplicateGameObject(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}
	GameObject* duplicated = App->scene->DuplicateGameObject(active_gameobject);
	if (duplicated)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(mono_domain, c);
			if (new_object)
			{
				created_gameobjects[new_object] = duplicated;
				return new_object;
			}
		}
	}
	
	return nullptr;
}

void CSScript::SetGameObjectParent(MonoObject * object, MonoObject * parent)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	GameObject* go_parent = nullptr;

	if (parent != nullptr)
	{
		go_parent = created_gameobjects.at(parent);
	}
	
	active_gameobject->SetParent(go_parent);
}

MonoObject* CSScript::GetGameObjectParent(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	GameObject* parent = active_gameobject->GetParent();
	if (parent)
	{
		MonoObject* parent_mono_object = FindMonoObject(parent);
		if (!parent_mono_object)
		{
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				MonoObject* new_object = mono_object_new(mono_domain, c);
				if (new_object)
				{
					created_gameobjects[new_object] = parent;
					return new_object;
				}
			}
		}
		return parent_mono_object;
	}

	return nullptr;
}

MonoObject * CSScript::GetGameObjectChild(MonoObject * object, int index)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	if (index >= 0 && index < active_gameobject->childs.size())
	{
		std::list<GameObject*>::iterator it = std::next(active_gameobject->childs.begin(), index);
		if (*it)
		{
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				MonoObject* new_object = mono_object_new(mono_domain, c);
				if (new_object)
				{
					created_gameobjects[new_object] = *it;
					return new_object;
				}
			}
		}
	}
	else
	{
		mono_raise_exception(mono_get_exception_argument_out_of_range(NULL));
	}
	
	return nullptr;
}

MonoObject * CSScript::GetGameObjectChildString(MonoObject * object, MonoString * name)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	const char* s_name = mono_string_to_utf8(name);

	for (std::list<GameObject*>::iterator it = active_gameobject->childs.begin(); it != active_gameobject->childs.end(); it++)
	{
		if (*it != nullptr && (*it)->GetName() == s_name)
		{
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				MonoObject* new_object = mono_object_new(mono_domain, c);
				if (new_object)
				{
					created_gameobjects[new_object] = *it;
					return new_object;
				}
			}
		}
	}

	return nullptr;
}

int CSScript::GetGameObjectChildCount(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return -1;
	}

	if (!GameObjectIsValid())
	{
		return -1;
	}

	return active_gameobject->childs.size();
}

MonoObject * CSScript::FindGameObject(MonoString * gameobject_name)
{
	const char* s_name = mono_string_to_utf8(gameobject_name);

	GameObject* go = App->scene->FindGameObjectByName(s_name);

	for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
	{
		if (it->second == go) return it->first;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			created_gameobjects[new_object] = go;
			return new_object;
		}
	}

	return nullptr;
}

MonoArray * CSScript::GetSceneGameObjects(MonoObject * object)
{
	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
	std::list<GameObject*> objects = App->scene->scene_gameobjects;

	if (c)
	{
		MonoArray* scene_objects = mono_array_new(mono_domain, c, objects.size());
		if (scene_objects)
		{
			int index = 0;
			for (GameObject* go : objects)
			{
				bool exist = false;
				for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
				{
					if (it->second == go)
					{
						mono_array_set(scene_objects, MonoObject*, index, it->first);
						index++;
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					MonoObject* new_object = mono_object_new(mono_domain, c);
					if (new_object)
					{
						mono_array_set(scene_objects, MonoObject*, index, new_object);
						index++;
						created_gameobjects[new_object] = go;
					}
				}
			}
			return scene_objects;
		}
	}
	return nullptr;
}

MonoArray * CSScript::GetObjectsInFrustum(MonoObject * pos, MonoObject * front, MonoObject * up, float nearPlaneDist, float farPlaneDist)
{

	MonoClass* c_pos = mono_object_get_class(pos);
	MonoClassField* x_field = mono_class_get_field_from_name(c_pos, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c_pos, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c_pos, "z");

	float3 frustum_pos;
	if (x_field) mono_field_get_value(pos, x_field, &frustum_pos.x);
	if (y_field) mono_field_get_value(pos, y_field, &frustum_pos.y);
	if (z_field) mono_field_get_value(pos, z_field, &frustum_pos.z);

	MonoClass* c_front = mono_object_get_class(front);
	x_field = mono_class_get_field_from_name(c_front, "x");
	y_field = mono_class_get_field_from_name(c_front, "y");
	z_field = mono_class_get_field_from_name(c_front, "z");

	float3 frustum_front;
	if (x_field) mono_field_get_value(front, x_field, &frustum_front.x);
	if (y_field) mono_field_get_value(front, y_field, &frustum_front.y);
	if (z_field) mono_field_get_value(front, z_field, &frustum_front.z);

	MonoClass* c_up = mono_object_get_class(up);
	x_field = mono_class_get_field_from_name(c_up, "x");
	y_field = mono_class_get_field_from_name(c_up, "y");
	z_field = mono_class_get_field_from_name(c_up, "z");

	float3 frustum_up;
	if (x_field) mono_field_get_value(up, x_field, &frustum_up.x);
	if (y_field) mono_field_get_value(up, y_field, &frustum_up.y);
	if (z_field) mono_field_get_value(up, z_field, &frustum_up.z);

	Frustum frustum;
	frustum.SetPos(frustum_pos);
	frustum.SetFront(frustum_front);
	frustum.SetUp(frustum_up);

	frustum.SetKind(math::FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlaneDist, farPlaneDist);

	float FOV = 60;
	float aspect_ratio = 1.3;
	frustum.SetHorizontalFovAndAspectRatio(FOV*DEGTORAD, aspect_ratio);
	frustum.ComputeProjectionMatrix();

	std::list<GameObject*> objects = App->scene->scene_gameobjects;
	std::list<GameObject*> obj_inFrustum;

	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		ComponentMeshRenderer* mesh = (ComponentMeshRenderer*)(*it)->GetComponent(Component::CompMeshRenderer);
		if (mesh == nullptr) continue;

		int vertex_num = mesh->bounding_box.NumVertices();

		for (int i = 0; i < 6; i++) //planes of frustum
		{
			int points_out = 0;
			for (int j = 0; j < vertex_num; j++) //vertex number of box
			{
				Plane plane = frustum.GetPlane(i);
				if (plane.IsOnPositiveSide(mesh->bounding_box.CornerPoint(j)))
				{
					points_out++;
				}
			}
			//if all the points are outside of a plane, the gameobject is not inside the frustum
			if (points_out < 8) obj_inFrustum.push_back((*it));
		}
	}
	// return obj_inFrustum; I don't know how to do that :S
	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");

	if (c)
	{
		MonoArray* scene_objects = mono_array_new(mono_domain, c, obj_inFrustum.size());
		if (scene_objects)
		{
			int index = 0;
			for (GameObject* go : obj_inFrustum)
			{
				bool exist = false;
				for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
				{
					if (it->second == go)
					{
						mono_array_set(scene_objects, MonoObject*, index, it->first);
						index++;
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					MonoObject* new_object = mono_object_new(mono_domain, c);
					if (new_object)
					{
						mono_array_set(scene_objects, MonoObject*, index, new_object);
						index++;
						created_gameobjects[new_object] = go;
					}
				}
			}
			return scene_objects;
		}
	}
	return nullptr;
}

MonoArray * CSScript::GetAllChilds(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
	std::vector<GameObject*> objects;

	active_gameobject->GetAllChilds(objects);

	if (c)
	{
		MonoArray* scene_objects = mono_array_new(mono_domain, c, objects.size());
		if (scene_objects)
		{
			int index = 0;
			for (GameObject* go : objects)
			{
				bool exist = false;
				for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
				{
					if (it->second == go)
					{
						mono_array_set(scene_objects, MonoObject*, index, it->first);
						index++;
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					MonoObject* new_object = mono_object_new(mono_domain, c);
					if (new_object)
					{
						mono_array_set(scene_objects, MonoObject*, index, new_object);
						index++;
						created_gameobjects[new_object] = go;
					}
				}
			}
			return scene_objects;
		}
	}
	return nullptr;
}

MonoObject* CSScript::AddComponent(MonoObject * object, MonoReflectionType * type)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoType* t = mono_reflection_type_get_type(type);
	std::string name = mono_type_get_name(t);

	const char* comp_name = "";
	Component::ComponentType comp_type = Component::CompUnknown;

	if (name == "TheEngine.TheTransform")
	{
		CONSOLE_ERROR("Can't add Transform component to %s. GameObjects cannot have more than 1 transform.", active_gameobject->GetName().c_str());
	}
	else if (name == "TheEngine.TheFactory")
	{
		comp_type = Component::CompTransform;
		comp_name = "TheTransform";
	}
	else if (name == "TheEngine.TheProgressBar")
	{
		comp_type = Component::CompProgressBar;
		comp_name = "TheProgressBar";
	}
	else if (name == "TheEngine.TheText")
	{
		comp_type = Component::CompText;
		comp_name = "TheText";
	}
	else if (name == "TheEngine.TheRigidBody")
	{
		comp_type = Component::CompRigidBody;
		comp_name = "TheRigidBody";
	}

	if (comp_type != Component::CompUnknown)
	{
		active_gameobject->AddComponent(comp_type);

		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == active_gameobject)
			{
				return it->first;
			}
		}

		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", comp_name);
		if (c)
		{
			MonoObject* new_object = mono_object_new(mono_domain, c);
			if (new_object)
			{
				return new_object;
			}
		}
	}
}

MonoObject* CSScript::GetComponent(MonoObject * object, MonoReflectionType * type, int index)
{

	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoType* t = mono_reflection_type_get_type(type);
	std::string name = mono_type_get_name(t);

	const char* comp_name = "";

	if (name == "TheEngine.TheTransform")
	{
		comp_name = "TheTransform";
	}
	else if (name == "TheEngine.TheFactory")
	{
		comp_name = "TheFactory";
	}
	else if (name == "TheEngine.TheRectTransform")
	{
		comp_name = "TheRectTransform";
	}
	else if (name == "TheEngine.TheProgressBar")
	{
		comp_name = "TheProgressBar";
	}
	else if (name == "TheEngine.TheAudioSource")
	{
		comp_name = "TheAudioSource";
	}
	else if (name == "TheEngine.TheParticleEmmiter")
	{
		comp_name = "TheParticleEmmiter";
	}
	else if (name == "TheEngine.TheText")
	{
		comp_name = "TheText";
	}
	else if (name == "TheEngine.TheRadar")
	{
		comp_name = "TheRadar";
	}
	else if (name == "TheEngine.TheRigidBody")
	{
		comp_name = "TheRigidBody";
	}
	else if (name == "TheEngine.TheGOAPAgent")
	{
		comp_name = "TheGOAPAgent";
	}
	else if (name == "TheEngine.TheScript")
	{
		comp_name = "TheScript";
	}

	Component::ComponentType cpp_type = CsToCppComponent(comp_name);

	int temp_index = index;
	if (cpp_type != Component::CompUnknown)
	{
		int comp_type_count = 0;
		for (Component* comp : active_gameobject->components_list)
		{
			if (comp->GetType() == cpp_type)
			{
				comp_type_count++;
			}
		}

		if (comp_type_count <= index)
		{
			if (comp_type_count == 0)
			{
				CONSOLE_ERROR("GetComponent method: %s does not exist in %s", comp_name, attached_gameobject->GetName().c_str());
			}
			else
			{
				CONSOLE_ERROR("GetComponent method: %s index is out of bounds", comp_name);
			}
			return nullptr;
		}

		for (Component* comp : active_gameobject->components_list)
		{
			if (comp->GetType() == cpp_type)
			{
				if (temp_index == 0)
				{
					for (MonoComponent* mono_comp : created_components)
					{
						if (mono_comp->component == comp)
						{
							return mono_comp->component_object;
						}
					}
					break;
				}
				else
				{
					temp_index--;
				}
			}
		}

		temp_index = index;
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", comp_name);
		if (c)
		{
			MonoObject* new_object = mono_object_new(mono_domain, c);
			if (new_object)
			{
				MonoComponent* mono_comp = new MonoComponent();
				mono_comp->component_object = new_object;
				mono_comp->attached_go = active_gameobject;
				created_components.push_back(mono_comp);
				created_gameobjects[new_object] = active_gameobject;
				for (Component* comp : active_gameobject->components_list)
				{
					if (comp->GetType() == cpp_type)
					{
						if (temp_index == 0)
						{
							mono_comp->component = comp;
							break;
						}
						else
						{
							temp_index--;
						}
					}
				}
				return new_object;
			}
		}
	}

	CONSOLE_ERROR("%s component type is unknown...", comp_name);
	return nullptr;
}

void CSScript::SetPosition(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_pos;

	if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

	ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
	transform->SetPosition(new_pos);
}

MonoObject* CSScript::GetPosition(MonoObject * object, mono_bool is_global)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 new_pos;
			if (is_global)
			{
				new_pos = transform->GetGlobalPosition();
			}
			else
			{
				new_pos = transform->GetLocalPosition();
			}

			if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
			if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
			if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

			return new_object;
		}
	}
	return nullptr;
}

void CSScript::SetRotation(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_rot;

	if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

	ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
	transform->SetRotation(new_rot);
}

MonoObject * CSScript::GetRotation(MonoObject * object, mono_bool is_global)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 new_rot;
			if (is_global)
			{
				new_rot = transform->GetGlobalRotation();
			}
			else
			{
				new_rot = transform->GetLocalRotation();
			}

			if (x_field) mono_field_set_value(new_object, x_field, &new_rot.x);
			if (y_field) mono_field_set_value(new_object, y_field, &new_rot.y);
			if (z_field) mono_field_set_value(new_object, z_field, &new_rot.z);

			return new_object;
		}
	}
	return nullptr;
}

void CSScript::SetScale(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_scale;

	if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

	ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
	transform->SetScale(new_scale);
}

MonoObject * CSScript::GetScale(MonoObject * object, mono_bool is_global)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 new_scale;
			if (is_global)
			{
				new_scale = transform->GetGlobalScale();
			}
			else
			{
				new_scale = transform->GetLocalScale();
			}

			if (x_field) mono_field_set_value(new_object, x_field, &new_scale.x);
			if (y_field) mono_field_set_value(new_object, y_field, &new_scale.y);
			if (z_field) mono_field_set_value(new_object, z_field, &new_scale.z);

			return new_object;
		}
	}
	return nullptr;
}

void CSScript::LookAt(MonoObject * object, MonoObject * vector)
{
}

void CSScript::SetRectPosition(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_pos;

	if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

	ComponentRectTransform* rect_transform = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
	rect_transform->SetPos(float2(new_pos.x, new_pos.y));
}

MonoObject * CSScript::GetRectPosition(MonoObject * object)
{
	return nullptr;
}

MonoObject * CSScript::GetForward(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 forward = transform->GetForward();

			if (x_field) mono_field_set_value(new_object, x_field, &forward.x);
			if (y_field) mono_field_set_value(new_object, y_field, &forward.y);
			if (z_field) mono_field_set_value(new_object, z_field, &forward.z);

			return new_object;
		}
	}

	return nullptr;
}

void CSScript::SetRectRotation(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_rot;

	if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

	ComponentRectTransform* rect_transform = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
	rect_transform->SetRotation(new_rot);
}

MonoObject * CSScript::GetRectRotation(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentRectTransform* rect_transform = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
			float3 new_rot;
			new_rot.x = rect_transform->GetLocalRotation().x;
			new_rot.y = rect_transform->GetLocalRotation().y;
			new_rot.z = rect_transform->GetLocalRotation().z;

			if (x_field) mono_field_set_value(new_object, x_field, &new_rot.x);
			if (y_field) mono_field_set_value(new_object, y_field, &new_rot.y);
			if (z_field) mono_field_set_value(new_object, z_field, &new_rot.z);

			return new_object;
		}
	}
	return nullptr;
}

void CSScript::SetRectSize(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_size;

	if (x_field) mono_field_get_value(vector3, x_field, &new_size.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_size.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_size.z);

	ComponentRectTransform* rect_transform = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
	rect_transform->SetSize(float2(new_size.x, new_size.y));
}

MonoObject * CSScript::GetRectSize(MonoObject * object)
{
	return nullptr;
}

MonoObject * CSScript::GetRight(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 right = transform->GetRight();

			if (x_field) mono_field_set_value(new_object, x_field, &right.x);
			if (y_field) mono_field_set_value(new_object, y_field, &right.y);
			if (z_field) mono_field_set_value(new_object, z_field, &right.z);

			return new_object;
		}
	}

	return nullptr;
}

void CSScript::SetRectAnchor(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_anchor;

	if (x_field) mono_field_get_value(vector3, x_field, &new_anchor.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_anchor.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_anchor.z);

	ComponentRectTransform* rect_transform = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
	rect_transform->SetAnchor(float2(new_anchor.x, new_anchor.y));
}

MonoObject * CSScript::GetRectAnchor(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheRectTransform");

	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 right = transform->GetRight();

			if (x_field) mono_field_set_value(new_object, x_field, &right.x);
			if (y_field) mono_field_set_value(new_object, y_field, &right.y);
			if (z_field) mono_field_set_value(new_object, z_field, &right.z);

			return new_object;
		}
	}

	return nullptr;
}

mono_bool CSScript::GetOnClick(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);
	
	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnClick();
	}

	return false;
}

mono_bool CSScript::GetOnClickDown(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);

	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnClickDown();
	}

	return false;
}

mono_bool CSScript::GetOnClickUp(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);

	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnClickUp();
	}

	return false;
}

mono_bool CSScript::GetOnMouseEnter(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);

	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnMouseEnter();
	}

	return false;
}

mono_bool CSScript::GetOnMouseOver(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);

	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnMouseOver();
	}

	return false;
}

mono_bool CSScript::GetOnMouseOut(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentRectTransform* rect_trans = (ComponentRectTransform*)active_gameobject->GetComponent(Component::CompRectTransform);

	if (rect_trans != nullptr)
	{
		return rect_trans->GetOnMouseOut();
	}

	return false;
}

void CSScript::SetText(MonoObject * object, MonoString* t)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	const char* txt = mono_string_to_utf8(t);

	ComponentText* text = (ComponentText*)active_gameobject->GetComponent(Component::CompText);
	if(text)
		text->SetText(txt);
}

MonoString* CSScript::GetText(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return mono_string_new(mono_domain, "");
	}

	if (!GameObjectIsValid())
	{
		return mono_string_new(mono_domain, "");
	}

	ComponentText* text = (ComponentText*)active_gameobject->GetComponent(Component::CompText);
	return mono_string_new(mono_domain, text->GetText().c_str());
}

MonoObject * CSScript::GetUp(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			ComponentTransform* transform = (ComponentTransform*)active_gameobject->GetComponent(Component::CompTransform);
			float3 up = transform->GetUp();

			if (x_field) mono_field_set_value(new_object, x_field, &up.x);
			if (y_field) mono_field_set_value(new_object, y_field, &up.y);
			if (z_field) mono_field_set_value(new_object, z_field, &up.z);

			return new_object;
		}
	}

	return nullptr;
}

void CSScript::SetPercentageProgress(MonoObject * object, float progress)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentProgressBar* progres_barr = (ComponentProgressBar*)active_gameobject->GetComponent(Component::CompProgressBar);

	progres_barr->SetProgressPercentage(progress);
}

float CSScript::GetPercentageProgress(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return 0;
	}

	if (!GameObjectIsValid())
	{
		return 0;
	}

	ComponentProgressBar* progres_barr = (ComponentProgressBar*)active_gameobject->GetComponent(Component::CompProgressBar);
	return progres_barr->GetProgressPercentage();
}

void CSScript::AddEntity(MonoObject * object, MonoObject * game_object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	ComponentRadar* radar = (ComponentRadar*)active_gameobject->GetComponent(Component::CompRadar);
	
	GameObject* go = FindGameObject(game_object);

	if (radar != nullptr)
	{
		if (go != nullptr)
		{
			radar->AddEntity(go);
		}
	}
}

void CSScript::RemoveEntity(MonoObject * object, MonoObject * game_object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	ComponentRadar* radar = (ComponentRadar*)active_gameobject->GetComponent(Component::CompRadar);

	GameObject* go = FindGameObject(game_object);

	if (radar != nullptr)
	{
		if (go != nullptr)
		{
			radar->RemoveEntity(go);
		}
	}
}

void CSScript::RemoveAllEntities(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentRadar* radar = (ComponentRadar*)active_gameobject->GetComponent(Component::CompRadar);

	if (radar != nullptr)
	{
		radar->RemoveAllEntities();
	}
}

void CSScript::SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString * marker_name)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentRadar* radar = (ComponentRadar*)active_gameobject->GetComponent(Component::CompRadar);
	
	GameObject* go = FindGameObject(game_object);

	if (radar != nullptr)
	{
		if (go != nullptr)
		{
			const char* txt = mono_string_to_utf8(marker_name);

			radar->AddMarkerToEntity(go, radar->GetMarker(txt));
		}
	}

}

void CSScript::StartFactory(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentFactory* factory = (ComponentFactory*)active_gameobject->GetComponent(Component::CompFactory);
	if(factory) factory->StartFactory();
}

MonoObject * CSScript::Spawn(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}

	ComponentFactory* factory = (ComponentFactory*)active_gameobject->GetComponent(Component::CompFactory);
	GameObject* go = nullptr;
	if (factory)
	{
		go = factory->Spawn();
	}

	if (go)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(mono_domain, c);
			if (new_object)
			{
				created_gameobjects[new_object] = go;
				return new_object;
			}
		}
	}
	return nullptr;
}

void CSScript::SetSpawnPosition(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_pos;

	if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

	ComponentFactory* factory = (ComponentFactory*)active_gameobject->GetComponent(Component::CompFactory);
	if(factory) factory->SetSpawnPos(new_pos);
}

void CSScript::SetSpawnRotation(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_rot;

	if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

	ComponentFactory* factory = (ComponentFactory*)active_gameobject->GetComponent(Component::CompFactory);
	if (factory) factory->SetSpawnRotation(new_rot);
}

void CSScript::SetSpawnScale(MonoObject * object, MonoObject * vector3)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	MonoClass* c = mono_object_get_class(vector3);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 new_scale;

	if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
	if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
	if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

	ComponentFactory* factory = (ComponentFactory*)active_gameobject->GetComponent(Component::CompFactory);
	if (factory) factory->SetSpawnScale(new_scale);
}

MonoObject * CSScript::ToQuaternion(MonoObject * object)
{
	if (!object)
	{
		return nullptr;
	}

	MonoClass* c = mono_object_get_class(object);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

	float3 rotation;

	if (x_field) mono_field_get_value(object, x_field, &rotation.x);
	if (y_field) mono_field_get_value(object, y_field, &rotation.y);
	if (z_field) mono_field_get_value(object, z_field, &rotation.z);

	math::Quat quat = math::Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);

	MonoClass* quaternion = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheQuaternion");
	if (quaternion)
	{
		MonoObject* new_object = mono_object_new(mono_domain, quaternion);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(quaternion, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(quaternion, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(quaternion, "z");
			MonoClassField* w_field = mono_class_get_field_from_name(quaternion, "w");

			if (x_field) mono_field_set_value(new_object, x_field, &quat.x);
			if (y_field) mono_field_set_value(new_object, y_field, &quat.y);
			if (z_field) mono_field_set_value(new_object, z_field, &quat.z);
			if (w_field) mono_field_set_value(new_object, w_field, &quat.w);

			return new_object;
		}
	}
	return nullptr;
}

void CSScript::SetTimeScale(MonoObject * object, float scale)
{
	App->time->time_scale = scale;
}

float CSScript::GetTimeScale()
{
	return App->time->time_scale;
}

float CSScript::GetDeltaTime()
{
	return App->time->GetGameDt();
}

void CSScript::CreateTimer(MonoObject * object, float time)
{
	
}

float CSScript::ReadTime(MonoObject * object)
{
	return 0.0f;
}

void CSScript::ResetTime(MonoObject * object)
{
}

mono_bool CSScript::IsKeyDown(MonoString * key_name)
{
	bool ret = false;
	const char* key = mono_string_to_utf8(key_name);
	SDL_Keycode code = App->input->StringToKey(key);
	if (code != SDL_SCANCODE_UNKNOWN)
	{
		if (App->input->GetKey(code) == KEY_DOWN) ret = true;
	}
	else
	{
		CONSOLE_WARNING("'%s' is not a key! Returned false by default", key);
	}

	return ret;
}

mono_bool CSScript::IsKeyUp(MonoString * key_name)
{
	bool ret = false;
	const char* key = mono_string_to_utf8(key_name);
	SDL_Keycode code = App->input->StringToKey(key);
	if (code != SDL_SCANCODE_UNKNOWN)
	{
		if (App->input->GetKey(code) == KEY_UP) ret = true;
	}
	else
	{
		CONSOLE_WARNING("'%s' is not a key! Returned false by default", key);
	}

	return ret;
}

mono_bool CSScript::IsKeyRepeat(MonoString * key_name)
{
	bool ret = false;
	const char* key = mono_string_to_utf8(key_name);
	SDL_Keycode code = App->input->StringToKey(key);
	if (code != SDL_SCANCODE_UNKNOWN)
	{
		if (App->input->GetKey(code) == KEY_REPEAT) ret = true;
	}
	else
	{
		CONSOLE_WARNING("'%s' is not a key! Returned false by default", key);
	}
	
	return ret;
}

mono_bool CSScript::IsMouseDown(int mouse_button)
{
	bool ret = false;
	if (mouse_button > 0 && mouse_button < 4)
	{
		if (App->input->GetMouseButton(mouse_button) == KEY_DOWN) ret = true;
	}
	else
	{
		CONSOLE_WARNING("%d is not a valid mouse button! Returned false by default");
	}

	return ret;
}

mono_bool CSScript::IsMouseUp(int mouse_button)
{
	bool ret = false;
	if (mouse_button > 0 && mouse_button < 4)
	{
		if (App->input->GetMouseButton(mouse_button) == KEY_UP) ret = true;
	}
	else
	{
		CONSOLE_WARNING("%d is not a valid mouse button! Returned false by default");
	}

	return ret;
}

mono_bool CSScript::IsMouseRepeat(int mouse_button)
{
	bool ret = false;
	if (mouse_button > 0 && mouse_button < 4)
	{
		if (App->input->GetMouseButton(mouse_button) == KEY_REPEAT) ret = true;
	}
	else
	{
		CONSOLE_WARNING("%d is not a valid mouse button! Returned false by default");
	}

	return ret;
}

MonoObject * CSScript::GetMousePosition()
{
	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (c)
	{
		MonoObject* new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			float mouse_x = App->input->GetMouseX();
			float mouse_y = App->input->GetMouseY();

			if (x_field) mono_field_set_value(new_object, x_field, &mouse_x);
			if (y_field) mono_field_set_value(new_object, y_field, &mouse_y);
			if (z_field) mono_field_set_value(new_object, z_field, 0);

			return new_object;
		}
	}
	return nullptr;
}

int CSScript::GetMouseXMotion()
{
	return App->input->GetMouseXMotion();
}

int CSScript::GetMouseYMotion()
{
	return App->input->GetMouseYMotion();
}

int CSScript::GetControllerJoystickMove(int pad, MonoString * axis)
{
	const char* key = mono_string_to_utf8(axis);
	JOYSTICK_MOVES code = App->input->StringToJoyMove(key);
	return App->input->GetControllerJoystickMove(pad,code);
}

int CSScript::GetControllerButton(int pad, MonoString * button)
{
	const char* key = mono_string_to_utf8(button);
	SDL_Keycode code = App->input->StringToKey(key);
	return App->input->GetControllerButton(pad,code);
}

void CSScript::RumbleController(int pad, float strength, int ms)
{
	App->input->RumbleController(pad, strength, ms);
}

void CSScript::CreateGameObject(MonoObject * object)
{
	if (!inside_function)
	{
		CONSOLE_ERROR("Can't create new GameObjects outside a function.");
		return;
	}
	GameObject* gameobject = App->scene->CreateGameObject();
	created_gameobjects[object] = gameobject;
}

MonoObject* CSScript::GetSelfGameObject()
{
	if (!attached_gameobject)
	{
		return nullptr;
	}
	else
	{
		if (mono_self_object != nullptr)
		{
			return mono_self_object;
		}
	}
}

void CSScript::SetGameObjectActive(MonoObject * object, mono_bool active)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}
	active_gameobject->SetActive(active);
}

mono_bool CSScript::GetGameObjectIsActive(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}
	return active_gameobject->IsActive();
}

void CSScript::FillSavingData()
{
	for (std::vector<ScriptField*>::const_iterator it = script_fields.begin(); it != script_fields.end(); it++)
	{
		switch ((*it)->propertyType)
		{
		case ScriptField::Integer:
		{
			int_fields[(*it)->fieldName] = GetIntProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Decimal:
		{
			double_fields[(*it)->fieldName] = GetDoubleProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Float:
		{
			float_fields[(*it)->fieldName] = GetFloatProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Bool:
		{
			bool_fields[(*it)->fieldName] = GetBoolProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::String:
		{
			string_fields[(*it)->fieldName] = GetStringProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::GameObject:
		{
			gameobject_fields[(*it)->fieldName] = GetGameObjectProperty((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Animation:
			break;
		case ScriptField::Vector2:
		{
			vec2_fields[(*it)->fieldName] = GetVec2Property((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Vector3:
		{
			vec3_fields[(*it)->fieldName] = GetVec3Property((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Vector4:
		{
			vec4_fields[(*it)->fieldName] = GetVec4Property((*it)->fieldName.c_str());
		}
		break;
		case ScriptField::Texture:
			break;
		case ScriptField::Audio:
			break;
		}
	}
}

void CSScript::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("script_name", GetName());
	data.AddUInt("UUID", GetUID());

	data.AddInt("int_fields_count", int_fields.size());
	data.AddInt("double_fields_count", double_fields.size());
	data.AddInt("float_fields_count", float_fields.size());
	data.AddInt("bool_fields_count", bool_fields.size());
	data.AddInt("string_fields_count", string_fields.size());
	data.AddInt("gameobject_fields_count", gameobject_fields.size());
	data.AddInt("vec2_fields_count", vec2_fields.size());
	data.AddInt("vec3_fields_count", vec3_fields.size());
	data.AddInt("vec4_fields_count", vec4_fields.size());

	data.CreateSection("int_fields");
	int i = 0;
	for (std::map<std::string, int>::const_iterator it = int_fields.begin(); it != int_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddInt("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("double_fields");
	i = 0;
	for (std::map<std::string, double>::const_iterator it = double_fields.begin(); it != double_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddDouble("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("float_fields");
	i = 0;
	for (std::map<std::string, float>::const_iterator it = float_fields.begin(); it != float_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddFloat("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("bool_fields");
	i = 0;
	for (std::map<std::string, bool>::const_iterator it = bool_fields.begin(); it != bool_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddBool("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("string_fields");
	i = 0;
	for (std::map<std::string, std::string>::const_iterator it = string_fields.begin(); it != string_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddString("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("gameobject_fields");
	i = 0;
	for (std::map<std::string, GameObject*>::const_iterator it = gameobject_fields.begin(); it != gameobject_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddInt("field_value", it->second ? it->second->GetUID() : 0);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("vec2_fields");
	i = 0;
	for (std::map<std::string, float2>::const_iterator it = vec2_fields.begin(); it != vec2_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddVector2("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("vec3_fields");
	i = 0;
	for (std::map<std::string, float3>::const_iterator it = vec3_fields.begin(); it != vec3_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddVector3("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
	data.CreateSection("vec4_fields");
	i = 0;
	for (std::map<std::string, float4>::const_iterator it = vec4_fields.begin(); it != vec4_fields.end(); it++)
	{
		data.CreateSection("field_" + std::to_string(i));
		data.AddString("field_name", it->first);
		data.AddVector4("field_value", it->second);
		data.CloseSection();
		i++;
	}
	data.CloseSection();
}

bool CSScript::Load(Data & data)
{
	bool ret = true;
	std::string library_path = data.GetString("library_path");

	CSScript* text = (CSScript*)App->script_importer->LoadScriptFromLibrary(library_path);
	if (!text)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			library_path = App->resources->CreateLibraryFile(Resource::ScriptResource, assets_path);
			if (!library_path.empty())
			{
				Load(data);
			}
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		SetAssetsPath(data.GetString("assets_path"));
		SetLibraryPath(data.GetString("library_path"));
		SetName(data.GetString("script_name"));
		SetUID(data.GetUInt("UUID"));
		mono_domain = text->mono_domain;
		mono_class = text->mono_class;
		mono_image = text->mono_image;
		mono_object = text->mono_object;
		init = text->init;
		start = text->start;
		update = text->update;
		on_collision_enter = text->on_collision_enter;
		on_collision_stay = text->on_collision_stay;
		on_collision_exit = text->on_collision_exit;
		on_enable = text->on_enable;
		on_disable = text->on_disable;

		int fields_count = data.GetInt("int_fields_count");
		data.EnterSection("int_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			int field_value = data.GetInt("field_value");
			data.LeaveSection();
			SetIntProperty(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("double_fields_count");
		data.EnterSection("double_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			double field_value = data.GetDouble("field_value");
			data.LeaveSection();
			SetDoubleProperty(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("float_fields_count");
		data.EnterSection("float_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			float field_value = data.GetFloat("field_value");
			data.LeaveSection();
			SetFloatProperty(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("bool_fields_count");
		data.EnterSection("bool_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			bool field_value = data.GetBool("field_value");
			data.LeaveSection();
			SetBoolProperty(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("string_fields_count");
		data.EnterSection("string_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			std::string field_value = data.GetString("field_value");
			data.LeaveSection();
			SetStringProperty(field_name.c_str(), field_value.c_str());
		}
		data.LeaveSection();
		fields_count = data.GetInt("gameobject_fields_count");
		data.EnterSection("gameobject_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			uint field_value = data.GetInt("field_value");
			GameObject* go = App->scene->FindGameObject(field_value);
			data.LeaveSection();
			SetGameObjectProperty(field_name.c_str(), go);
		}
		data.LeaveSection();
		fields_count = data.GetInt("vec2_fields_count");
		data.EnterSection("vec2_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			float2 field_value = data.GetVector2("field_value");
			data.LeaveSection();
			SetVec2Property(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("vec3_fields_count");
		data.EnterSection("vec3_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			float3 field_value = data.GetVector3("field_value");
			data.LeaveSection();
			SetVec3Property(field_name.c_str(), field_value);
		}
		data.LeaveSection();
		fields_count = data.GetInt("vec4_fields_count");
		data.EnterSection("vec4_fields");
		for (int i = 0; i < fields_count; i++)
		{
			data.EnterSection("field_" + std::to_string(i));
			std::string field_name = data.GetString("field_name");
			float4 field_value = data.GetVector4("field_value");
			data.LeaveSection();
			SetVec4Property(field_name.c_str(), field_value);
		}
		data.LeaveSection();
	}
	GetScriptFields();
	return ret;
}

void CSScript::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.SaveAsMeta(GetAssetsPath());
}

void CSScript::LoadToMemory()
{
}

void CSScript::UnloadFromMemory()
{
}

bool CSScript::MonoObjectIsValid(MonoObject* object)
{
	if (object != nullptr)
	{
		active_gameobject = created_gameobjects[object];
		return true;
	}
	return false;
}

bool CSScript::MonoComponentIsValid(MonoObject* object)
{
	if (object != nullptr)
	{
		for (MonoComponent* mono_comp : created_components)
		{
			if (mono_comp->component_object == object)
			{
				active_component = mono_comp->component;
				return true;
			}
		}
	}
	return false;
}

MonoObject * CSScript::FindMonoObject(GameObject * go)
{
	MonoObject* ret = nullptr;

	if (go != nullptr)
	{
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); ++it)
		{
			if ((*it).second == go)
			{
				ret = (*it).first;
				break;
			}
		}
	}

	return ret;
}

GameObject * CSScript::FindGameObject(MonoObject * object)
{
	GameObject* ret = nullptr;

	if (object != nullptr)
	{
		ret = created_gameobjects[object];
	}

	return ret;
}

bool CSScript::GameObjectIsValid()
{
	if (active_gameobject == nullptr)
	{
		CONSOLE_ERROR("You are trying to modify a null GameObject");
		return false;
	}
	return true;
}

bool CSScript::IsMuted()
{
	return App->audio->IsMuted();
}

void CSScript::SetMute(bool set)
{
	App->audio->SetMute(set);
}

int CSScript::GetVolume()
{
	return App->audio->GetVolume();
}

void CSScript::SetVolume(int volume)
{
	App->audio->SetVolume(volume);
}

int CSScript::GetPitch()
{
	return App->audio->GetPitch();
}

void CSScript::SetPitch(int pitch)
{
	App->audio->SetPitch(pitch);
}

void CSScript::SetRTPCvalue(MonoString* name, float value)
{
	const char* new_name = mono_string_to_utf8(name);
	App->audio->SetRTPCvalue(new_name, value);
}

bool CSScript::Play(MonoObject * object, MonoString* name)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentAudioSource* as = (ComponentAudioSource*)active_gameobject->GetComponent(Component::CompAudioSource);
	const char* event_name = mono_string_to_utf8(name);
	return as->PlayEvent(event_name);
}

bool CSScript::Stop(MonoObject * object, MonoString* name)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentAudioSource* as = (ComponentAudioSource*)active_gameobject->GetComponent(Component::CompAudioSource);
	const char* event_name = mono_string_to_utf8(name);
	
	return as->StopEvent(event_name);
}

bool CSScript::Send(MonoObject * object, MonoString* name)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	ComponentAudioSource* as = (ComponentAudioSource*)active_gameobject->GetComponent(Component::CompAudioSource);
	const char* event_name = mono_string_to_utf8(name);
	
	return as->SendEvent(event_name);
}

bool CSScript::SetMyRTPCvalue(MonoObject * object, MonoString* name, float value)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	const char* new_name = mono_string_to_utf8(name);
	ComponentAudioSource* as = (ComponentAudioSource*)active_gameobject->GetComponent(Component::CompAudioSource);
	return as->obj->SetRTPCvalue(new_name, value);
}

bool CSScript::SetState(MonoObject* object, MonoString* group, MonoString* state)
{
	if (!MonoObjectIsValid(object))
	{
		return false;
	}

	if (!GameObjectIsValid())
	{
		return false;
	}

	const char* group_name = mono_string_to_utf8(group);
	const char* state_name = mono_string_to_utf8(state);
	ComponentAudioSource* as = (ComponentAudioSource*)active_gameobject->GetComponent(Component::CompAudioSource);
	as->SetState(group_name, state_name);
	return true;
}

void CSScript::PlayEmmiter(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)active_gameobject->GetComponent(Component::CompParticleSystem);

	if(emmiter != nullptr)
		emmiter->PlayEmmiter(); 
}

void CSScript::StopEmmiter(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)active_gameobject->GetComponent(Component::CompParticleSystem);

	if (emmiter != nullptr)
		emmiter->StopEmmiter();
}

void CSScript::SetLinearVelocity(MonoObject * object, float x, float y, float z)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentRigidBody* rb = (ComponentRigidBody*)active_gameobject->GetComponent(Component::CompRigidBody);

	if (rb != nullptr)
		rb->SetLinearVelocity({ x,y,z });
}

void CSScript::SetRBPosition(MonoObject * object, float x, float y, float z)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentRigidBody* rb = (ComponentRigidBody*)active_gameobject->GetComponent(Component::CompRigidBody);

	if (rb != nullptr)
		rb->SetPosition({ x,y,z });
}

void CSScript::SetRBRotation(MonoObject * object, float x, float y, float z)
{
	if (!MonoObjectIsValid(object))
	{
		return;
	}

	if (!GameObjectIsValid())
	{
		return;
	}

	ComponentRigidBody* rb = (ComponentRigidBody*)active_gameobject->GetComponent(Component::CompRigidBody);

	if (rb != nullptr)
		rb->SetRotation({ x,y,z });
}

// ----- GOAP AGENT -----
mono_bool CSScript::GetBlackboardVariableB(MonoString * name) const
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		bool var;
		const char* var_name = mono_string_to_utf8(name);
		if (goap->GetBlackboardVariable(var_name, var))
		{
			return var;
		}
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return false;
}

float CSScript::GetBlackboardVariableF(MonoString * name)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		float var;
		const char* var_name = mono_string_to_utf8(name);
		if (goap->GetBlackboardVariable(var_name, var))
		{
			return var;
		}
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return 0.0f;
}

int CSScript::GetNumGoals()
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		return goap->goals.size();
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return 0;
}

MonoString * CSScript::GetGoalName(int index)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		if (index >= 0 && index < goap->goals.size())
			mono_string_new(mono_domain, goap->goals[index]->GetName().c_str());
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return mono_string_new(mono_domain, "");
}

MonoString * CSScript::GetGoalConditionName(int index)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		if (index >= 0 && index < goap->goals.size())
			mono_string_new(mono_domain, goap->goals[index]->GetConditionName());
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return mono_string_new(mono_domain, "");
}

void CSScript::SetGoalPriority(int index, int priority)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		if (index >= 0 && index < goap->goals.size())
			goap->goals[index]->SetPriority(priority);
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
}

int CSScript::GetGoalPriority(int index)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		if (index >= 0 && index < goap->goals.size())
			return goap->goals[index]->GetPriority();
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
	return -1;
}

void CSScript::CompleteAction()
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		goap->CompleteCurrentAction();
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
}

void CSScript::FailAction()
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		goap->FailCurrentAction();
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
}

void CSScript::SetBlackboardVariable(MonoString * name, float value)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		const char* var_name = mono_string_to_utf8(name);
		goap->SetBlackboardVariable(var_name, value);
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
}

void CSScript::SetBlackboardVariable(MonoString * name, bool value)
{
	ComponentGOAPAgent* goap = (ComponentGOAPAgent*)active_gameobject->GetComponent(Component::CompGOAPAgent);
	if (goap != nullptr)
	{
		const char* var_name = mono_string_to_utf8(name);
		goap->SetBlackboardVariable(var_name, value);
	}
	else
	{
		CONSOLE_WARNING("GOAPAgent not found!");
	}
}
// ------

int CSScript::RandomInt(MonoObject * object)
{
	return App->RandomNumber().Int();
}

float CSScript::RandomFloat(MonoObject * object)
{
	return App->RandomNumber().Float();
}

float CSScript::RandomRange(MonoObject * object, float min, float max)
{
	return App->RandomNumber().FloatIncl(min, max);;
}


void CSScript::LoadScene(MonoString * scene_name)
{
	const char* name = mono_string_to_utf8(scene_name);
	std::vector<std::string> scenes = App->resources->GetSceneList();
	for (std::string scene : scenes)
	{
		std::string scene_name = App->file_system->GetFileNameWithoutExtension(scene);
		if (scene_name == name)
		{
			App->scene->LoadScene(LIBRARY_SCENES_FOLDER + scene_name + ".scene");
			return;
		}
	}

	CONSOLE_ERROR("LoadScene: Scene %s does not exist", name);
}

void CSScript::Quit()
{
	if (App->IsGame())
	{
		App->quit = true;
	}
	else
	{
		if (!App->IsStopped())
		{
			App->Stop();
		}
	}
}

void CSScript::SetBoolField(MonoObject * object, MonoString * field_name, bool value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			script->SetBoolProperty(name, value);
		}
	}
}

bool CSScript::GetBoolField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			return script->GetBoolProperty(name);
		}
	}
}

void CSScript::SetIntField(MonoObject * object, MonoString * field_name, int value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			script->SetIntProperty(name, value);
		}
	}
}

int CSScript::GetIntField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			return script->GetIntProperty(name);
		}
	}
}

void CSScript::SetFloatField(MonoObject * object, MonoString * field_name, float value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			script->SetFloatProperty(name, value);
		}
	}
}

float CSScript::GetFloatField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			return script->GetFloatProperty(name);
		}
	}
}

void CSScript::SetDoubleField(MonoObject * object, MonoString * field_name, double value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			script->SetDoubleProperty(name, value);
		}
	}
}

double CSScript::GetDoubleField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			return script->GetDoubleProperty(name);
		}
	}
}

void CSScript::SetStringField(MonoObject * object, MonoString * field_name, MonoString * value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		const char* value_to_string = mono_string_to_utf8(value);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			script->SetStringProperty(name, value_to_string);
		}
	}
}

MonoString * CSScript::GetStringField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return nullptr;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			std::string s = script->GetStringProperty(name);
			MonoString* ms = mono_string_new(mono_domain, s.c_str());
			return ms;
		}
	}
	return nullptr;
}

void CSScript::SetGameObjectField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			GameObject* go_to_set = nullptr;
			for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
			{
				if (it->first == value)
				{
					go_to_set = it->second;
					break;
				}
			}
			script->SetGameObjectProperty(name, go_to_set);
		}
	}
}

MonoObject * CSScript::GetGameObjectField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			GameObject* go = script->GetGameObjectProperty(name);
			MonoObject* mono_object = FindMonoObject(go);
			if (mono_object)
			{
				return mono_object;
			}
			else
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(mono_domain, c);
					if (new_object)
					{
						created_gameobjects[new_object] = go;
						return new_object;
					}
				}
			}
		}
	}
	return nullptr;
}

void CSScript::SetVector3Field(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			MonoClass* c_pos = mono_object_get_class(value);
			MonoClassField* x_field = mono_class_get_field_from_name(c_pos, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c_pos, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c_pos, "z");

			float3 vector;
			if (x_field) mono_field_get_value(value, x_field, &vector.x);
			if (y_field) mono_field_get_value(value, y_field, &vector.y);
			if (z_field) mono_field_get_value(value, z_field, &vector.z);

			script->SetVec3Property(name, vector);
		}
	}
}

MonoObject * CSScript::GetVector3Field(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			float3 vector3 = script->GetVec3Property(name);
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
			if (c)
			{
				MonoObject* new_object = mono_object_new(mono_domain, c);
				if (new_object)
				{
					MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
					MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
					MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

					if (x_field) mono_field_set_value(new_object, x_field, &vector3.x);
					if (y_field) mono_field_set_value(new_object, y_field, &vector3.y);
					if (z_field) mono_field_set_value(new_object, z_field, &vector3.z);

					return new_object;
				}
			}
		}
	}
	return nullptr;
}

void CSScript::SetQuaternionField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			MonoClass* c_pos = mono_object_get_class(value);
			MonoClassField* x_field = mono_class_get_field_from_name(c_pos, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c_pos, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c_pos, "z");
			MonoClassField* w_field = mono_class_get_field_from_name(c_pos, "w");

			float4 quat;
			if (x_field) mono_field_get_value(value, x_field, &quat.x);
			if (y_field) mono_field_get_value(value, y_field, &quat.y);
			if (z_field) mono_field_get_value(value, z_field, &quat.z);
			if (w_field) mono_field_get_value(value, w_field, &quat.w);

			script->SetVec4Property(name, quat);
		}
	}
}

MonoObject * CSScript::GetQuaternionField(MonoObject * object, MonoString * field_name)
{
	if (!MonoComponentIsValid(object))
	{
		return false;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(field_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			float4 quat = script->GetVec4Property(name);
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheQuaternion");
			if (c)
			{
				MonoObject* new_object = mono_object_new(mono_domain, c);
				if (new_object)
				{
					MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
					MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
					MonoClassField* z_field = mono_class_get_field_from_name(c, "z");
					MonoClassField* w_field = mono_class_get_field_from_name(c, "w");

					if (x_field) mono_field_set_value(new_object, x_field, &quat.x);
					if (y_field) mono_field_set_value(new_object, y_field, &quat.y);
					if (z_field) mono_field_set_value(new_object, z_field, &quat.z);
					if (w_field) mono_field_set_value(new_object, w_field, &quat.w);

					return new_object;
				}
			}
		}
	}
	return nullptr;
}

void CSScript::CallFunction(MonoObject * object, MonoString * function_name)
{
	if (!MonoComponentIsValid(object))
	{
		return;
	}

	if (active_component && active_component->GetType() == Component::CompScript)
	{
		ComponentScript* comp_script = (ComponentScript*)active_component;
		const char* name = mono_string_to_utf8(function_name);
		CSScript* script = (CSScript*)comp_script->GetScript();
		if (script)
		{
			MonoMethod* method = script->GetFunction(name, 0);
			if (method)
			{
				App->script_importer->SetCurrentScript(script);
				script->CallFunction(method, nullptr);
			}
			else
			{
				CONSOLE_ERROR("Function %s in script %s does not exist", name, script->GetName().c_str());
			}
		}
	}
}

Component::ComponentType CSScript::CsToCppComponent(std::string component_type)
{
	Component::ComponentType type;

	if (component_type == "TheTransform")
	{
		type = Component::CompTransform;
	}
	else if (component_type == "TheFactory")
	{
		type = Component::CompFactory;
	}
	else if (component_type == "TheRectTransform")
	{
		type = Component::CompRectTransform;
	}
	else if (component_type == "TheProgressBar")
	{
		type = Component::CompProgressBar;
	}
	else if (component_type == "TheAudioSource")
	{
		type = Component::CompAudioSource;
	}
	else if (component_type == "TheParticleEmmiter")
	{
		type = Component::CompParticleSystem;
	}
	else if (component_type == "TheText")
	{
		type = Component::CompText;
	}
	else if (component_type == "TheRigidBody")
	{
		type = Component::CompRigidBody;
	}
	else if (component_type == "TheGOAPAgent")
	{
		type = Component::CompGOAPAgent;
	}
	else if (component_type == "TheScript")
	{
		type = Component::CompScript;
	}
	else
	{
		type = Component::CompUnknown;
	}
	return type;
}