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
#include "ModulePhysics.h"

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
		mono_gchandle_new(mono_object, 1);

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

void CSScript::OnCollisionEnter(CollisionData& col_data)
{
	if (on_collision_enter != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_collision_enter, &param);
		inside_function = false;
	}
}

void CSScript::OnCollisionStay(CollisionData& col_data)
{
	if (on_collision_stay != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_collision_stay, &param);
		inside_function = false;
	}
}

void CSScript::OnCollisionExit(CollisionData& col_data)
{
	if (on_collision_exit != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_collision_exit, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerEnter(CollisionData& col_data)
{
	if (on_trigger_enter != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_trigger_enter, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerStay(CollisionData& col_data)
{
	if (on_trigger_stay != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_trigger_stay, &param);
		inside_function = false;
	}
}

void CSScript::OnTriggerExit(CollisionData& col_data)
{
	if (on_trigger_exit != nullptr)
	{
		MonoObject* new_object = FillCollisionData(col_data);

		void* param = new_object;
		CallFunction(on_trigger_exit, &param);
		inside_function = false;
	}
}

MonoObject * CSScript::FillCollisionData(CollisionData & col_data)
{
	MonoObject* new_object = nullptr;

	MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheCollisionData");
	if (c)
	{
		new_object = mono_object_new(mono_domain, c);
		if (new_object)
		{
			mono_runtime_object_init(new_object);
			MonoClassField* impulse_field = mono_class_get_field_from_name(c, "Impulse");
			MonoClassField* contact_points_field = mono_class_get_field_from_name(c, "ContactPoints");
			MonoClassField* collider_field = mono_class_get_field_from_name(c, "Collider");

			if (impulse_field)
			{
				MonoObject* impulse_object = mono_field_get_value_object(App->script_importer->GetDomain(), impulse_field, new_object);
				MonoClass* impulse_class = mono_object_get_class(impulse_object);

				MonoClassField* x_field = mono_class_get_field_from_name(impulse_class, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(impulse_class, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(impulse_class, "z");

				mono_field_set_value(impulse_object, x_field, &col_data.impulse.x);
				mono_field_set_value(impulse_object, y_field, &col_data.impulse.y);
				mono_field_set_value(impulse_object, z_field, &col_data.impulse.z);
			}

			if (contact_points_field)
			{
				/*MonoObject* contact_points_field_object = mono_field_get_value_object(App->script_importer->GetDomain(), contact_points_field, new_object);
				MonoClass* contact_points_field_class = mono_object_get_class(contact_points_field_object);*/
				MonoClass* contact_point_class = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheContactPoint");

				MonoArray* contact_points_array = mono_array_new(App->script_importer->GetDomain(), contact_point_class, col_data.contacts.size());
				int index = 0;
				if (contact_points_array)
				{
					for (ContactPoint point : col_data.contacts)
					{
						if (contact_point_class)
						{
							MonoObject* contact_point_object = mono_object_new(App->script_importer->GetDomain(), contact_point_class);
							if (contact_point_object)
							{
								mono_runtime_object_init(contact_point_object);
								MonoClassField* cp_position_field = mono_class_get_field_from_name(contact_point_class, "Position");
								MonoClassField* cp_normal_field = mono_class_get_field_from_name(contact_point_class, "Normal");
								MonoClassField* cp_separation_field = mono_class_get_field_from_name(contact_point_class, "Separation");
								MonoClassField* cp_impulse_field = mono_class_get_field_from_name(contact_point_class, "Impulse");

								if (cp_position_field)
								{
									MonoObject* cp_position_object = mono_field_get_value_object(App->script_importer->GetDomain(), cp_position_field, contact_point_object);
									MonoClass* cp_position_class = mono_object_get_class(cp_position_object);

									MonoClassField* x_field = mono_class_get_field_from_name(cp_position_class, "x");
									MonoClassField* y_field = mono_class_get_field_from_name(cp_position_class, "y");
									MonoClassField* z_field = mono_class_get_field_from_name(cp_position_class, "z");

									mono_field_set_value(cp_position_object, x_field, &point.position.x);
									mono_field_set_value(cp_position_object, y_field, &point.position.y);
									mono_field_set_value(cp_position_object, z_field, &point.position.z);
								}

								if (cp_normal_field)
								{
									MonoObject* cp_normal_object = mono_field_get_value_object(App->script_importer->GetDomain(), cp_normal_field, contact_point_object);
									MonoClass* cp_normal_class = mono_object_get_class(cp_normal_object);

									MonoClassField* x_field = mono_class_get_field_from_name(cp_normal_class, "x");
									MonoClassField* y_field = mono_class_get_field_from_name(cp_normal_class, "y");
									MonoClassField* z_field = mono_class_get_field_from_name(cp_normal_class, "z");

									mono_field_set_value(cp_normal_object, x_field, &point.normal.x);
									mono_field_set_value(cp_normal_object, y_field, &point.normal.y);
									mono_field_set_value(cp_normal_object, z_field, &point.normal.z);
								}

								if (cp_separation_field) mono_field_set_value(contact_point_object, cp_separation_field, &point.separation);

								if (cp_impulse_field)
								{
									MonoObject* cp_impulse_object = mono_field_get_value_object(App->script_importer->GetDomain(), cp_impulse_field, contact_point_object);
									MonoClass* cp_impulse_class = mono_object_get_class(cp_impulse_object);

									MonoClassField* x_field = mono_class_get_field_from_name(cp_impulse_class, "x");
									MonoClassField* y_field = mono_class_get_field_from_name(cp_impulse_class, "y");
									MonoClassField* z_field = mono_class_get_field_from_name(cp_impulse_class, "z");

									mono_field_set_value(cp_impulse_object, x_field, &point.impulse.x);
									mono_field_set_value(cp_impulse_object, y_field, &point.impulse.y);
									mono_field_set_value(cp_impulse_object, z_field, &point.impulse.z);
								}

								mono_array_set(contact_points_array, MonoObject*, index, contact_point_object);
								index++;
							}
						}
					}
				}
				mono_field_set_value(new_object, contact_points_field, contact_points_array);
			}

			if (collider_field)
			{
				MonoObject* collider_object = nullptr;
				collider_object = App->script_importer->ns_importer->GetMonoObjectFromComponent((Component*)col_data.other_collider);
				mono_field_set_value(new_object, collider_field, collider_object);
			}
		}
	}

	return new_object;
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetIntProperty: Trying to get a non int field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetDoubleProperty: Trying to get a non double field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetFloatProperty: Trying to get a non float field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetBoolProperty: Trying to get a non bool field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetStringProperty: Trying to get a non string field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		MonoObject* object = mono_field_get_value_object(mono_domain, field, mono_object);
		if (object && value)
		{
			mono_gchandle_new(object, 1);
			App->script_importer->ns_importer->created_gameobjects[object] = value;
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
			//CONSOLE_ERROR("GetGameObjectProperty: Trying to get a non GameObject field");
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec2Property(const char * propertyName, float2 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_type_get_class(type);

		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), eclass);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(eclass, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(eclass, "y");

			if (x_field) mono_field_set_value(new_object, x_field, &value.x);
			if (y_field) mono_field_set_value(new_object, y_field, &value.y);

			mono_field_set_value(mono_object, field, new_object);

			mono_gchandle_new(new_object, 1);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float2 CSScript::GetVec2Property(const char * propertyName)
{
	float2 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoObject* vector = nullptr;
		mono_field_get_value(mono_object, field, &vector);

		if (vector)
		{
			MonoClass* c = mono_object_get_class(vector);

			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");

			if (x_field) mono_field_get_value(vector, x_field, &value.x);
			if (y_field) mono_field_get_value(vector, y_field, &value.y);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec3Property(const char * propertyName, float3 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_type_get_class(type);

		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), eclass);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(eclass, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(eclass, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(eclass, "z");

			if (x_field) mono_field_set_value(new_object, x_field, &value.x);
			if (y_field) mono_field_set_value(new_object, y_field, &value.y);
			if (z_field) mono_field_set_value(new_object, z_field, &value.z);

			mono_field_set_value(mono_object, field, new_object);

			mono_gchandle_new(new_object, 1);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float3 CSScript::GetVec3Property(const char * propertyName)
{
	float3 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoObject* vector = nullptr;
		mono_field_get_value(mono_object, field, &vector);

		if (vector)
		{
			MonoClass* c = mono_object_get_class(vector);

			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");

			if (x_field) mono_field_get_value(vector, x_field, &value.x);
			if (y_field) mono_field_get_value(vector, y_field, &value.y);
			if (z_field) mono_field_get_value(vector, z_field, &value.z);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}

	return value;
}

void CSScript::SetVec4Property(const char * propertyName, float4 value)
{
	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoType* type = mono_field_get_type(field);
		MonoClass* eclass = mono_type_get_class(type);

		MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), eclass);
		if (new_object)
		{
			MonoClassField* x_field = mono_class_get_field_from_name(eclass, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(eclass, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(eclass, "z");
			MonoClassField* w_field = mono_class_get_field_from_name(eclass, "w");

			if (x_field) mono_field_set_value(new_object, x_field, &value.x);
			if (y_field) mono_field_set_value(new_object, y_field, &value.y);
			if (z_field) mono_field_set_value(new_object, z_field, &value.z);
			if (w_field) mono_field_set_value(new_object, w_field, &value.w);

			mono_field_set_value(mono_object, field, new_object);

			mono_gchandle_new(new_object, 1);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
	}
}

float4 CSScript::GetVec4Property(const char * propertyName)
{
	float4 value;

	MonoClassField* field = mono_class_get_field_from_name(mono_class, propertyName);

	if (field)
	{
		MonoObject* vector = nullptr;
		mono_field_get_value(mono_object, field, &vector);

		if (vector)
		{
			MonoClass* c = mono_object_get_class(vector);

			MonoClassField* x_field = mono_class_get_field_from_name(c, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(c, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(c, "z");
			MonoClassField* w_field = mono_class_get_field_from_name(c, "w");

			if (x_field) mono_field_get_value(vector, x_field, &value.x);
			if (y_field) mono_field_get_value(vector, y_field, &value.y);
			if (z_field) mono_field_get_value(vector, z_field, &value.z);
			if (w_field) mono_field_get_value(vector, w_field, &value.w);
		}
	}
	else
	{
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		//CONSOLE_ERROR("Field '%s' does not exist in %s", propertyName, GetName().c_str());
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
		const char* name = mono_class_get_name(mono_class);
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
	for (std::map<std::string, int>::const_iterator it = int_fields.begin(); it != int_fields.end(); ++it)
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
	for (std::map<std::string, double>::const_iterator it = double_fields.begin(); it != double_fields.end(); ++it)
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
	for (std::map<std::string, float>::const_iterator it = float_fields.begin(); it != float_fields.end(); ++it)
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
	for (std::map<std::string, bool>::const_iterator it = bool_fields.begin(); it != bool_fields.end(); ++it)
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
	for (std::map<std::string, std::string>::const_iterator it = string_fields.begin(); it != string_fields.end(); ++it)
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
	for (std::map<std::string, GameObject*>::const_iterator it = gameobject_fields.begin(); it != gameobject_fields.end(); ++it)
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
	for (std::map<std::string, float2>::const_iterator it = vec2_fields.begin(); it != vec2_fields.end(); ++it)
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
	for (std::map<std::string, float3>::const_iterator it = vec3_fields.begin(); it != vec3_fields.end(); ++it)
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
	for (std::map<std::string, float4>::const_iterator it = vec4_fields.begin(); it != vec4_fields.end(); ++it)
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
		mono_assembly = text->mono_assembly;
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

		RELEASE(text);
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