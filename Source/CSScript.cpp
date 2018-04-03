#include "CSScript.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"
#include "ModuleInput.h"
#include "ModuleScriptImporter.h"
#include "ModuleScene.h"
#include "ModuleResources.h"
#include "Data.h"
#include <ctime>
#include <mono/metadata/reflection.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/exception.h>
#include "ComponentScript.h"
#include "ComponentAudioSource.h"
#include "ComponentRigidBody.h"
#include "ComponentGOAPAgent.h"
#include "ComponentFactory.h"
#include "ComponentTransform.h"
#include "ComponentRectTransform.h"
#include "ComponentText.h"
#include "ComponentProgressBar.h"
#include "ComponentRadar.h"
#include "GOAPGoal.h"
#include "Prefab.h"

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

	//active_gameobject = nullptr;
	attached_gameobject = nullptr;
	inside_function = false;

	SetType(Resource::ScriptResource);
	SetScriptType(ScriptType::CsScript);
	SetName("No Script");
}

CSScript::~CSScript()
{
	/*for (MonoComponent* mono_comp : created_components)
	{
	RELEASE(mono_comp);
	}*/
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
		on_trigger_enter = GetFunction("OnTriggerEnter", 1);
		on_trigger_stay = GetFunction("OnTriggerStay", 1);
		on_trigger_exit = GetFunction("OnTriggerExit", 1);
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
	AddFieldsToMonoObjectList();

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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		std::map<MonoObject*, GameObject*> objects = App->script_importer->ns_importer->created_gameobjects;
		for (std::map<MonoObject*, GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == other_collider)
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
					App->script_importer->ns_importer->created_gameobjects[new_object] = other_collider;
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
		void* params = value;
		mono_field_set_value(mono_object, field, params);
		if (App->IsPlaying())
		{
			MonoObject* object = mono_field_get_value_object(mono_domain, field, mono_object);
			if (object && value)
			{
				App->script_importer->ns_importer->created_gameobjects[object] = value;
			}
		}
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

void CSScript::AddFieldsToMonoObjectList()
{
	std::vector<ScriptField*> script_fields = GetScriptFields();

	for (std::vector<ScriptField*>::iterator it = script_fields.begin(); it != script_fields.end(); it++)
	{
		if ((*it)->propertyType == ScriptField::GameObject)
		{
			GameObject* gameobject = GetGameObjectProperty((*it)->fieldName.c_str());
			MonoClassField* field = mono_class_get_field_from_name(mono_class, (*it)->fieldName.c_str());
			MonoObject* object = mono_field_get_value_object(mono_domain, field, mono_object);
			if (object && gameobject)
			{
				App->script_importer->ns_importer->created_gameobjects[object] = gameobject;
			}
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
		if (name == "TheEngine.TheGameObject") script_field.propertyType = ScriptField::GameObject;
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
			App->script_importer->ns_importer->AddCreatedGameObjectToList(mono_self_object, attached_gameobject);
		}
	}
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
		on_trigger_enter = text->on_trigger_enter;
		on_trigger_stay = text->on_trigger_stay;
		on_trigger_exit = text->on_trigger_exit;
		on_enable = text->on_enable;
		on_disable = text->on_disable;
		on_complete = text->on_complete;
		on_fail = text->on_fail;

		int fields_count = data.GetInt("int_fields_count");
		if (data.EnterSection("int_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				int field_value = data.GetInt("field_value");
				data.LeaveSection();
				SetIntProperty(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("double_fields_count");
		if (data.EnterSection("double_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				double field_value = data.GetDouble("field_value");
				data.LeaveSection();
				SetDoubleProperty(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("float_fields_count");
		if (data.EnterSection("float_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				float field_value = data.GetFloat("field_value");
				data.LeaveSection();
				SetFloatProperty(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("bool_fields_count");
		if (data.EnterSection("bool_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				bool field_value = data.GetBool("field_value");
				data.LeaveSection();
				SetBoolProperty(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("string_fields_count");
		if (data.EnterSection("string_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				std::string field_value = data.GetString("field_value");
				data.LeaveSection();
				SetStringProperty(field_name.c_str(), field_value.c_str());
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("gameobject_fields_count");
		if (data.EnterSection("gameobject_fields"))
		{
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
		}

		fields_count = data.GetInt("vec2_fields_count");
		if (data.EnterSection("vec2_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				float2 field_value = data.GetVector2("field_value");
				data.LeaveSection();
				SetVec2Property(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("vec3_fields_count");
		if (data.EnterSection("vec3_fields"))
		{
			for (int i = 0; i < fields_count; i++)
			{
				data.EnterSection("field_" + std::to_string(i));
				std::string field_name = data.GetString("field_name");
				float3 field_value = data.GetVector3("field_value");
				data.LeaveSection();
				SetVec3Property(field_name.c_str(), field_value);
			}
			data.LeaveSection();
		}

		fields_count = data.GetInt("vec4_fields_count");
		if (data.EnterSection("vec4_fields"))
		{
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

void CSScript::SetGameObjectName(MonoObject * object, MonoString* name)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go && name)
	{
		const char* new_name = mono_string_to_utf8(name);
		go->SetName(new_name);
		App->scene->RenameDuplicatedGameObject(go);
	}
}

MonoString* CSScript::GetGameObjectName(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetName().c_str());
}

void CSScript::SetGameObjectTag(MonoObject * object, MonoString * tag)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go && tag)
	{
		const char* new_tag = mono_string_to_utf8(tag);
		go->SetName(new_tag);
	}
}

MonoString * CSScript::GetGameObjectTag(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetTag().c_str());
}

void CSScript::SetGameObjectLayer(MonoObject * object, MonoString * layer)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (layer != nullptr)
	{
		const char* new_layer = mono_string_to_utf8(layer);
		go->SetLayer(new_layer);
	}
}

MonoString * CSScript::GetGameObjectLayer(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetLayer().c_str());
}

void CSScript::SetGameObjectStatic(MonoObject * object, mono_bool value)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetStatic(value);
	}
}

mono_bool CSScript::GameObjectIsStatic(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->IsStatic();
	}
	return false;
}

MonoObject * CSScript::DuplicateGameObject(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);
	if (!go)
	{
		CONSOLE_ERROR("Trying to instantiate a null object");
		return nullptr;
	}

	GameObject* duplicated = App->scene->DuplicateGameObject(go);
	if (duplicated)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				App->script_importer->ns_importer->created_gameobjects[new_object] = duplicated;
				return new_object;
			}
		}
	}

	return nullptr;
}

void CSScript::SetGameObjectParent(MonoObject * object, MonoObject * parent)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetParent(App->script_importer->ns_importer->GetGameObjectFromMonoObject(object));
	}
}

MonoObject* CSScript::GetGameObjectParent(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		GameObject* parent = go->GetParent();
		if (parent)
		{
			MonoObject* parent_mono_object = App->script_importer->ns_importer->GetMonoObjectFromGameObject(parent);
			if (!parent_mono_object)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						App->script_importer->ns_importer->created_gameobjects[new_object] = parent;
						return new_object;
					}
				}
			}
			return parent_mono_object;
		}
	}

	return nullptr;
}

MonoObject * CSScript::GetGameObjectChild(MonoObject * object, int index)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		if (index >= 0 && index < go->childs.size())
		{
			std::list<GameObject*>::iterator it = std::next(go->childs.begin(), index);
			if (*it)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						App->script_importer->ns_importer->created_gameobjects[new_object] = *it;
						return new_object;
					}
				}
			}
		}
		else
		{
			CONSOLE_ERROR("GetChild: Index out of bounds");
		}

	}

	return nullptr;
}

MonoObject * CSScript::GetGameObjectChildString(MonoObject * object, MonoString * name)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	const char* s_name = mono_string_to_utf8(name);

	if (go)
	{
		for (std::list<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); it++)
		{
			if (*it != nullptr && (*it)->GetName() == s_name)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						App->script_importer->ns_importer->created_gameobjects[new_object] = *it;
						return new_object;
					}
				}
			}
		}
	}


	return nullptr;
}

int CSScript::GetGameObjectChildCount(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->childs.size();
	}

	return -1;
}

MonoObject * CSScript::FindGameObject(MonoString * gameobject_name)
{
	const char* s_name = mono_string_to_utf8(gameobject_name);

	GameObject* go = App->scene->FindGameObjectByName(s_name);

	if (go)
	{
		for (std::map<MonoObject*, GameObject*>::iterator it = App->script_importer->ns_importer->created_gameobjects.begin(); it != App->script_importer->ns_importer->created_gameobjects.end(); it++)
		{
			if (it->second == go) return it->first;
		}

		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				App->script_importer->ns_importer->created_gameobjects[new_object] = go;
				return new_object;
			}
		}
	}
	else
	{
		CONSOLE_WARNING("Find: Cannot find gameobject %s", s_name);
	}

	return nullptr;
}

MonoArray * CSScript::GetSceneGameObjects(MonoObject * object)
{
	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
	std::list<GameObject*> objects = App->scene->scene_gameobjects;

	if (c)
	{
		MonoArray* scene_objects = mono_array_new(App->script_importer->GetDomain(), c, objects.size());
		if (scene_objects)
		{
			int index = 0;
			for (GameObject* go : objects)
			{
				bool exist = false;
				for (std::map<MonoObject*, GameObject*>::iterator it = App->script_importer->ns_importer->created_gameobjects.begin(); it != App->script_importer->ns_importer->created_gameobjects.end(); ++it)
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
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						mono_array_set(scene_objects, MonoObject*, index, new_object);
						index++;
						App->script_importer->ns_importer->created_gameobjects[new_object] = go;
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
		MonoArray* scene_objects = mono_array_new(App->script_importer->GetDomain(), c, obj_inFrustum.size());
		if (scene_objects)
		{
			int index = 0;
			for (GameObject* go : obj_inFrustum)
			{
				bool exist = false;
				for (std::map<MonoObject*, GameObject*>::iterator it = App->script_importer->ns_importer->created_gameobjects.begin(); it != App->script_importer->ns_importer->created_gameobjects.end(); it++)
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
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						mono_array_set(scene_objects, MonoObject*, index, new_object);
						index++;
						App->script_importer->ns_importer->created_gameobjects[new_object] = go;
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
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		std::vector<GameObject*> objects;

		go->GetAllChilds(objects);

		if (c)
		{
			MonoArray* scene_objects = mono_array_new(App->script_importer->GetDomain(), c, objects.size());
			if (scene_objects)
			{
				int index = 0;
				for (GameObject* go : objects)
				{
					bool exist = false;
					for (std::map<MonoObject*, GameObject*>::iterator it = App->script_importer->ns_importer->created_gameobjects.begin(); it != App->script_importer->ns_importer->created_gameobjects.end(); it++)
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
						MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
						if (new_object)
						{
							mono_array_set(scene_objects, MonoObject*, index, new_object);
							index++;
							App->script_importer->ns_importer->created_gameobjects[new_object] = go;
						}
					}
				}
				return scene_objects;
			}
		}
	}

	return nullptr;
}

void CSScript::SetComponentActive(MonoObject * object, bool active)
{
	Component* cmp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);
	cmp->SetActive(active);
}

MonoObject* CSScript::AddComponent(MonoObject * object, MonoReflectionType * type)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		MonoType* t = mono_reflection_type_get_type(type);
		std::string name = mono_type_get_name(t);

		const char* comp_name = "";
		Component::ComponentType comp_type = Component::CompUnknown;

		if (name == "TheEngine.TheTransform")
		{
			CONSOLE_ERROR("Can't add Transform component to %s. GameObjects cannot have more than 1 transform.", go->GetName().c_str());
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
			Component* comp = go->AddComponent(comp_type);

			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", comp_name);
			if (c)
			{
				MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					App->script_importer->ns_importer->created_components[new_object] = comp;
					return new_object;
				}
			}
		}
	}

	return nullptr;
}

MonoObject* CSScript::GetComponent(MonoObject * object, MonoReflectionType * type, int index)
{

	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
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
		else if (name == "TheEngine.TheMeshCollider")
		{
			comp_name = "TheMeshCollider";
		}
		else if (name == "TheEngine.TheGOAPAgent")
		{
			comp_name = "TheGOAPAgent";
		}
		else if (name == "TheEngine.TheScript")
		{
			comp_name = "TheScript";
		}

		Component::ComponentType cpp_type = App->script_importer->ns_importer->CsToCppComponent(comp_name);

		int temp_index = index;
		if (cpp_type != Component::CompUnknown)
		{
			int comp_type_count = 0;
			for (Component* comp : go->components_list)
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
					CONSOLE_ERROR("GetComponent method: %s does not exist in %s", comp_name, go->GetName().c_str());
				}
				else
				{
					CONSOLE_ERROR("GetComponent method: %s index is out of bounds", comp_name);
				}
				return nullptr;
			}

			for (Component* comp : go->components_list)
			{
				if (comp->GetType() == cpp_type)
				{
					if (temp_index == 0)
					{
						for (std::map<MonoObject*, Component*>::iterator it = App->script_importer->ns_importer->created_components.begin(); it != App->script_importer->ns_importer->created_components.end(); it++)
						{
							if (comp == it->second)
							{
								return it->first;
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
				MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
				if (new_object)
				{
					for (Component* comp : go->components_list)
					{
						if (comp->GetType() == cpp_type)
						{
							if (temp_index == 0)
							{
								App->script_importer->ns_importer->created_components[new_object] = comp;
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
	}

	return nullptr;
}

void CSScript::DestroyComponent(MonoObject* object, MonoObject* cmp)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(cmp);
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (comp != nullptr && go != nullptr)
	{
		go->DestroyComponent(comp);
	}
}

void CSScript::SetPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		transform->SetPosition(new_pos);
	}
}

MonoObject* CSScript::GetPosition(MonoObject * object, mono_bool is_global)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
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
	}
	return nullptr;
}

void CSScript::SetRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		transform->SetRotation(new_rot);
	}
}

MonoObject * CSScript::GetRotation(MonoObject * object, mono_bool is_global)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
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
	}
	return nullptr;
}

void CSScript::SetScale(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_scale;

		if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		transform->SetScale(new_scale);
	}
}

MonoObject * CSScript::GetScale(MonoObject * object, mono_bool is_global)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
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
	}
	return nullptr;
}

void CSScript::LookAt(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 target_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &target_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &target_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &target_pos.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		float3 direction = target_pos - transform->GetGlobalPosition();
		transform->LookAt(direction.Normalized(), float3::unitY);
	}
}

void CSScript::SetRectPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		ComponentRectTransform* rect_transform = (ComponentRectTransform*)comp;
		rect_transform->SetPos(float2(new_pos.x, new_pos.y));
	}
}

MonoObject * CSScript::GetRectPosition(MonoObject * object)
{
	return nullptr;
}

MonoObject * CSScript::GetForward(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
				float3 forward = transform->GetForward();

				if (x_field) mono_field_set_value(new_object, x_field, &forward.x);
				if (y_field) mono_field_set_value(new_object, y_field, &forward.y);
				if (z_field) mono_field_set_value(new_object, z_field, &forward.z);

				return new_object;
			}
		}
	}
	return nullptr;
}

void CSScript::SetRectRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		ComponentRectTransform* rect_transform = (ComponentRectTransform*)comp;
		rect_transform->SetRotation(new_rot);
	}
}

MonoObject * CSScript::GetRectRotation(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentRectTransform* rect_transform = (ComponentRectTransform*)comp;
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
	}
	return nullptr;
}

void CSScript::SetRectSize(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_size;

		if (x_field) mono_field_get_value(vector3, x_field, &new_size.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_size.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_size.z);

		ComponentRectTransform* rect_transform = (ComponentRectTransform*)comp;
		rect_transform->SetSize(float2(new_size.x, new_size.y));
	}
}

MonoObject * CSScript::GetRectSize(MonoObject * object)
{
	return nullptr;
}

MonoObject * CSScript::GetRight(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
				float3 right = transform->GetRight();

				if (x_field) mono_field_set_value(new_object, x_field, &right.x);
				if (y_field) mono_field_set_value(new_object, y_field, &right.y);
				if (z_field) mono_field_set_value(new_object, z_field, &right.z);

				return new_object;
			}
		}
	}
	return nullptr;
}

void CSScript::SetRectAnchor(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_anchor;

		if (x_field) mono_field_get_value(vector3, x_field, &new_anchor.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_anchor.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_anchor.z);

		ComponentRectTransform* rect_transform = (ComponentRectTransform*)comp;
		rect_transform->SetAnchor(float2(new_anchor.x, new_anchor.y));
	}
}

MonoObject * CSScript::GetRectAnchor(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheRectTransform");

		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
				float3 right = transform->GetRight();

				if (x_field) mono_field_set_value(new_object, x_field, &right.x);
				if (y_field) mono_field_set_value(new_object, y_field, &right.y);
				if (z_field) mono_field_set_value(new_object, z_field, &right.z);

				return new_object;
			}
		}
	}
	return nullptr;
}

mono_bool CSScript::GetOnClick(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnClick();
		}
	}
	return false;
}

mono_bool CSScript::GetOnClickDown(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnClickDown();
		}
	}
	return false;
}

mono_bool CSScript::GetOnClickUp(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnClickUp();
		}
	}
	return false;
}

mono_bool CSScript::GetOnMouseEnter(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnMouseEnter();
		}
	}
	return false;
}

mono_bool CSScript::GetOnMouseOver(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnMouseOver();
		}
	}
	return false;
}

mono_bool CSScript::GetOnMouseOut(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRectTransform* rect_trans = (ComponentRectTransform*)comp;

		if (rect_trans != nullptr)
		{
			return rect_trans->GetOnMouseOut();
		}
	}
	return false;
}

void CSScript::SetText(MonoObject * object, MonoString* t)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* txt = mono_string_to_utf8(t);

		ComponentText* text = (ComponentText*)comp;
		if (text)
			text->SetText(txt);
	}
}

MonoString* CSScript::GetText(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentText* text = (ComponentText*)comp;
		return mono_string_new(App->script_importer->GetDomain(), text->GetText().c_str());
	}
	return mono_string_new(App->script_importer->GetDomain(), "");
}

MonoObject * CSScript::GetUp(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

				ComponentTransform* transform = (ComponentTransform*)comp;
				float3 up = transform->GetUp();

				if (x_field) mono_field_set_value(new_object, x_field, &up.x);
				if (y_field) mono_field_set_value(new_object, y_field, &up.y);
				if (z_field) mono_field_set_value(new_object, z_field, &up.z);

				return new_object;
			}
		}
	}
	return nullptr;
}

void CSScript::RotateAroundAxis(MonoObject * object, MonoObject * axis, float angle)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(axis);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 rot_axis;

		if (x_field) mono_field_get_value(axis, x_field, &rot_axis.x);
		if (y_field) mono_field_get_value(axis, y_field, &rot_axis.y);
		if (z_field) mono_field_get_value(axis, z_field, &rot_axis.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		transform->RotateAroundAxis(rot_axis, angle);
	}
}

void CSScript::SetIncrementalRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		ComponentTransform* transform = (ComponentTransform*)comp;
		transform->SetIncrementalRotation(new_rot);
	}
}

void CSScript::SetPercentageProgress(MonoObject * object, float progress)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentProgressBar* progres_barr = (ComponentProgressBar*)comp;

		progres_barr->SetProgressPercentage(progress);
	}
}

float CSScript::GetPercentageProgress(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentProgressBar* progres_barr = (ComponentProgressBar*)comp;
		return progres_barr->GetProgressPercentage();
	}
	return 0.0f;
}

void CSScript::AddEntity(MonoObject * object, MonoObject * game_object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(game_object);

		if (radar != nullptr)
		{
			if (go != nullptr)
			{
				radar->AddEntity(go);
			}
		}
	}
}

void CSScript::RemoveEntity(MonoObject * object, MonoObject * game_object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(game_object);

		if (radar != nullptr)
		{
			if (go != nullptr)
			{
				radar->RemoveEntity(go);
			}
		}
	}
}

void CSScript::RemoveAllEntities(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		if (radar != nullptr)
		{
			radar->RemoveAllEntities();
		}
	}
}

void CSScript::SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString * marker_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(game_object);

		if (radar != nullptr)
		{
			if (go != nullptr)
			{
				const char* txt = mono_string_to_utf8(marker_name);

				radar->AddMarkerToEntity(go, radar->GetMarker(txt));
			}
		}
	}
}

void CSScript::StartFactory(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentFactory* factory = (ComponentFactory*)comp;
		if (factory) factory->StartFactory();
	}
}

MonoObject * CSScript::Spawn(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentFactory* factory = (ComponentFactory*)comp;
		GameObject* go = nullptr;
		if (factory)
		{
			go = factory->Spawn();
		}

		if (go)
		{
			MonoObject* obj = App->script_importer->ns_importer->GetMonoObjectFromGameObject(go);
			if (!obj)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						App->script_importer->ns_importer->created_gameobjects[new_object] = go;
						return new_object;
					}
				}
			}
			return obj;
		}
	}
	return nullptr;
}

void CSScript::SetSpawnPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		ComponentFactory* factory = (ComponentFactory*)comp;
		if (factory) factory->SetSpawnPos(new_pos);
	}
}

void CSScript::SetSpawnRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		ComponentFactory* factory = (ComponentFactory*)comp;
		if (factory) factory->SetSpawnRotation(new_rot);
	}
}

void CSScript::SetSpawnScale(MonoObject * object, MonoObject * vector3)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		MonoClass* c = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

		float3 new_scale;

		if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

		ComponentFactory* factory = (ComponentFactory*)comp;
		if (factory) factory->SetSpawnScale(new_scale);
	}
}

void CSScript::AddString(MonoString * name, MonoString * string)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);
		const char* c_string = mono_string_to_utf8(string);

		json->SetString(c_name, c_string);

		json->Save();
	}
}

void CSScript::AddInt(MonoString * name, int value)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);

		json->SetNumber(c_name, value);

		json->Save();
	}
}

void CSScript::AddFloat(MonoString * name, float value)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);

		json->SetNumber(c_name, value);

		json->Save();
	}
}

MonoString* CSScript::GetString(MonoString* name)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);

		const char* ret = json->GetString(c_name, "no_str");

		return mono_string_new(App->script_importer->GetDomain(), ret);
	}

	return mono_string_new(App->script_importer->GetDomain(), "no_str");
}

int CSScript::GetInt(MonoString * name)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);

		int ret = json->GetNumber(c_name, 0);

		return ret;
	}

	return 0;
}

float CSScript::GetFloat(MonoString * name)
{
	JSON_File* json = App->scene->GetJSONTool()->LoadJSON(LIBRARY_GAME_DATA);
	if (json == nullptr)
		json = App->scene->GetJSONTool()->CreateJSON(LIBRARY_GAME_DATA);

	if (json != nullptr)
	{
		const char* c_name = mono_string_to_utf8(name);

		int ret = json->GetNumber(c_name, 0.0f);

		return ret;
	}

	return 0;
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
		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), quaternion);
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

MonoObject * CSScript::ToEulerAngles(MonoObject * object)
{
	if (!object)
	{
		return nullptr;
	}

	MonoClass* c = mono_object_get_class(object);
	MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
	MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
	MonoClassField* z_field = mono_class_get_field_from_name(c, "z");
	MonoClassField* w_field = mono_class_get_field_from_name(c, "w");

	Quat rotation;

	if (x_field) mono_field_get_value(object, x_field, &rotation.x);
	if (y_field) mono_field_get_value(object, y_field, &rotation.y);
	if (z_field) mono_field_get_value(object, z_field, &rotation.z);
	if (w_field) mono_field_get_value(object, w_field, &rotation.w);

	math::float3 euler = rotation.ToEulerXYZ() * RADTODEG;

	MonoClass* vector = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
	if (vector)
	{
		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), vector);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(vector, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(vector, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(vector, "z");

			if (x_field) mono_field_set_value(new_object, x_field, &euler.x);
			if (y_field) mono_field_set_value(new_object, y_field, &euler.y);
			if (z_field) mono_field_set_value(new_object, z_field, &euler.z);

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

float CSScript::GetTimeSinceStart()
{
	return App->time->GetPlayTime();
}


void CSScript::Start(MonoObject * object, float time)
{
}

float CSScript::ReadTime(MonoObject * object)
{
	return 0.0f;
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
		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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
	return App->input->GetControllerJoystickMove(pad, code);
}

int CSScript::GetControllerButton(int pad, MonoString * button)
{
	const char* key = mono_string_to_utf8(button);
	SDL_Keycode code = App->input->StringToKey(key);
	return App->input->GetControllerButton(pad, code);
}

void CSScript::RumbleController(int pad, float strength, int ms)
{
	App->input->RumbleController(pad, strength, ms);
}

void CSScript::CreateGameObject(MonoObject * object)
{
	/*if (!inside_function)
	{
	CONSOLE_ERROR("Can't create new GameObjects outside a function.");
	return;
	}*/
	GameObject* gameobject = App->scene->CreateGameObject();
	App->script_importer->ns_importer->created_gameobjects[object] = gameobject;
}

MonoObject* CSScript::GetSelfGameObject()
{
	return mono_self_object;
}

void CSScript::SetGameObjectActive(MonoObject * object, mono_bool active)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetActive(active);
	}
}

mono_bool CSScript::GetGameObjectIsActive(MonoObject * object)
{
	GameObject* go = App->script_importer->ns_importer->GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->IsActive();
	}
	return false;
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

void CSScript::SetVolume(MonoObject* obj, int volume)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(obj);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		as->volume = volume;
	}
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
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);
		return as->PlayEvent(event_name);
	}
	return false;
}

bool CSScript::Stop(MonoObject * object, MonoString* name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);

		return as->StopEvent(event_name);
	}
	return false;
}

bool CSScript::Send(MonoObject * object, MonoString* name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);

		return as->SendEvent(event_name);
	}
	return false;
}

bool CSScript::SetMyRTPCvalue(MonoObject * object, MonoString* name, float value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* new_name = mono_string_to_utf8(name);
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		return as->obj->SetRTPCvalue(new_name, value);
	}
	return false;
}

bool CSScript::SetState(MonoObject* object, MonoString* group, MonoString* state)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* group_name = mono_string_to_utf8(group);
		const char* state_name = mono_string_to_utf8(state);
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		as->SetState(group_name, state_name);
	}
	return true;
}

void CSScript::PlayEmmiter(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)comp;

		if (emmiter != nullptr)
			emmiter->PlayEmmiter();
	}
}

void CSScript::StopEmmiter(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)comp;

		if (emmiter != nullptr)
			emmiter->StopEmmiter();
	}
}

void CSScript::SetLinearVelocity(MonoObject * object, float x, float y, float z)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetLinearVelocity({ x,y,z });
	}
}

void CSScript::SetAngularVelocity(MonoObject * object, float x, float y, float z)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetAngularVelocity({ x,y,z });
	}
}

void CSScript::AddTorque(MonoObject* object, float x, float y, float z, int force_type)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
		{
			float3 force = { x,y,z };
			rb->AddTorque(force, force_type);
		}
	}
}

void CSScript::DisableCollider(MonoObject * object, int index)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->DisableShapeByIndex(index);
	}
}

void CSScript::DisableAllColliders(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->DisableShapes();
	}
}

void CSScript::EnableCollider(MonoObject * object, int index)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->EnableShapeByIndex(index);
	}
}

void CSScript::EnableAllColliders(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->EnableShapes();
	}
}

void CSScript::SetKinematic(MonoObject * object, bool kinematic)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetKinematic(kinematic);
	}
}

void CSScript::SetTransformGO(MonoObject * object, bool transform_go)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetTransformsGo(transform_go);
	}
}

bool CSScript::IsKinematic(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			return rb->IsKinematic();
	}

	return false;
}

bool CSScript::IsTransformGO(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			return rb->GetTransformsGo();
	}

	return false;
}

void CSScript::SetRBPosition(MonoObject * object, float x, float y, float z)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetPosition({ x,y,z });
	}
}

void CSScript::SetRBRotation(MonoObject * object, float x, float y, float z)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetRotation({ x,y,z });
	}
}

// ----- GOAP AGENT -----
mono_bool CSScript::GetBlackboardVariableB(MonoObject * object, MonoString * name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
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
	}
	return false;
}

float CSScript::GetBlackboardVariableF(MonoObject * object, MonoString * name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
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
	}
	return 0.0f;
}

int CSScript::GetNumGoals(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			return goap->goals.size();
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
	return 0;
}

MonoString * CSScript::GetGoalName(MonoObject * object, int index)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			if (index >= 0 && index < goap->goals.size())
				mono_string_new(App->script_importer->GetDomain(), goap->goals[index]->GetName().c_str());
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
	return mono_string_new(App->script_importer->GetDomain(), "");
}

MonoString * CSScript::GetGoalConditionName(MonoObject * object, int index)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			if (index >= 0 && index < goap->goals.size())
				mono_string_new(App->script_importer->GetDomain(), goap->goals[index]->GetConditionName());
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
	return mono_string_new(App->script_importer->GetDomain(), "");
}

void CSScript::SetGoalPriority(MonoObject * object, int index, int priority)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
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
}

int CSScript::GetGoalPriority(MonoObject * object, int index)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			if (index >= 0 && index < goap->goals.size())
				return goap->goals[index]->GetPriority();
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
	return -1;
}

void CSScript::CompleteAction(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			goap->CompleteCurrentAction();
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
}

void CSScript::FailAction(MonoObject * object)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
		if (goap != nullptr)
		{
			goap->FailCurrentAction();
		}
		else
		{
			CONSOLE_WARNING("GOAPAgent not found!");
		}
	}
}

void CSScript::SetBlackboardVariable(MonoObject * object, MonoString * name, float value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
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
}

void CSScript::SetBlackboardVariable(MonoObject * object, MonoString * name, bool value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentGOAPAgent* goap = (ComponentGOAPAgent*)comp;
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
			App->scene->LoadScene(LIBRARY_SCENES_FOLDER + scene_name + ".jscene");
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
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				script->SetBoolProperty(name, value);
			}
		}
	}
}

bool CSScript::GetBoolField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				return script->GetBoolProperty(name);
			}
		}
	}
}

void CSScript::SetIntField(MonoObject * object, MonoString * field_name, int value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				script->SetIntProperty(name, value);
			}
		}
	}
}

int CSScript::GetIntField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				return script->GetIntProperty(name);
			}
		}
	}
}

void CSScript::SetFloatField(MonoObject * object, MonoString * field_name, float value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				script->SetFloatProperty(name, value);
			}
		}
	}
}

float CSScript::GetFloatField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				return script->GetFloatProperty(name);
			}
		}
	}
}

void CSScript::SetDoubleField(MonoObject * object, MonoString * field_name, double value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				script->SetDoubleProperty(name, value);
			}
		}
	}
}

double CSScript::GetDoubleField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				return script->GetDoubleProperty(name);
			}
		}
	}
}

void CSScript::SetStringField(MonoObject * object, MonoString * field_name, MonoString * value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			const char* value_to_string = mono_string_to_utf8(value);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				script->SetStringProperty(name, value_to_string);
			}
		}
	}
}

MonoString * CSScript::GetStringField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				std::string s = script->GetStringProperty(name);
				MonoString* ms = mono_string_new(App->script_importer->GetDomain(), s.c_str());
				return ms;
			}
		}
	}
	return nullptr;
}

void CSScript::SetGameObjectField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				GameObject* go_to_set = nullptr;
				for (std::map<MonoObject*, GameObject*>::iterator it = App->script_importer->ns_importer->created_gameobjects.begin(); it != App->script_importer->ns_importer->created_gameobjects.end(); it++)
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
}

MonoObject * CSScript::GetGameObjectField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				GameObject* go = script->GetGameObjectProperty(name);
				MonoObject* mono_object = App->script_importer->ns_importer->GetMonoObjectFromGameObject(go);
				if (mono_object)
				{
					return mono_object;
				}
				else
				{
					MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
					if (c)
					{
						MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
						if (new_object)
						{
							App->script_importer->ns_importer->created_gameobjects[new_object] = go;
							return new_object;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

void CSScript::SetVector3Field(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
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
}

MonoObject * CSScript::GetVector3Field(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				float3 vector3 = script->GetVec3Property(name);
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheVector3");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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
	}
	return nullptr;
}

void CSScript::SetQuaternionField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
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
}

MonoObject * CSScript::GetQuaternionField(MonoObject * object, MonoString * field_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(field_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				float4 quat = script->GetVec4Property(name);
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheQuaternion");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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
	}
	return nullptr;
}

void CSScript::CallFunction(MonoObject * object, MonoString * function_name)
{
	Component* comp = App->script_importer->ns_importer->GetComponentFromMonoObject(object);

	if (comp)
	{
		if (comp->GetType() == Component::CompScript)
		{
			ComponentScript* comp_script = (ComponentScript*)comp;
			const char* name = mono_string_to_utf8(function_name);
			CSScript* script = (CSScript*)comp_script->GetScript();
			if (script)
			{
				/*NSScriptImporter* last_script = App->script_importer->GetCurrentSctipt();
				App->script_importer->SetCurrentScript(script);*/
				CONSOLE_LOG("function %s called from %s", name, script->GetName().c_str());
				MonoMethod* method = script->GetFunction(name, 0);
				if (method)
				{
					script->CallFunction(method, nullptr);
				}
				else
				{
					CONSOLE_ERROR("Function %s in script %s does not exist", name, script->GetName().c_str());
				}
				/*CONSOLE_LOG("Going back to %s", last_script->GetName().c_str());
				App->script_importer->SetCurrentScript(last_script);*/
			}
		}
	}
}

MonoObject * CSScript::LoadPrefab(MonoString* prefab_name)
{
	const char* name = mono_string_to_utf8(prefab_name);

	Prefab* prefab = App->resources->GetPrefab(name);
	if (prefab != nullptr)
	{
		GameObject* go = App->scene->LoadPrefabToScene(prefab);

		if (go)
		{
			MonoObject* mono_object = App->script_importer->ns_importer->GetMonoObjectFromGameObject(go);
			if (mono_object)
			{
				return mono_object;
			}
			else
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						App->script_importer->ns_importer->created_gameobjects[new_object] = go;
						return new_object;
					}
				}
			}
		}

	}

	return nullptr;
}