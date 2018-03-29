#include "ModuleScriptImporter.h"
#include "Script.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include <mono/utils/mono-logger.h>
#include <mono/metadata/metadata.h>
#include "CSScript.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleResources.h"
#include "PropertiesWindow.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentRectTransform.h"
#include "ComponentText.h"
#include "ComponentProgressBar.h"
#include "ComponentRadar.h"
#include "ModuleEditor.h"
#include "ComponentFactory.h"
#include "ModuleTime.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ComponentAudioSource.h"
#include "ComponentRigidBody.h"
#include "ComponentGOAPAgent.h"
#include "GOAPGoal.h"
#include "ComponentScript.h"
#include <mono/metadata/attrdefs.h>

//CSScript* ModuleScriptImporter::current_script = nullptr;
bool ModuleScriptImporter::inside_function = false;
NSScriptImporter* ModuleScriptImporter::ns_importer = nullptr;

ModuleScriptImporter::ModuleScriptImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Script_Importer";
	mono_domain = nullptr;
	mono_engine_image = nullptr;
	ns_importer = new NSScriptImporter();
}

ModuleScriptImporter::~ModuleScriptImporter()
{
	RELEASE(ns_importer);
}

void MonoInternalWarning(const char * string, mono_bool is_stdout)
{
	CONSOLE_WARNING("%s", string);
}

void MonoInternalError(const char * string, mono_bool is_stdout)
{
	CONSOLE_ERROR("%s", string);
}

void MonoLogToLog(const char * log_domain, const char * log_level, const char * message, mono_bool fatal, void * user_data)
{
	if (fatal || log_level == "error")
	{
		CONSOLE_ERROR("%s%s", log_domain != nullptr ? ((std::string)log_domain + ": ").c_str() : "", message);
	}
	else if (log_level == "warning")
	{
		CONSOLE_WARNING("%s%s", log_domain != nullptr ? ((std::string)log_domain + ": ").c_str() : "", message);
	}
	else if (log_level == "critical")
	{
		CONSOLE_ERROR("%s%s", log_domain != nullptr ? ((std::string)log_domain + ": ").c_str() : "", message);
	}
	else
	{
		CONSOLE_LOG("%s%s", log_domain != nullptr ? ((std::string)log_domain + ": ").c_str() : "", message);
	}
}

bool ModuleScriptImporter::Init(Data * editor_config)
{
	mono_trace_set_log_handler(MonoLogToLog, this);
	mono_trace_set_print_handler(MonoInternalWarning);
	mono_trace_set_printerr_handler(MonoInternalError);

	mono_path = App->file_system->GetFullPath("mono/");

	mono_set_dirs((mono_path + "lib").c_str(), (mono_path + "etc").c_str());

	mono_domain = mono_jit_init("TheCreator");

	MonoAssembly* engine_assembly = mono_domain_assembly_open(mono_domain, REFERENCE_ASSEMBLIES_FOLDER"TheEngine.dll");
	if (engine_assembly)
	{
		mono_engine_image = mono_assembly_get_image(engine_assembly);
		RegisterAPI();
		DumpEngineDLLInfo(engine_assembly, mono_engine_image);
	}
	else
	{
		CONSOLE_ERROR("Can't load 'TheEngine.dll'!");
		return false;
	}

	MonoAssembly* compiler_assembly = mono_domain_assembly_open(mono_domain, REFERENCE_ASSEMBLIES_FOLDER"ScriptCompiler.dll");
	if (compiler_assembly)
	{
		mono_compiler_image = mono_assembly_get_image(compiler_assembly);
	}
	else
	{
		CONSOLE_ERROR("Can't load 'ScriptCompiler.dll'!");
		return false;
	}

	return true;
}

std::string ModuleScriptImporter::ImportScript(std::string path)
{
	std::string ret = "";
	std::string lib;
	std::string result = CompileScript(path, lib);

	if (result != "")
	{
		for (int i = 0; i < result.size(); i++)
			{
			std::string message;
			while (result[i] != '|' && result[i] != '\0')
			{
				message += result[i];
				i++;
			}
			if (message.find("[Warning]") != std::string::npos)
			{
				message.erase(0, 9);
				CONSOLE_WARNING("%s", message.c_str());
			}
			else if (message.find("[Error]") != std::string::npos)
			{
				message.erase(0, 7);
				CONSOLE_ERROR("%s", message.c_str());
			}
			else
			{
				CONSOLE_LOG("%s", message.c_str());
			}
		}
	}
	else
	{
		return lib;
	}

	return ret;
}

Script * ModuleScriptImporter::LoadScriptFromLibrary(std::string path)
{
	MonoAssembly* assembly = mono_domain_assembly_open(mono_domain, path.c_str());
	if (assembly)
	{
		CSScript* script = DumpAssemblyInfo(assembly);
		if (script != nullptr)
		{
			if (script->LoadScript(path))
			{
				return script;
			}
		}
	}

	return nullptr;
}

MonoDomain * ModuleScriptImporter::GetDomain() const
{
	return mono_domain;
}

MonoImage * ModuleScriptImporter::GetEngineImage() const
{
	return mono_engine_image;
}

std::string ModuleScriptImporter::CompileScript(std::string assets_path, std::string& lib)
{
	std::string script_name = App->file_system->GetFileNameWithoutExtension(assets_path);
	std::string ret;
	if (!App->file_system->DirectoryExist(LIBRARY_SCRIPTS_FOLDER_PATH)) App->file_system->Create_Directory(LIBRARY_SCRIPTS_FOLDER_PATH);
	std::string output_name = LIBRARY_SCRIPTS_FOLDER + script_name + ".dll";
	lib = LIBRARY_SCRIPTS_FOLDER + script_name + ".dll";

	MonoClass* compiler_class = mono_class_from_name(mono_compiler_image, "Compiler", "Compiler");
	if (compiler_class)
	{
		MonoMethod* method = method = mono_class_get_method_from_name(compiler_class, "Compile", 2);

		if (method)
		{
			MonoObject* exception = nullptr;
			MonoObject* class_object = mono_object_new(mono_domain, compiler_class);
			void* args[2];
			MonoString* input_path = mono_string_new(mono_domain, assets_path.c_str());
			MonoString* output_path = mono_string_new(mono_domain, output_name.c_str());
			
			args[0] = input_path;
			args[1] = output_path;

			MonoString* handle = (MonoString*)mono_runtime_invoke(method, NULL, args, &exception);

			if (exception)
			{
				mono_print_unhandled_exception(exception);
			}
			else
			{
				ret = mono_string_to_utf8(handle);
			}
		}
	}

	return ret;
}

//void ModuleScriptImporter::SetCurrentScript(CSScript * script)
//{
//	current_script = script;
//}

//CSScript * ModuleScriptImporter::GetCurrentSctipt() const
//{
//	return current_script;
//}

CSScript* ModuleScriptImporter::DumpAssemblyInfo(MonoAssembly * assembly)
{
	MonoImage* mono_image = mono_assembly_get_image(assembly);
	if (mono_image)
	{
		std::string class_name;
		std::string name_space;
		MonoClass* loaded_script = DumpClassInfo(mono_image, class_name, name_space);

		if (loaded_script != nullptr)
		{
			CSScript* script = new CSScript();
			script->SetDomain(mono_domain);
			script->SetImage(mono_image);
			script->SetName(class_name);
			script->SetClassName(class_name);
			script->SetNameSpace(name_space);
			script->SetClass(loaded_script);
			return script;
		}
	}
}

MonoClass* ModuleScriptImporter::DumpClassInfo(MonoImage * image, std::string& class_name, std::string& name_space)
{
	MonoClass* mono_class = nullptr;

	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

	int rows = mono_table_info_get_rows(table_info);

	for (int i = 1; i < rows; i++) 
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* _name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		mono_class = mono_class_from_name(image, _name_space, name);

		if (mono_class)
		{
			class_name = name;
			name_space = _name_space;
		}
	}

	return mono_class;
}

void ModuleScriptImporter::DumpEngineDLLInfo(MonoAssembly * assembly, MonoImage* image)
{
	const MonoTableInfo* class_table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int class_table_rows = mono_table_info_get_rows(class_table_info);
	for (int i = 1; i < class_table_rows; i++) {
		DLLClassInfo class_info;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(class_table_info, i, cols, MONO_TYPEDEF_SIZE);
		class_info.name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		MonoClass* m_class = mono_class_from_name(image, name_space, class_info.name);

		void* properties_iter = nullptr;
		MonoProperty* property;
		while ((property = mono_class_get_properties(m_class, &properties_iter)))
		{
			MonoMethod* method = mono_property_get_get_method(property);
			if (!(mono_method_get_flags(method, NULL) & MONO_METHOD_ATTR_PUBLIC))
			{
				continue;
			}
			DLLPropertyInfo property_info;
			property_info.name = mono_property_get_name(property);
			MonoMethodSignature* sig = mono_method_get_signature(method, image, 0);
			MonoType* return_type = mono_signature_get_return_type(sig);
			std::string return_name = mono_type_get_name(return_type);
			if (return_name.find("TheEngine") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 10);
			else if (return_name.find("System") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 7);
			if (return_name == "Single") return_name = "Float";
			else if (return_name == "Int32") return_name = "Int";
			property_info.returning_type = return_name.c_str();
			class_info.properties.push_back(property_info);
		}

		void* fields_iter = nullptr;
		MonoClassField* field;
		while ((field = mono_class_get_fields(m_class, &fields_iter)))
		{
			if (!(mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC))
			{
				continue;
			}
			DLLFieldsInfo field_info;
			field_info.name = mono_field_get_name(field);
			MonoType* return_type = mono_field_get_type(field);
			std::string return_name = mono_type_get_name(return_type);
			if (return_name.find("TheEngine") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 10);
			else if (return_name.find("System") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 7);
			if (return_name == "Single") return_name = "Float";
			else if (return_name == "Int32") return_name = "Int";
			field_info.returning_type = return_name.c_str();
			class_info.fields.push_back(field_info);
		}

		MonoMethod *method;
		void* iter = nullptr;
		while ((method = mono_class_get_methods(m_class, &iter)))
		{
			if (!(mono_method_get_flags(method, NULL) & MONO_METHOD_ATTR_PUBLIC) || mono_method_get_flags(method, NULL) == 6278
				|| mono_method_get_flags(method, NULL) == 2182 || mono_method_get_flags(method, NULL) == 2198)
			{
				continue;
			}
			std::string method_name = mono_method_get_name(method);
			const MonoTableInfo* method_table_info = mono_image_get_table_info(image, MONO_TABLE_METHOD);
			int method_table_rows = mono_table_info_get_rows(method_table_info);
			const MonoTableInfo* param_table_info = mono_image_get_table_info(image, MONO_TABLE_PARAM);
			int param_table_rows = mono_table_info_get_rows(param_table_info);
			uint index = mono_method_get_index(method);
			uint param_index = mono_metadata_decode_row_col(method_table_info, index - 1, MONO_METHOD_PARAMLIST);
			uint lastp = 0;
			if (index < method_table_rows)
				lastp = mono_metadata_decode_row_col(method_table_info, index, MONO_METHOD_PARAMLIST);
			else
				lastp = param_table_rows + 1;
			uint cols[MONO_PARAM_SIZE];

			DLLMethodInfo method_info;
			if (mono_method_get_flags(method, NULL) & MONO_METHOD_ATTR_STATIC) method_info.is_static = true;
			MonoMethodSignature* sig = mono_method_get_signature(method, image, 0);
			MonoType* return_type = mono_signature_get_return_type(sig);
			std::string return_name = mono_type_get_name(return_type);
			if (return_name.find("TheEngine") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 10);
			else if (return_name.find("System") != std::string::npos) return_name.erase(return_name.begin(), return_name.begin() + 7);
			if (return_name == "Single") return_name = "Float";
			else if (return_name == "Int32") return_name = "Int";
			std::string template_name = "";
			method_info.method_name = method_name;
			if (return_name == "T")
			{
				method_info.method_name += "<>";
				return_name = "TheComponent";
				template_name = "<TheComponent>";
			}
			else method_info.method_name += "()";
			method_info.returning_type = return_name;
			method_info.declaration += return_name + " " + method_name + template_name + "(";
			
			void* param_iter = nullptr;
			MonoType* param_type = nullptr;
			uint param_count = mono_signature_get_param_count(sig);
			if (param_count == 0)
			{
				method_info.declaration += ")";
			}
			int param_index_start = 0;
			while ((param_type = mono_signature_get_params(sig, &param_iter)))
			{
				std::string param_type_name = mono_type_get_name(param_type);
				if (param_type_name.find("TheEngine") != std::string::npos) param_type_name.erase(param_type_name.begin(), param_type_name.begin() + 10);
				else if (param_type_name.find("System") != std::string::npos) param_type_name.erase(param_type_name.begin(), param_type_name.begin() + 7);
				if (mono_signature_param_is_out(sig, param_index_start))
				{
					method_info.declaration += "out ";
				}
				else if (param_type_name.find('&') != std::string::npos)
				{
					method_info.declaration += "ref ";
				}
				if (param_type_name == "Single") param_type_name = "Float";
				else if (param_type_name == "Int32") param_type_name = "Int";
				method_info.declaration += param_type_name + " ";
				mono_metadata_decode_row(param_table_info, param_index - 1, cols, MONO_PARAM_SIZE);
				const char* param_name = mono_metadata_string_heap(image, cols[MONO_PARAM_NAME]);
				method_info.declaration += param_name;
				param_count--;
				if (param_count != 0)
				{
					method_info.declaration += ", ";
				}
				else
				{
					method_info.declaration += ")";
				}
				param_index++;
				param_index_start++;
			}
			class_info.methods.push_back(method_info);
		}
		engine_dll_info.push_back(class_info);
	}
}

void ModuleScriptImporter::RegisterAPI()
{
	//GAMEOBJECT
	mono_add_internal_call("TheEngine.TheGameObject::CreateNewGameObject", (const void*)CreateGameObject);
	mono_add_internal_call("TheEngine.TheGameObject::SetName", (const void*)SetGameObjectName);
	mono_add_internal_call("TheEngine.TheGameObject::GetName", (const void*)GetGameObjectName);
	mono_add_internal_call("TheEngine.TheGameObject::SetActive", (const void*)SetGameObjectActive);
	mono_add_internal_call("TheEngine.TheGameObject::IsActive", (const void*)GetGameObjectIsActive);
	mono_add_internal_call("TheEngine.TheGameObject::SetTag", (const void*)SetGameObjectTag);
	mono_add_internal_call("TheEngine.TheGameObject::GetTag", (const void*)GetGameObjectTag);
	mono_add_internal_call("TheEngine.TheGameObject::SetLayer", (const void*)SetGameObjectLayer);
	mono_add_internal_call("TheEngine.TheGameObject::GetLayer", (const void*)GetGameObjectLayer);
	mono_add_internal_call("TheEngine.TheGameObject::SetStatic", (const void*)SetGameObjectStatic);
	mono_add_internal_call("TheEngine.TheGameObject::IsStatic", (const void*)GameObjectIsStatic);
	mono_add_internal_call("TheEngine.TheGameObject::Duplicate", (const void*)DuplicateGameObject);
	mono_add_internal_call("TheEngine.TheGameObject::SetParent", (const void*)SetGameObjectParent);
	mono_add_internal_call("TheEngine.TheGameObject::GetParent", (const void*)GetGameObjectParent);
	mono_add_internal_call("TheEngine.TheGameObject::GetSelf", (const void*)GetSelfGameObject);
	mono_add_internal_call("TheEngine.TheGameObject::GetChild(int)", (const void*)GetGameObjectChild);
	mono_add_internal_call("TheEngine.TheGameObject::GetChild(string)", (const void*)GetGameObjectChildString);
	mono_add_internal_call("TheEngine.TheGameObject::GetChildCount", (const void*)GetGameObjectChildCount);
	mono_add_internal_call("TheEngine.TheGameObject::AddComponent", (const void*)AddComponent);
	mono_add_internal_call("TheEngine.TheGameObject::DestroyComponent", (const void*)DestroyComponent);
	mono_add_internal_call("TheEngine.TheGameObject::GetComponent", (const void*)GetComponent);
	mono_add_internal_call("TheEngine.TheGameObject::Find", (const void*)FindGameObject);
	mono_add_internal_call("TheEngine.TheGameObject::GetSceneGameObjects", (const void*)GetSceneGameObjects);
	mono_add_internal_call("TheEngine.TheGameObject::GetObjectsInFrustum", (const void*)GetObjectsInFrustum);
	mono_add_internal_call("TheEngine.TheGameObject::GetAllChilds", (const void*)GetAllChilds);

	//COMPONENT
	mono_add_internal_call("TheEngine.TheComponent::SetComponentActive", (const void*)SetComponentActive);

	//TRANSFORM
	mono_add_internal_call("TheEngine.TheTransform::SetPosition", (const void*)SetPosition);
	mono_add_internal_call("TheEngine.TheTransform::GetPosition", (const void*)GetPosition);
	mono_add_internal_call("TheEngine.TheTransform::SetRotation", (const void*)SetRotation);
	mono_add_internal_call("TheEngine.TheTransform::GetRotation", (const void*)GetRotation);
	mono_add_internal_call("TheEngine.TheTransform::SetScale", (const void*)SetScale);
	mono_add_internal_call("TheEngine.TheTransform::GetScale", (const void*)GetScale);
	mono_add_internal_call("TheEngine.TheTransform::LookAt", (const void*)LookAt);
	mono_add_internal_call("TheEngine.TheTransform::GetForward", (const void*)GetForward);
	mono_add_internal_call("TheEngine.TheTransform::GetUp", (const void*)GetUp);
	mono_add_internal_call("TheEngine.TheTransform::GetRight", (const void*)GetRight);
	mono_add_internal_call("TheEngine.TheTransform::RotateAroundAxis", (const void*)RotateAroundAxis);

	//RECTTRANSFORM
	mono_add_internal_call("TheEngine.TheRectTransform::SetRectPosition", (const void*)SetRectPosition);
	mono_add_internal_call("TheEngine.TheRectTransform::GetRectPosition", (const void*)GetRectPosition);
	mono_add_internal_call("TheEngine.TheRectTransform::SetRectRotation", (const void*)SetRectRotation);
	mono_add_internal_call("TheEngine.TheRectTransform::GetRectRotation", (const void*)GetRectRotation);
	mono_add_internal_call("TheEngine.TheRectTransform::SetRectSize", (const void*)SetRectSize);
	mono_add_internal_call("TheEngine.TheRectTransform::GetRectSize", (const void*)GetRectSize);
	mono_add_internal_call("TheEngine.TheRectTransform::SetRectAnchor", (const void*)SetRectAnchor);
	mono_add_internal_call("TheEngine.TheRectTransform::GetRectAnchor", (const void*)GetRectAnchor);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnClick", (const void*)GetOnClick);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnClickDown", (const void*)GetOnClickDown);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnClickUp", (const void*)GetOnClickUp);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnMouseOver", (const void*)GetOnMouseOver);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnMouseEnter", (const void*)GetOnMouseEnter);
	mono_add_internal_call("TheEngine.TheRectTransform::GetOnMouseOut", (const void*)GetOnMouseOut);

	//TEXT
	mono_add_internal_call("TheEngine.TheText::SetText", (const void*)SetText);
	mono_add_internal_call("TheEngine.TheText::GettText", (const void*)GetText);

	//PROGRESSBAR
	mono_add_internal_call("TheEngine.TheProgressBar::SetPercentageProgress", (const void*)SetPercentageProgress);
	mono_add_internal_call("TheEngine.TheProgressBar::GetPercentageProgress", (const void*)GetPercentageProgress);

	//RADAR
	mono_add_internal_call("TheEngine.TheRadar::AddEntity", (const void*)AddEntity);
	mono_add_internal_call("TheEngine.TheRadar::RemoveEntity", (const void*)RemoveEntity);
	mono_add_internal_call("TheEngine.TheRadar::RemoveAllEntities", (const void*)RemoveAllEntities);
	mono_add_internal_call("TheEngine.TheRadar::SetMarkerToEntity", (const void*)SetMarkerToEntity);

	//FACTORY
	mono_add_internal_call("TheEngine.TheFactory::StartFactory", (const void*)StartFactory);
	mono_add_internal_call("TheEngine.TheFactory::Spawn", (const void*)Spawn);
	mono_add_internal_call("TheEngine.TheFactory::SetSpawnPosition", (const void*)SetSpawnPosition);
	mono_add_internal_call("TheEngine.TheFactory::SetSpawnRotation", (const void*)SetSpawnRotation);
	mono_add_internal_call("TheEngine.TheFactory::SetSpawnScale", (const void*)SetSpawnScale);

	//VECTOR/QUATERNION
	mono_add_internal_call("TheEngine.TheVector3::ToQuaternion", (const void*)ToQuaternion);
	mono_add_internal_call("TheEngine.TheQuaternion::ToEulerAngles", (const void*)ToEulerAngles);

	//DATA SAVE/LOAD
	mono_add_internal_call("TheEngine.TheData::AddString", (const void*)AddString);
	mono_add_internal_call("TheEngine.TheData::AddInt", (const void*)AddInt);
	mono_add_internal_call("TheEngine.TheData::AddFloat", (const void*)AddFloat);
	mono_add_internal_call("TheEngine.TheData::GetString", (const void*)GetString);
	mono_add_internal_call("TheEngine.TheData::GetInt", (const void*)GetInt);
	mono_add_internal_call("TheEngine.TheData::GetFloat", (const void*)GetFloat);

	//TIME
	mono_add_internal_call("TheEngine.TheTime::SetTimeScale", (const void*)SetTimeScale);
	mono_add_internal_call("TheEngine.TheTime::GetTimeScale", (const void*)GetTimeScale);
	mono_add_internal_call("TheEngine.TheTime::GetDeltaTime", (const void*)GetDeltaTime);

	//INPUT
	mono_add_internal_call("TheEngine.TheInput::IsKeyDown", (const void*)IsKeyDown);
	mono_add_internal_call("TheEngine.TheInput::IsKeyUp", (const void*)IsKeyUp);
	mono_add_internal_call("TheEngine.TheInput::IsKeyRepeat", (const void*)IsKeyRepeat);
	mono_add_internal_call("TheEngine.TheInput::IsMouseButtonDown", (const void*)IsMouseDown);
	mono_add_internal_call("TheEngine.TheInput::IsMouseButtonUp", (const void*)IsMouseUp);
	mono_add_internal_call("TheEngine.TheInput::IsMouseButtonRepeat", (const void*)IsMouseRepeat);
	mono_add_internal_call("TheEngine.TheInput::GetMousePosition", (const void*)GetMousePosition);
	mono_add_internal_call("TheEngine.TheInput::GetMouseXMotion", (const void*)GetMouseXMotion);
	mono_add_internal_call("TheEngine.TheInput::GetMouseYMotion", (const void*)GetMouseYMotion);
	mono_add_internal_call("TheEngine.TheInput::GetControllerButton", (const void*)GetControllerButton);
	mono_add_internal_call("TheEngine.TheInput::GetControllerJoystickMove", (const void*)GetControllerJoystickMove);
	mono_add_internal_call("TheEngine.TheInput::RumbleController", (const void*)RumbleController);

	//CONSOLE
	mono_add_internal_call("TheEngine.TheConsole.TheConsole::Log", (const void*)Log);
	mono_add_internal_call("TheEngine.TheConsole.TheConsole::Warning", (const void*)Warning);
	mono_add_internal_call("TheEngine.TheConsole.TheConsole::Error", (const void*)Error);

	//Audio
	mono_add_internal_call("TheEngine.TheAudio::IsMuted", (const void*)IsMuted);
	mono_add_internal_call("TheEngine.TheAudio::SetMute", (const void*)SetMute);
	mono_add_internal_call("TheEngine.TheAudio::GetVolume", (const void*)GetVolume);
	mono_add_internal_call("TheEngine.TheAudio::GetPitch", (const void*)GetPitch);
	mono_add_internal_call("TheEngine.TheAudio::SetPitch", (const void*)SetPitch);

	mono_add_internal_call("TheEngine.TheAudio::SetRTPCvalue", (const void*)SetRTPCvalue);

	mono_add_internal_call("TheEngine.TheAudioSource::Play", (const void*)Play);
	mono_add_internal_call("TheEngine.TheAudioSource::Stop", (const void*)Stop);
	mono_add_internal_call("TheEngine.TheAudioSource::Send", (const void*)Send);
	mono_add_internal_call("TheEngine.TheAudioSource::SetMyRTPCvalue", (const void*)SetMyRTPCvalue);
	mono_add_internal_call("TheEngine.TheAudioSource::SetState", (const void*)SetState);
	mono_add_internal_call("TheEngine.TheAudioSource::SetVolume", (const void*)SetVolume);

	//EMITER
	mono_add_internal_call("TheEngine.TheParticleEmmiter::Play", (const void*)PlayEmmiter);
	mono_add_internal_call("TheEngine.TheParticleEmmiter::Stop", (const void*)StopEmmiter);

	//RIGIDBODY
	mono_add_internal_call("TheEngine.TheRigidBody::SetLinearVelocity", (const void*)SetLinearVelocity);
	mono_add_internal_call("TheEngine.TheRigidBody::SetAngularVelocity", (const void*)SetAngularVelocity);
	mono_add_internal_call("TheEngine.TheRigidBody::SetPosition", (const void*)SetRBPosition);
	mono_add_internal_call("TheEngine.TheRigidBody::SetRotation", (const void*)SetRBRotation);
	mono_add_internal_call("TheEngine.TheRigidBody::DisableCollider", (const void*)DisableCollider);
	mono_add_internal_call("TheEngine.TheRigidBody::DisableAllColliders", (const void*)DisableAllColliders);
	mono_add_internal_call("TheEngine.TheRigidBody::IsTransformGO", (const bool*)IsTransformGO);
	mono_add_internal_call("TheEngine.TheRigidBody::AddTorque", (const void*)AddTorque);
	mono_add_internal_call("TheEngine.TheRigidBody::SetTransformGO", (const void*)SetTransformGO);
	mono_add_internal_call("TheEngine.TheRigidBody::IsKinematic", (const bool*)IsKinematic);
	mono_add_internal_call("TheEngine.TheRigidBody::SetKinematic", (const void*)SetKinematic);

	//GOAP
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetBlackboardVariableB", (const void*)GetBlackboardVariableB);
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetBlackboardVariableF", (const void*)GetBlackboardVariableF);
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetNumGoals", (const void*)GetNumGoals);
	mono_add_internal_call("TheEngine.TheGOAPAgent::SetGoalPriority", (const void*)SetGoalPriority);
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetGoalPriority", (const void*)GetGoalPriority);
	mono_add_internal_call("TheEngine.TheGOAPAgent::CompleteAction", (const void*)CompleteAction);
	mono_add_internal_call("TheEngine.TheGOAPAgent::FailAction", (const void*)FailAction);
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetGoalName", (const void*)GetGoalName);
	mono_add_internal_call("TheEngine.TheGOAPAgent::GetGoalConditionName", (const void*)GetGoalConditionName);
	mono_add_internal_call("TheEngine.TheGOAPAgent::SetBlackboardVariable(string, float)", (const void*)SetBlackboardVariable);
	mono_add_internal_call("TheEngine.TheGOAPAgent::SetBlackboardVariable(string, bool)", (const void*)SetBlackboardVariableB);

	//RANDOM
	mono_add_internal_call("TheEngine.TheRandom::RandomInt", (const void*)RandomInt);
	mono_add_internal_call("TheEngine.TheRandom::RandomFloat", (const void*)RandomFloat);
	mono_add_internal_call("TheEngine.TheRandom::RandomRange", (const void*)RandomRange);

	//SCRIPT
	mono_add_internal_call("TheEngine.TheScript::SetBoolField", (const void*)SetBoolField);
	mono_add_internal_call("TheEngine.TheScript::GetBoolField", (const void*)GetBoolField);
	mono_add_internal_call("TheEngine.TheScript::SetIntField", (const void*)SetIntField);
	mono_add_internal_call("TheEngine.TheScript::GetIntField", (const void*)GetIntField);
	mono_add_internal_call("TheEngine.TheScript::SetFloatField", (const void*)SetFloatField);
	mono_add_internal_call("TheEngine.TheScript::GetFloatField", (const void*)GetFloatField);
	mono_add_internal_call("TheEngine.TheScript::SetDoubleField", (const void*)SetDoubleField);
	mono_add_internal_call("TheEngine.TheScript::GetDoubleField", (const void*)GetDoubleField);
	mono_add_internal_call("TheEngine.TheScript::SetStringField", (const void*)SetStringField);
	mono_add_internal_call("TheEngine.TheScript::GetStringField", (const void*)GetStringField);
	mono_add_internal_call("TheEngine.TheScript::SetGameObjectField", (const void*)SetGameObjectField);
	mono_add_internal_call("TheEngine.TheScript::GetGameObjectField", (const void*)GetGameObjectField);
	mono_add_internal_call("TheEngine.TheScript::SetVector3Field", (const void*)SetVector3Field);
	mono_add_internal_call("TheEngine.TheScript::GetVector3Field", (const void*)GetVector3Field);
	mono_add_internal_call("TheEngine.TheScript::SetQuaternionField", (const void*)SetQuaternionField);
	mono_add_internal_call("TheEngine.TheScript::GetQuaternionField", (const void*)GetQuaternionField);
	mono_add_internal_call("TheEngine.TheScript::CallFunction", (const void*)CallFunction);

	//APPLICATION
	mono_add_internal_call("TheEngine.TheApplication::LoadScene", (const void*)LoadScene);
	mono_add_internal_call("TheEngine.TheApplication::Quit", (const void*)Quit);
}

void ModuleScriptImporter::SetGameObjectName(MonoObject * object, MonoString * name)
{
	ns_importer->SetGameObjectName(object, name);
}

MonoString* ModuleScriptImporter::GetGameObjectName(MonoObject * object)
{
	return ns_importer->GetGameObjectName(object);
}

void ModuleScriptImporter::SetGameObjectActive(MonoObject * object, mono_bool active)
{
	ns_importer->SetGameObjectActive(object, active);
}

mono_bool ModuleScriptImporter::GetGameObjectIsActive(MonoObject * object)
{
	return ns_importer->GetGameObjectIsActive(object);
}

void ModuleScriptImporter::CreateGameObject(MonoObject * object)
{
	ns_importer->CreateGameObject(object);
}

MonoObject* ModuleScriptImporter::GetSelfGameObject()
{
	return ns_importer->GetSelfGameObject();
}

void ModuleScriptImporter::SetGameObjectTag(MonoObject * object, MonoString * name)
{
	ns_importer->SetGameObjectTag(object, name);
}

MonoString* ModuleScriptImporter::GetGameObjectTag(MonoObject * object)
{
	return ns_importer->GetGameObjectTag(object);
}

void ModuleScriptImporter::SetGameObjectLayer(MonoObject * object, MonoString * layer)
{
	ns_importer->SetGameObjectLayer(object, layer);
}

MonoString * ModuleScriptImporter::GetGameObjectLayer(MonoObject * object)
{
	return ns_importer->GetGameObjectLayer(object);
}

void ModuleScriptImporter::SetGameObjectStatic(MonoObject * object, mono_bool value)
{
	ns_importer->SetGameObjectStatic(object, value);
}

mono_bool ModuleScriptImporter::GameObjectIsStatic(MonoObject * object)
{
	return ns_importer->GameObjectIsStatic(object);
}

MonoObject * ModuleScriptImporter::DuplicateGameObject(MonoObject * object)
{
	return ns_importer->DuplicateGameObject(object);
}

void ModuleScriptImporter::SetGameObjectParent(MonoObject * object, MonoObject * parent)
{
	ns_importer->SetGameObjectParent(object, parent);
}

MonoObject* ModuleScriptImporter::GetGameObjectParent(MonoObject * object)
{
	return ns_importer->GetGameObjectParent(object);
}

MonoObject * ModuleScriptImporter::GetGameObjectChild(MonoObject * object, int index)
{
	return ns_importer->GetGameObjectChild(object, index);
}

MonoObject * ModuleScriptImporter::GetGameObjectChildString(MonoObject * object, MonoString * name)
{
	return ns_importer->GetGameObjectChildString(object, name);
}

int ModuleScriptImporter::GetGameObjectChildCount(MonoObject * object)
{
	return ns_importer->GetGameObjectChildCount(object);
}

MonoObject * ModuleScriptImporter::FindGameObject(MonoString * gameobject_name)
{
	return ns_importer->FindGameObject(gameobject_name);
}

MonoArray * ModuleScriptImporter::GetSceneGameObjects(MonoObject * object)
{
	return ns_importer->GetSceneGameObjects(object);
}

MonoArray * ModuleScriptImporter::GetObjectsInFrustum(MonoObject * pos, MonoObject* front, MonoObject* up, float nearPlaneDist, float farPlaneDist )
{
	return ns_importer->GetObjectsInFrustum(pos, front, up, nearPlaneDist, farPlaneDist);
}

MonoArray * ModuleScriptImporter::GetAllChilds(MonoObject * object)
{
	return ns_importer->GetAllChilds(object);
}

void ModuleScriptImporter::SetComponentActive(MonoObject * object, bool active)
{
	ns_importer->SetComponentActive(object, active);
}

MonoObject* ModuleScriptImporter::AddComponent(MonoObject * object, MonoReflectionType* type)
{
	return ns_importer->AddComponent(object, type);
}

MonoObject* ModuleScriptImporter::GetComponent(MonoObject * object, MonoReflectionType * type, int index)
{
	return ns_importer->GetComponent(object, type, index);
}

void ModuleScriptImporter::DestroyComponent(MonoObject * object, MonoObject * cmp)
{
	ns_importer->DestroyComponent(object, cmp);
}

void ModuleScriptImporter::SetPosition(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetPosition(object, vector3);
}

MonoObject* ModuleScriptImporter::GetPosition(MonoObject * object, mono_bool is_global)
{
	return ns_importer->GetPosition(object, is_global);
}

void ModuleScriptImporter::SetRotation(MonoObject * object, MonoObject * vector)
{
	ns_importer->SetRotation(object, vector);
}

MonoObject * ModuleScriptImporter::GetRotation(MonoObject * object, mono_bool is_global)
{
	return ns_importer->GetRotation(object, is_global);
}

void ModuleScriptImporter::SetScale(MonoObject * object, MonoObject * vector)
{
	ns_importer->SetScale(object, vector);
}

MonoObject * ModuleScriptImporter::GetScale(MonoObject * object, mono_bool is_global)
{
	return ns_importer->GetScale(object, is_global);
}

void ModuleScriptImporter::LookAt(MonoObject * object, MonoObject * vector)
{
	ns_importer->LookAt(object, vector);
}

void ModuleScriptImporter::SetRectPosition(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetRectPosition(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectPosition(MonoObject * object)
{
	return ns_importer->GetRectPosition(object);
}

void ModuleScriptImporter::SetRectRotation(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetRectRotation(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectRotation(MonoObject * object)
{
	return ns_importer->GetRectRotation(object);
}

void ModuleScriptImporter::SetRectSize(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetRectSize(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectSize(MonoObject * object)
{
	return ns_importer->GetRectSize(object);
}

void ModuleScriptImporter::SetRectAnchor(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetRectAnchor(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectAnchor(MonoObject * object)
{
	return ns_importer->GetRectAnchor(object);
}

mono_bool ModuleScriptImporter::GetOnClick(MonoObject * object)
{
	return ns_importer->GetOnClick(object);
}

mono_bool ModuleScriptImporter::GetOnClickDown(MonoObject * object)
{
	return ns_importer->GetOnClickDown(object);
}

mono_bool ModuleScriptImporter::GetOnClickUp(MonoObject * object)
{
	return ns_importer->GetOnClickUp(object);
}

mono_bool ModuleScriptImporter::GetOnMouseEnter(MonoObject * object)
{
	return ns_importer->GetOnMouseEnter(object);
}

mono_bool ModuleScriptImporter::GetOnMouseOver(MonoObject * object)
{
	return ns_importer->GetOnMouseOver(object);
}

mono_bool ModuleScriptImporter::GetOnMouseOut(MonoObject * object)
{
	return ns_importer->GetOnMouseOut(object);
}

void ModuleScriptImporter::SetText(MonoObject * object, MonoString* text)
{
	return ns_importer->SetText(object, text);
}

MonoString* ModuleScriptImporter::GetText(MonoObject * object)
{
	return ns_importer->GetText(object);
}

void ModuleScriptImporter::SetPercentageProgress(MonoObject * object, float progress)
{
	ns_importer->SetPercentageProgress(object, progress);
}

float ModuleScriptImporter::GetPercentageProgress(MonoObject * object)
{
	return ns_importer->GetPercentageProgress(object);
}

void ModuleScriptImporter::AddEntity(MonoObject * object, MonoObject * game_object)
{
	ns_importer->AddEntity(object, game_object);
}

void ModuleScriptImporter::RemoveEntity(MonoObject * object, MonoObject * game_object)
{
	ns_importer->RemoveEntity(object, game_object);
}

void ModuleScriptImporter::RemoveAllEntities(MonoObject * object)
{
	ns_importer->RemoveAllEntities(object);
}

void ModuleScriptImporter::SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString * marker_name)
{
	ns_importer->SetMarkerToEntity(object, game_object, marker_name);
}

void ModuleScriptImporter::AddString(MonoString * name, MonoString * string)
{
	ns_importer->AddString(name, string);
}

void ModuleScriptImporter::AddInt(MonoString * name, int value)
{
	ns_importer->AddInt(name, value);
}

void ModuleScriptImporter::AddFloat(MonoString * name, float value)
{
	ns_importer->AddFloat(name, value);
}

MonoString* ModuleScriptImporter::GetString(MonoString * name)
{
	return ns_importer->GetString(name);
}

int ModuleScriptImporter::GetInt(MonoString * name)
{
	return ns_importer->GetInt(name);
}

float ModuleScriptImporter::GetFloat(MonoString * name)
{
	return ns_importer->GetFloat(name);
}

MonoObject * ModuleScriptImporter::GetForward(MonoObject * object)
{
	return ns_importer->GetForward(object);
}

MonoObject * ModuleScriptImporter::GetRight(MonoObject * object)
{
	return ns_importer->GetRight(object);
}

MonoObject * ModuleScriptImporter::GetUp(MonoObject * object)
{
	return ns_importer->GetUp(object);
}

void ModuleScriptImporter::RotateAroundAxis(MonoObject * object, MonoObject * axis, float angle)
{
	ns_importer->RotateAroundAxis(object, axis, angle);
}

void ModuleScriptImporter::SetIncrementalRotation(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetIncrementalRotation(object, vector3);
}

void ModuleScriptImporter::StartFactory(MonoObject * object)
{
	ns_importer->StartFactory(object);
}

MonoObject * ModuleScriptImporter::Spawn(MonoObject * object)
{
	return ns_importer->Spawn(object);
}

void ModuleScriptImporter::SetSpawnPosition(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetSpawnPosition(object, vector3);
}

void ModuleScriptImporter::SetSpawnRotation(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetSpawnRotation(object, vector3);
}

void ModuleScriptImporter::SetSpawnScale(MonoObject * object, MonoObject * vector3)
{
	ns_importer->SetSpawnScale(object, vector3);
}

MonoObject * ModuleScriptImporter::ToQuaternion(MonoObject * object)
{
	return ns_importer->ToQuaternion(object);
}

MonoObject * ModuleScriptImporter::ToEulerAngles(MonoObject * object)
{
	return ns_importer->ToEulerAngles(object);
}

void ModuleScriptImporter::SetTimeScale(MonoObject * object, float scale)
{
	ns_importer->SetTimeScale(object, scale);
}

float ModuleScriptImporter::GetTimeScale()
{
	return ns_importer->GetTimeScale();
}

float ModuleScriptImporter::GetDeltaTime()
{
	return ns_importer->GetDeltaTime();
}

mono_bool ModuleScriptImporter::IsKeyDown(MonoString * key_name)
{
	return ns_importer->IsKeyDown(key_name);
}

mono_bool ModuleScriptImporter::IsKeyUp(MonoString * key_name)
{
	return ns_importer->IsKeyUp(key_name);
}

mono_bool ModuleScriptImporter::IsKeyRepeat(MonoString * key_name)
{
	return ns_importer->IsKeyRepeat(key_name);
}

mono_bool ModuleScriptImporter::IsMouseDown(int mouse_button)
{
	return ns_importer->IsMouseDown(mouse_button);
}

mono_bool ModuleScriptImporter::IsMouseUp(int mouse_button)
{
	return ns_importer->IsMouseUp(mouse_button);
}

mono_bool ModuleScriptImporter::IsMouseRepeat(int mouse_button)
{
	return ns_importer->IsMouseRepeat(mouse_button);
}

MonoObject * ModuleScriptImporter::GetMousePosition()
{
	return ns_importer->GetMousePosition();
}

int ModuleScriptImporter::GetMouseXMotion()
{
	return ns_importer->GetMouseXMotion();
}

int ModuleScriptImporter::GetMouseYMotion()
{
	return ns_importer->GetMouseYMotion();
}

int ModuleScriptImporter::GetControllerJoystickMove(int pad, MonoString * axis)
{
	return ns_importer->GetControllerJoystickMove(pad, axis);
}

int ModuleScriptImporter::GetControllerButton(int pad, MonoString * button)
{
	return ns_importer->GetControllerButton(pad, button);
}

void ModuleScriptImporter::RumbleController(int pad, float strength, int ms)
{
	ns_importer->RumbleController(pad, strength, ms);
}

void ModuleScriptImporter::Log(MonoObject * object)
{
	MonoObject* exception = nullptr;
	if (object != nullptr)
	{
		MonoString* str = mono_object_to_string(object, &exception);
		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}
		else
		{
			const char* message = mono_string_to_utf8(str);
			CONSOLE_LOG("%s", message);
		}
	}
	else
	{
		CONSOLE_ERROR("Trying to print a null argument!");
	}
}

void ModuleScriptImporter::Warning(MonoObject * object)
{
	MonoObject* exception = nullptr;
	if (object != nullptr)
	{
		MonoString* str = mono_object_to_string(object, &exception);
		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}
		else
		{
			const char* message = mono_string_to_utf8(str);
			CONSOLE_WARNING("%s", message);
		}
	}
	else
	{
		CONSOLE_ERROR("Trying to print a null argument!");
	}
}

void ModuleScriptImporter::Error(MonoObject * object)
{
	MonoObject* exception = nullptr;
	if (object != nullptr)
	{
		MonoString* str2 = mono_object_to_string(object, &exception);
		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}
		else
		{
			const char* message = mono_string_to_utf8(str2);
			CONSOLE_ERROR("%s", message);
		}
	}
	else
	{
		CONSOLE_ERROR("Trying to print a null argument!");
	}
}

bool ModuleScriptImporter::IsMuted()
{
	return ns_importer->IsMuted();
}

void ModuleScriptImporter::SetMute(bool set)
{
	ns_importer->SetMute(set);
}

int ModuleScriptImporter::GetVolume() 
{
	return ns_importer->GetVolume();
}

int ModuleScriptImporter::GetPitch()
{
	return ns_importer->GetPitch();
}

void ModuleScriptImporter::SetPitch(int pitch)
{
	ns_importer->SetPitch(pitch);
}

void ModuleScriptImporter::SetRTPCvalue(MonoString* name, float value)
{
	ns_importer->SetRTPCvalue(name, value);
}

bool ModuleScriptImporter::Play(MonoObject * object, MonoString * name)
{
	return ns_importer->Play(object, name);
}

bool ModuleScriptImporter::Stop(MonoObject * object, MonoString * name)
{
	return ns_importer->Stop(object, name);
}

bool ModuleScriptImporter::Send(MonoObject * object, MonoString * name)
{
	return ns_importer->Send(object, name);
}

bool ModuleScriptImporter::SetMyRTPCvalue(MonoObject * object, MonoString* name, float value)
{
	return ns_importer->SetMyRTPCvalue(object, name, value);
}

void ModuleScriptImporter::SetState(MonoObject* object, MonoString* group, MonoString* state)
{
	ns_importer->SetState(object, group, state);
}

void ModuleScriptImporter::SetVolume(MonoObject* object, int value)
{
	ns_importer->SetVolume(object, value);
}

void  ModuleScriptImporter::PlayEmmiter(MonoObject * object)
{
	ns_importer->PlayEmmiter(object); 
}
void  ModuleScriptImporter::StopEmmiter(MonoObject * object)
{
	ns_importer->StopEmmiter(object);
}

void ModuleScriptImporter::SetLinearVelocity(MonoObject * object, float x, float y, float z)
{
	ns_importer->SetLinearVelocity(object, x, y, z);
}

void ModuleScriptImporter::SetAngularVelocity(MonoObject * object, float x, float y, float z)
{
	ns_importer->SetAngularVelocity(object, x, y, z);
}

void ModuleScriptImporter::AddTorque(MonoObject * object, float x, float y, float z, int force_type)
{
	ns_importer->AddTorque(object, x, y, z, force_type);
}

void ModuleScriptImporter::DisableCollider(MonoObject * object, int index)
{
	ns_importer->DisableCollider(object, index);
}

void ModuleScriptImporter::DisableAllColliders(MonoObject * object)
{
	ns_importer->DisableAllColliders(object);
}

void ModuleScriptImporter::EnableCollider(MonoObject * object, int index)
{
	ns_importer->EnableCollider(object, index);
}

void ModuleScriptImporter::EnableAllColliders(MonoObject * object)
{
	ns_importer->EnableAllColliders(object);
}

bool ModuleScriptImporter::IsKinematic(MonoObject * object)
{
	return ns_importer->IsKinematic(object);
}

bool ModuleScriptImporter::IsTransformGO(MonoObject * object)
{
	return ns_importer->IsTransformGO(object);
}

void ModuleScriptImporter::SetTransformGO(MonoObject * object, bool transform_go)
{
	ns_importer->SetTransformGO(object, transform_go);
}

void ModuleScriptImporter::SetKinematic(MonoObject * object, bool kinematic)
{
	ns_importer->SetKinematic(object, kinematic);
}

void ModuleScriptImporter::SetRBPosition(MonoObject * object, float x, float y, float z)
{
	ns_importer->SetRBPosition(object, x, y, z);
}

void ModuleScriptImporter::SetRBRotation(MonoObject * object, float x, float y, float z)
{
	ns_importer->SetRBRotation(object, x, y, z);
}

mono_bool ModuleScriptImporter::GetBlackboardVariableB(MonoObject * object, MonoString * name)
{
	return ns_importer->GetBlackboardVariableB(object, name);
}

float ModuleScriptImporter::GetBlackboardVariableF(MonoObject * object, MonoString * name)
{
	return ns_importer->GetBlackboardVariableF(object, name);
}

int ModuleScriptImporter::GetNumGoals(MonoObject * object)
{
	return ns_importer->GetNumGoals(object);
}

MonoString * ModuleScriptImporter::GetGoalName(MonoObject * object, int index)
{
	return ns_importer->GetGoalName(object, index);
}

MonoString * ModuleScriptImporter::GetGoalConditionName(MonoObject * object, int index)
{
	return ns_importer->GetGoalConditionName(object, index);
}

void ModuleScriptImporter::SetGoalPriority(MonoObject * object, int index, int priority)
{
	ns_importer->SetGoalPriority(object, index, priority);
}

int ModuleScriptImporter::GetGoalPriority(MonoObject * object, int index)
{
	return ns_importer->GetGoalPriority(object, index);
}

void ModuleScriptImporter::CompleteAction(MonoObject * object)
{
	ns_importer->CompleteAction(object);
}

void ModuleScriptImporter::FailAction(MonoObject * object)
{
	ns_importer->FailAction(object);
}

void ModuleScriptImporter::SetBlackboardVariable(MonoObject * object, MonoString * name, float value)
{
	ns_importer->SetBlackboardVariable(object, name, value);
}

void ModuleScriptImporter::SetBlackboardVariableB(MonoObject * object, MonoString * name, bool value)
{
	ns_importer->SetBlackboardVariable(object, name, value);
}

int ModuleScriptImporter::RandomInt(MonoObject * object)
{
	return ns_importer->RandomInt(object);
}

float ModuleScriptImporter::RandomFloat(MonoObject * object)
{
	return ns_importer->RandomFloat(object);
}

float ModuleScriptImporter::RandomRange(MonoObject * object, float min, float max)
{
	return ns_importer->RandomRange(object, min, max);
}

void ModuleScriptImporter::LoadScene(MonoString * scene_name)
{
	ns_importer->LoadScene(scene_name);
}

void ModuleScriptImporter::Quit()
{
	ns_importer->Quit();
}

void ModuleScriptImporter::SetBoolField(MonoObject * object, MonoString * field_name, bool value)
{
	ns_importer->SetBoolField(object, field_name, value);
}

bool ModuleScriptImporter::GetBoolField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetBoolField(object, field_name);
}

void ModuleScriptImporter::SetIntField(MonoObject * object, MonoString * field_name, int value)
{
	ns_importer->SetIntField(object, field_name, value);
}

int ModuleScriptImporter::GetIntField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetIntField(object, field_name);
}

void ModuleScriptImporter::SetFloatField(MonoObject * object, MonoString * field_name, float value)
{
	ns_importer->SetFloatField(object, field_name, value);
}

float ModuleScriptImporter::GetFloatField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetFloatField(object, field_name);
}

void ModuleScriptImporter::SetDoubleField(MonoObject * object, MonoString * field_name, double value)
{
	ns_importer->SetDoubleField(object, field_name, value);
}

double ModuleScriptImporter::GetDoubleField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetDoubleField(object, field_name);
}

void ModuleScriptImporter::SetStringField(MonoObject * object, MonoString * field_name, MonoString * value)
{
	ns_importer->SetStringField(object, field_name, value);
}

MonoString * ModuleScriptImporter::GetStringField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetStringField(object, field_name);
}

void ModuleScriptImporter::SetGameObjectField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	ns_importer->SetGameObjectField(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetGameObjectField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetGameObjectField(object, field_name);
}

void ModuleScriptImporter::SetVector3Field(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	ns_importer->SetVector3Field(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetVector3Field(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetVector3Field(object, field_name);
}

void ModuleScriptImporter::SetQuaternionField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	ns_importer->SetQuaternionField(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetQuaternionField(MonoObject * object, MonoString * field_name)
{
	return ns_importer->GetQuaternionField(object, field_name);
}

void ModuleScriptImporter::CallFunction(MonoObject * object, MonoString * function_name)
{
	ns_importer->CallFunction(object, function_name);
}


/////////// Non Static Class Defs //////////////

void NSScriptImporter::AddCreatedGameObjectToList(MonoObject* object, GameObject * go)
{
	if (go && object)
	{
		created_gameobjects[object] = go;
	}
}

GameObject * NSScriptImporter::GetGameObjectFromMonoObject(MonoObject * object)
{
	if (object)
	{
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (object == it->first)
			{
				return it->second;
			}
		}
	}
	
	return nullptr;
}

Component * NSScriptImporter::GetComponentFromMonoObject(MonoObject * object)
{
	if (object)
	{
		for (std::map<MonoObject*, Component*>::iterator it = created_components.begin(); it != created_components.end(); it++)
		{
			if (object == it->first)
			{
				return it->second;
			}
		}
	}
	
	return nullptr;
}

MonoObject * NSScriptImporter::GetMonoObjectFromGameObject(GameObject * go)
{
	if (go)
	{
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (go == it->second)
			{
				return it->first;
			}
		}
	}
	return nullptr;
}

MonoObject * NSScriptImporter::GetMonoObjectFromComponent(Component * component)
{
	if (component)
	{
		for (std::map<MonoObject*, Component*>::iterator it = created_components.begin(); it != created_components.end(); it++)
		{
			if (component == it->second)
			{
				return it->first;
			}
		}
	}
	return nullptr;
}

void NSScriptImporter::SetGameObjectName(MonoObject * object, MonoString* name)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go && name)
	{
		const char* new_name = mono_string_to_utf8(name);
		go->SetName(new_name);
		App->scene->RenameDuplicatedGameObject(go);
	}
}

MonoString* NSScriptImporter::GetGameObjectName(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetName().c_str());
}

void NSScriptImporter::SetGameObjectTag(MonoObject * object, MonoString * tag)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go && tag)
	{
		const char* new_tag = mono_string_to_utf8(tag);
		go->SetName(new_tag);
	}
}

MonoString * NSScriptImporter::GetGameObjectTag(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetTag().c_str());
}

void NSScriptImporter::SetGameObjectLayer(MonoObject * object, MonoString * layer)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (layer != nullptr)
	{
		const char* new_layer = mono_string_to_utf8(layer);
		go->SetLayer(new_layer);
	}
}

MonoString * NSScriptImporter::GetGameObjectLayer(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (!go) return nullptr;
	return mono_string_new(App->script_importer->GetDomain(), go->GetLayer().c_str());
}

void NSScriptImporter::SetGameObjectStatic(MonoObject * object, mono_bool value)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetStatic(value);
	}
}

mono_bool NSScriptImporter::GameObjectIsStatic(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->IsStatic();
	}
	return false;
}

MonoObject * NSScriptImporter::DuplicateGameObject(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);
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
				created_gameobjects[new_object] = duplicated;
				return new_object;
			}
		}
	}

	return nullptr;
}

void NSScriptImporter::SetGameObjectParent(MonoObject * object, MonoObject * parent)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetParent(GetGameObjectFromMonoObject(object));
	}
}

MonoObject* NSScriptImporter::GetGameObjectParent(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		GameObject* parent = go->GetParent();
		if (parent)
		{
			MonoObject* parent_mono_object = GetMonoObjectFromGameObject(parent);
			if (!parent_mono_object)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						created_gameobjects[new_object] = parent;
						return new_object;
					}
				}
			}
			return parent_mono_object;
		}
	}

	return nullptr;
}

MonoObject * NSScriptImporter::GetGameObjectChild(MonoObject * object, int index)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

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
						created_gameobjects[new_object] = *it;
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

MonoObject * NSScriptImporter::GetGameObjectChildString(MonoObject * object, MonoString * name)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

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
						created_gameobjects[new_object] = *it;
						return new_object;
					}
				}
			}
		}
	}
	

	return nullptr;
}

int NSScriptImporter::GetGameObjectChildCount(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->childs.size();
	}

	return -1;
}

MonoObject * NSScriptImporter::FindGameObject(MonoString * gameobject_name)
{
	const char* s_name = mono_string_to_utf8(gameobject_name);

	GameObject* go = App->scene->FindGameObjectByName(s_name);

	if (go)
	{
		for (std::map<MonoObject*, GameObject*>::iterator it = created_gameobjects.begin(); it != created_gameobjects.end(); it++)
		{
			if (it->second == go) return it->first;
		}

		MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
			if (new_object)
			{
				created_gameobjects[new_object] = go;
				return new_object;
			}
		}
	}
	else
	{
		CONSOLE_ERROR("Find: Cannot find gameobject %s", s_name);
	}

	return nullptr;
}

MonoArray * NSScriptImporter::GetSceneGameObjects(MonoObject * object)
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
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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

MonoArray * NSScriptImporter::GetObjectsInFrustum(MonoObject * pos, MonoObject * front, MonoObject * up, float nearPlaneDist, float farPlaneDist)
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
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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

MonoArray * NSScriptImporter::GetAllChilds(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

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
						MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
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
	}

	return nullptr;
}

void NSScriptImporter::SetComponentActive(MonoObject * object, bool active)
{
	Component* cmp = GetComponentFromMonoObject(object); 
	cmp->SetActive(active); 
}

MonoObject* NSScriptImporter::AddComponent(MonoObject * object, MonoReflectionType * type)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

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
					created_components[new_object] = comp;
					return new_object;
				}
			}
		}
	}

	return nullptr;
}

MonoObject* NSScriptImporter::GetComponent(MonoObject * object, MonoReflectionType * type, int index)
{

	GameObject* go = GetGameObjectFromMonoObject(object);

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

		Component::ComponentType cpp_type = CsToCppComponent(comp_name);

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
						for (std::map<MonoObject*, Component*>::iterator it = created_components.begin(); it != created_components.end(); it++)
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
								created_components[new_object] = comp;
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

void NSScriptImporter::DestroyComponent(MonoObject* object, MonoObject* cmp)
{
	Component* comp = GetComponentFromMonoObject(cmp);
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (comp != nullptr && go != nullptr)
	{
		go->DestroyComponent(comp);
	}
}

void NSScriptImporter::SetPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject* NSScriptImporter::GetPosition(MonoObject * object, mono_bool is_global)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetRotation(MonoObject * object, mono_bool is_global)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetScale(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetScale(MonoObject * object, mono_bool is_global)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::LookAt(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetRectPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetRectPosition(MonoObject * object)
{
	return nullptr;
}

MonoObject * NSScriptImporter::GetForward(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetRectRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetRectRotation(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetRectSize(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetRectSize(MonoObject * object)
{
	return nullptr;
}

MonoObject * NSScriptImporter::GetRight(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetRectAnchor(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetRectAnchor(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnClick(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnClickDown(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnClickUp(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnMouseEnter(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnMouseOver(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

mono_bool NSScriptImporter::GetOnMouseOut(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetText(MonoObject * object, MonoString* t)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* txt = mono_string_to_utf8(t);

		ComponentText* text = (ComponentText*)comp;
		if (text)
			text->SetText(txt);
	}
}

MonoString* NSScriptImporter::GetText(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentText* text = (ComponentText*)comp;
		return mono_string_new(App->script_importer->GetDomain(), text->GetText().c_str());
	}
	return mono_string_new(App->script_importer->GetDomain(), "");
}

MonoObject * NSScriptImporter::GetUp(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::RotateAroundAxis(MonoObject * object, MonoObject * axis, float angle)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetIncrementalRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetPercentageProgress(MonoObject * object, float progress)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentProgressBar* progres_barr = (ComponentProgressBar*)comp;

		progres_barr->SetProgressPercentage(progress);
	}
}

float NSScriptImporter::GetPercentageProgress(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentProgressBar* progres_barr = (ComponentProgressBar*)comp;
		return progres_barr->GetProgressPercentage();
	}
	return 0.0f;
}

void NSScriptImporter::AddEntity(MonoObject * object, MonoObject * game_object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = GetGameObjectFromMonoObject(game_object);

		if (radar != nullptr)
		{
			if (go != nullptr)
			{
				radar->AddEntity(go);
			}
		}
	}
}

void NSScriptImporter::RemoveEntity(MonoObject * object, MonoObject * game_object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = GetGameObjectFromMonoObject(game_object);

		if (radar != nullptr)
		{
			if (go != nullptr)
			{
				radar->RemoveEntity(go);
			}
		}
	}
}

void NSScriptImporter::RemoveAllEntities(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		if (radar != nullptr)
		{
			radar->RemoveAllEntities();
		}
	}
}

void NSScriptImporter::SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString * marker_name)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRadar* radar = (ComponentRadar*)comp;

		GameObject* go = GetGameObjectFromMonoObject(game_object);

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

void NSScriptImporter::StartFactory(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentFactory* factory = (ComponentFactory*)comp;
		if (factory) factory->StartFactory();
	}
}

MonoObject * NSScriptImporter::Spawn(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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
			MonoObject* obj = GetMonoObjectFromGameObject(go);
			if (!obj)
			{
				MonoClass* c = mono_class_from_name(App->script_importer->GetEngineImage(), "TheEngine", "TheGameObject");
				if (c)
				{
					MonoObject* new_object = mono_object_new(App->script_importer->GetDomain(), c);
					if (new_object)
					{
						created_gameobjects[new_object] = go;
						return new_object;
					}
				}
			}
			return obj;
		}
	}
	return nullptr;
}

void NSScriptImporter::SetSpawnPosition(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetSpawnRotation(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetSpawnScale(MonoObject * object, MonoObject * vector3)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::AddString(MonoString * name, MonoString * string)
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

void NSScriptImporter::AddInt(MonoString * name, int value)
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

void NSScriptImporter::AddFloat(MonoString * name, float value)
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

MonoString* NSScriptImporter::GetString(MonoString* name)
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

int NSScriptImporter::GetInt(MonoString * name)
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

float NSScriptImporter::GetFloat(MonoString * name)
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

MonoObject * NSScriptImporter::ToQuaternion(MonoObject * object)
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

MonoObject * NSScriptImporter::ToEulerAngles(MonoObject * object)
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

void NSScriptImporter::SetTimeScale(MonoObject * object, float scale)
{
	App->time->time_scale = scale;
}

float NSScriptImporter::GetTimeScale()
{
	return App->time->time_scale;
}

float NSScriptImporter::GetDeltaTime()
{
	return App->time->GetGameDt();
}


void NSScriptImporter::Start(MonoObject * object, float time)
{
}

float NSScriptImporter::ReadTime(MonoObject * object)
{
	return 0.0f;
}

mono_bool NSScriptImporter::IsKeyDown(MonoString * key_name)
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

mono_bool NSScriptImporter::IsKeyUp(MonoString * key_name)
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

mono_bool NSScriptImporter::IsKeyRepeat(MonoString * key_name)
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

mono_bool NSScriptImporter::IsMouseDown(int mouse_button)
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

mono_bool NSScriptImporter::IsMouseUp(int mouse_button)
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

mono_bool NSScriptImporter::IsMouseRepeat(int mouse_button)
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

MonoObject * NSScriptImporter::GetMousePosition()
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

int NSScriptImporter::GetMouseXMotion()
{
	return App->input->GetMouseXMotion();
}

int NSScriptImporter::GetMouseYMotion()
{
	return App->input->GetMouseYMotion();
}

int NSScriptImporter::GetControllerJoystickMove(int pad, MonoString * axis)
{
	const char* key = mono_string_to_utf8(axis);
	JOYSTICK_MOVES code = App->input->StringToJoyMove(key);
	return App->input->GetControllerJoystickMove(pad, code);
}

int NSScriptImporter::GetControllerButton(int pad, MonoString * button)
{
	const char* key = mono_string_to_utf8(button);
	SDL_Keycode code = App->input->StringToKey(key);
	return App->input->GetControllerButton(pad, code);
}

void NSScriptImporter::RumbleController(int pad, float strength, int ms)
{
	App->input->RumbleController(pad, strength, ms);
}

void NSScriptImporter::CreateGameObject(MonoObject * object)
{
	/*if (!inside_function)
	{
		CONSOLE_ERROR("Can't create new GameObjects outside a function.");
		return;
	}*/
	GameObject* gameobject = App->scene->CreateGameObject();
	created_gameobjects[object] = gameobject;
}

MonoObject* NSScriptImporter::GetSelfGameObject()
{
	if (current_script)
	{
		return current_script->mono_self_object;
	}
	return nullptr;
}

void NSScriptImporter::SetGameObjectActive(MonoObject * object, mono_bool active)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		go->SetActive(active);
	}
}

mono_bool NSScriptImporter::GetGameObjectIsActive(MonoObject * object)
{
	GameObject* go = GetGameObjectFromMonoObject(object);

	if (go)
	{
		return go->IsActive();
	}
	return false;
}

bool NSScriptImporter::IsMuted()
{
	return App->audio->IsMuted();
}

void NSScriptImporter::SetMute(bool set)
{
	App->audio->SetMute(set);
}

int NSScriptImporter::GetVolume()
{
	return App->audio->GetVolume();
}

void NSScriptImporter::SetVolume(MonoObject* obj, int volume)
{
	Component* comp = GetComponentFromMonoObject(obj);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		as->volume = volume;
	}
}

int NSScriptImporter::GetPitch()
{
	return App->audio->GetPitch();
}

void NSScriptImporter::SetPitch(int pitch)
{
	App->audio->SetPitch(pitch);
}

void NSScriptImporter::SetRTPCvalue(MonoString* name, float value)
{
	const char* new_name = mono_string_to_utf8(name);
	App->audio->SetRTPCvalue(new_name, value);
}

bool NSScriptImporter::Play(MonoObject * object, MonoString* name)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);
		return as->PlayEvent(event_name);
	}
	return false;
}

bool NSScriptImporter::Stop(MonoObject * object, MonoString* name)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);

		return as->StopEvent(event_name);
	}
	return false;
}

bool NSScriptImporter::Send(MonoObject * object, MonoString* name)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		const char* event_name = mono_string_to_utf8(name);

		return as->SendEvent(event_name);
	}
	return false;
}

bool NSScriptImporter::SetMyRTPCvalue(MonoObject * object, MonoString* name, float value)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* new_name = mono_string_to_utf8(name);
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		return as->obj->SetRTPCvalue(new_name, value);
	}
	return false;
}

bool NSScriptImporter::SetState(MonoObject* object, MonoString* group, MonoString* state)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		const char* group_name = mono_string_to_utf8(group);
		const char* state_name = mono_string_to_utf8(state);
		ComponentAudioSource* as = (ComponentAudioSource*)comp;
		as->SetState(group_name, state_name);
	}
	return true;
}

void NSScriptImporter::PlayEmmiter(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)comp;

		if (emmiter != nullptr)
			emmiter->PlayEmmiter();
	}
}

void NSScriptImporter::StopEmmiter(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentParticleEmmiter* emmiter = (ComponentParticleEmmiter*)comp;

		if (emmiter != nullptr)
			emmiter->StopEmmiter();
	}
}

void NSScriptImporter::SetLinearVelocity(MonoObject * object, float x, float y, float z)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetLinearVelocity({ x,y,z });
	}
}

void NSScriptImporter::SetAngularVelocity(MonoObject * object, float x, float y, float z)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetAngularVelocity({ x,y,z });
	}
}

void NSScriptImporter::AddTorque(MonoObject* object, float x, float y, float z, int force_type)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::DisableCollider(MonoObject * object, int index)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->DisableShapeByIndex(index);
	}
}

void NSScriptImporter::DisableAllColliders(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->DisableShapes();
	}
}

void NSScriptImporter::EnableCollider(MonoObject * object, int index)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->EnableShapeByIndex(index);
	}
}

void NSScriptImporter::EnableAllColliders(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->EnableShapes();
	}
}

void NSScriptImporter::SetKinematic(MonoObject * object, bool kinematic)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetKinematic(kinematic);
	}
}

void NSScriptImporter::SetTransformGO(MonoObject * object, bool transform_go)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetTransformsGo(transform_go);
	}
}

bool NSScriptImporter::IsKinematic(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			return rb->IsKinematic();
	}

	return false; 
}

bool NSScriptImporter::IsTransformGO(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			return rb->GetTransformsGo();
	}

	return false; 
}

void NSScriptImporter::SetRBPosition(MonoObject * object, float x, float y, float z)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetPosition({ x,y,z });
	}
}

void NSScriptImporter::SetRBRotation(MonoObject * object, float x, float y, float z)
{
	Component* comp = GetComponentFromMonoObject(object);

	if (comp)
	{
		ComponentRigidBody* rb = (ComponentRigidBody*)comp;

		if (rb != nullptr)
			rb->SetRotation({ x,y,z });
	}
}

// ----- GOAP AGENT -----
mono_bool NSScriptImporter::GetBlackboardVariableB(MonoObject * object, MonoString * name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

float NSScriptImporter::GetBlackboardVariableF(MonoObject * object, MonoString * name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

int NSScriptImporter::GetNumGoals(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoString * NSScriptImporter::GetGoalName(MonoObject * object, int index)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoString * NSScriptImporter::GetGoalConditionName(MonoObject * object, int index)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetGoalPriority(MonoObject * object, int index, int priority)
{
	Component* comp = GetComponentFromMonoObject(object);

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

int NSScriptImporter::GetGoalPriority(MonoObject * object, int index)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::CompleteAction(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::FailAction(MonoObject * object)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetBlackboardVariable(MonoObject * object, MonoString * name, float value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetBlackboardVariable(MonoObject * object, MonoString * name, bool value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

int NSScriptImporter::RandomInt(MonoObject * object)
{
	return App->RandomNumber().Int();
}

float NSScriptImporter::RandomFloat(MonoObject * object)
{
	return App->RandomNumber().Float();
}

float NSScriptImporter::RandomRange(MonoObject * object, float min, float max)
{
	return App->RandomNumber().FloatIncl(min, max);;
}


void NSScriptImporter::LoadScene(MonoString * scene_name)
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

void NSScriptImporter::Quit()
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

void NSScriptImporter::SetBoolField(MonoObject * object, MonoString * field_name, bool value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

bool NSScriptImporter::GetBoolField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetIntField(MonoObject * object, MonoString * field_name, int value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

int NSScriptImporter::GetIntField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetFloatField(MonoObject * object, MonoString * field_name, float value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

float NSScriptImporter::GetFloatField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetDoubleField(MonoObject * object, MonoString * field_name, double value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

double NSScriptImporter::GetDoubleField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetStringField(MonoObject * object, MonoString * field_name, MonoString * value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoString * NSScriptImporter::GetStringField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetGameObjectField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = GetComponentFromMonoObject(object);

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
}

MonoObject * NSScriptImporter::GetGameObjectField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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
				MonoObject* mono_object = GetMonoObjectFromGameObject(go);
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
							created_gameobjects[new_object] = go;
							return new_object;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

void NSScriptImporter::SetVector3Field(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetVector3Field(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::SetQuaternionField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	Component* comp = GetComponentFromMonoObject(object);

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

MonoObject * NSScriptImporter::GetQuaternionField(MonoObject * object, MonoString * field_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

void NSScriptImporter::CallFunction(MonoObject * object, MonoString * function_name)
{
	Component* comp = GetComponentFromMonoObject(object);

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

Component::ComponentType NSScriptImporter::CsToCppComponent(std::string component_type)
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
	else if (component_type == "TheMeshCollider")
	{
		type = Component::CompMeshCollider;
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