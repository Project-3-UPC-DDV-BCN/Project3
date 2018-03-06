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
	on_enable = nullptr;
	on_disable = nullptr;

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
}

bool CSScript::LoadScript(std::string script_path)
{
	bool ret = false;
	SetLibraryPath(script_path);

	if (mono_class)
	{
		mono_object = mono_object_new(App->script_importer->GetEngineDomain(), mono_class);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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
			MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
			if (c)
			{
				new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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

void CSScript::SetIntProperty(const char * propertyName, int value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
}

int CSScript::GetIntProperty(const char * propertyName)
{
	int value = 0;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
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
}

double CSScript::GetDoubleProperty(const char * propertyName)
{
	double value = 0.0;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
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
}

float CSScript::GetFloatProperty(const char * propertyName)
{
	float value = 0.0f;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
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
}

bool CSScript::GetBoolProperty(const char * propertyName)
{
	bool value = true;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
	}

	return value;
}

void CSScript::SetStringProperty(const char * propertyName, const char * value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		void* params = &value;
		mono_field_set_value(mono_object, field, params);
	}
}

std::string CSScript::GetStringProperty(const char * propertyName)
{
	std::string value = "";

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
	}

	return value;
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
}

GameObject * CSScript::GetGameObjectProperty(const char * propertyName)
{
	GameObject* value = nullptr;
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);
	if (field)
	{
		mono_field_get_value(mono_object, field, &value);
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

}

Texture * CSScript::GetTextureProperty(const char * propertyName)
{
	Texture* value = nullptr;
	MonoClassField* Field = mono_class_get_field_from_name(mono_class, propertyName);
	if (Field)
	{
		mono_field_get_value(mono_object, Field, &value);
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

MonoDomain * CSScript::GetDomain() const
{
	return mono_domain;
}

MonoImage * CSScript::GetImage() const
{
	return mono_image;
}

MonoClass * CSScript::GetClass() const
{
	return mono_class;
}

MonoAssembly * CSScript::GetAssembly() const
{
	return mono_assembly;
}

std::string CSScript::GetNameSpace() const
{
	return name_space;
}

std::string CSScript::_GetClassName() const
{
	return class_name;
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
	inside_function = true;
	MonoObject* exception = nullptr;
	MonoMarshalSpec* spec;

	MonoObject* obj = mono_runtime_invoke(function, mono_object, parameter, &exception);

	if (exception)
	{
		mono_print_unhandled_exception(exception);
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
		MonoObject* new_object = mono_object_new(App->script_importer->GetEngineDomain(), c);
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

	GameObject* go_parent = created_gameobjects[parent];
	if (go_parent != nullptr)
	{
		active_gameobject->SetParent(go_parent);
	}
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

MonoObject* CSScript::GetComponent(MonoObject * object, MonoReflectionType * type)
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
	else if (name == "TheEngine.TheRigidBody")
	{
		comp_name = "TheRigidBody";
	}

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
			created_gameobjects[new_object] = active_gameobject;
			return new_object;
		}
	}
	return nullptr;
}

MonoObject * CSScript::GetGameObject(MonoObject * object)
{
	if (!MonoObjectIsValid(object))
	{
		return nullptr;
	}

	if (!GameObjectIsValid())
	{
		return nullptr;
	}



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

	return nullptr;
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
	//RELEASE(text);
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

void CSScript::SetRTPvalue(MonoString* name, float value)
{
	const char* new_name = mono_string_to_utf8(name);
	App->audio->SetRTPvalue(new_name, value);
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
