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

CSScript* ModuleScriptImporter::current_script = nullptr;
bool ModuleScriptImporter::inside_function = false;

ModuleScriptImporter::ModuleScriptImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Script_Importer";
	mono_domain = nullptr;
	mono_engine_image = nullptr;
}

ModuleScriptImporter::~ModuleScriptImporter()
{
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

void ModuleScriptImporter::SetCurrentScript(CSScript * script)
{
	current_script = script;
}

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

	for (int i = 1; i < rows; i++) {
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
	mono_add_internal_call("TheEngine.TheGameObject::GetComponent", (const void*)GetComponent);
	mono_add_internal_call("TheEngine.TheGameObject::Find", (const void*)FindGameObject);
	mono_add_internal_call("TheEngine.TheGameObject::GetSceneGameObjects", (const void*)GetSceneGameObjects);
	mono_add_internal_call("TheEngine.TheGameObject::GetObjectsInFrustum", (const void*)GetObjectsInFrustum);

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

	mono_add_internal_call("TheEngine.TheVector3::ToQuaternion", (const void*)ToQuaternion);

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
	mono_add_internal_call("TheEngine.TheAudio::SetVolume", (const void*)SetVolume);
	mono_add_internal_call("TheEngine.TheAudio::GetPitch", (const void*)GetPitch);
	mono_add_internal_call("TheEngine.TheAudio::SetPitch", (const void*)SetPitch);

	mono_add_internal_call("TheEngine.TheAudio::SetRTPCvalue", (const void*)SetRTPCvalue);

	mono_add_internal_call("TheEngine.TheAudioSource::Play", (const void*)Play);
	mono_add_internal_call("TheEngine.TheAudioSource::Stop", (const void*)Stop);
	mono_add_internal_call("TheEngine.TheAudioSource::Send", (const void*)Send);
	mono_add_internal_call("TheEngine.TheAudioSource::SetMyRTPCvalue", (const void*)SetMyRTPCvalue);
	mono_add_internal_call("TheEngine.TheAudioSource::SetState", (const void*)SetState);

	//EMITER
	mono_add_internal_call("TheEngine.TheParticleEmmiter::Play", (const void*)PlayEmmiter);
	mono_add_internal_call("TheEngine.TheParticleEmmiter::Stop", (const void*)StopEmmiter);

	//RIGIDBODY
	mono_add_internal_call("TheEngine.TheRigidBody::SetLinearVelocity", (const void*)SetLinearVelocity);
	mono_add_internal_call("TheEngine.TheRigidBody::SetPosition", (const void*)SetRBPosition);
	mono_add_internal_call("TheEngine.TheRigidBody::SetRotation", (const void*)SetRBRotation);

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
}

void ModuleScriptImporter::SetGameObjectName(MonoObject * object, MonoString * name)
{
	current_script->SetGameObjectName(object, name);
}

MonoString* ModuleScriptImporter::GetGameObjectName(MonoObject * object)
{
	return current_script->GetGameObjectName(object);
}

void ModuleScriptImporter::SetGameObjectActive(MonoObject * object, mono_bool active)
{
	current_script->SetGameObjectActive(object, active);
}

mono_bool ModuleScriptImporter::GetGameObjectIsActive(MonoObject * object)
{
	return current_script->GetGameObjectIsActive(object);
}

void ModuleScriptImporter::CreateGameObject(MonoObject * object)
{
	current_script->CreateGameObject(object);
}

MonoObject* ModuleScriptImporter::GetSelfGameObject()
{
	return current_script->GetSelfGameObject();
}

void ModuleScriptImporter::SetGameObjectTag(MonoObject * object, MonoString * name)
{
	current_script->SetGameObjectTag(object, name);
}

MonoString* ModuleScriptImporter::GetGameObjectTag(MonoObject * object)
{
	return current_script->GetGameObjectTag(object);
}

void ModuleScriptImporter::SetGameObjectLayer(MonoObject * object, MonoString * layer)
{
	current_script->SetGameObjectLayer(object, layer);
}

MonoString * ModuleScriptImporter::GetGameObjectLayer(MonoObject * object)
{
	return current_script->GetGameObjectLayer(object);
}

void ModuleScriptImporter::SetGameObjectStatic(MonoObject * object, mono_bool value)
{
	current_script->SetGameObjectStatic(object, value);
}

mono_bool ModuleScriptImporter::GameObjectIsStatic(MonoObject * object)
{
	return current_script->GameObjectIsStatic(object);
}

MonoObject * ModuleScriptImporter::DuplicateGameObject(MonoObject * object)
{
	return current_script->DuplicateGameObject(object);
}

void ModuleScriptImporter::SetGameObjectParent(MonoObject * object, MonoObject * parent)
{
	current_script->SetGameObjectParent(object, parent);
}

//MonoObject* ModuleScriptImporter::GetGameObjectParent(MonoObject * object)
//{
//	return current_script->GetGameObjectParent(object);
//}

MonoObject * ModuleScriptImporter::GetGameObjectChild(MonoObject * object, int index)
{
	return current_script->GetGameObjectChild(object, index);
}

MonoObject * ModuleScriptImporter::GetGameObjectChildString(MonoObject * object, MonoString * name)
{
	return current_script->GetGameObjectChildString(object, name);
}

int ModuleScriptImporter::GetGameObjectChildCount(MonoObject * object)
{
	return current_script->GetGameObjectChildCount(object);
}

MonoObject * ModuleScriptImporter::FindGameObject(MonoString * gameobject_name)
{
	return current_script->FindGameObject(gameobject_name);
}

MonoArray * ModuleScriptImporter::GetSceneGameObjects(MonoObject * object)
{
	return current_script->GetSceneGameObjects(object);
}

MonoArray * ModuleScriptImporter::GetObjectsInFrustum(MonoObject * pos, MonoObject* front, MonoObject* up, float nearPlaneDist, float farPlaneDist )
{
	return current_script->GetObjectsInFrustum(pos, front, up, nearPlaneDist, farPlaneDist);
}

MonoObject* ModuleScriptImporter::AddComponent(MonoObject * object, MonoReflectionType* type)
{
	return current_script->AddComponent(object, type);
}

MonoObject* ModuleScriptImporter::GetComponent(MonoObject * object, MonoReflectionType * type, int index)
{
	return current_script->GetComponent(object, type, index);
}

void ModuleScriptImporter::SetPosition(MonoObject * object, MonoObject * vector3)
{
	current_script->SetPosition(object, vector3);
}

MonoObject* ModuleScriptImporter::GetPosition(MonoObject * object, mono_bool is_global)
{
	return current_script->GetPosition(object, is_global);
}

void ModuleScriptImporter::SetRotation(MonoObject * object, MonoObject * vector)
{
	current_script->SetRotation(object, vector);
}

MonoObject * ModuleScriptImporter::GetRotation(MonoObject * object, mono_bool is_global)
{
	return current_script->GetRotation(object, is_global);
}

void ModuleScriptImporter::SetScale(MonoObject * object, MonoObject * vector)
{
	current_script->SetScale(object, vector);
}

MonoObject * ModuleScriptImporter::GetScale(MonoObject * object, mono_bool is_global)
{
	return current_script->GetScale(object, is_global);
}

void ModuleScriptImporter::LookAt(MonoObject * object, MonoObject * vector)
{
	current_script->LookAt(object, vector);
}

void ModuleScriptImporter::SetRectPosition(MonoObject * object, MonoObject * vector3)
{
	current_script->SetRectPosition(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectPosition(MonoObject * object)
{
	return current_script->GetRectPosition(object);
}

void ModuleScriptImporter::SetRectRotation(MonoObject * object, MonoObject * vector3)
{
	current_script->SetRectRotation(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectRotation(MonoObject * object)
{
	return current_script->GetRectRotation(object);
}

void ModuleScriptImporter::SetRectSize(MonoObject * object, MonoObject * vector3)
{
	current_script->SetRectSize(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectSize(MonoObject * object)
{
	return current_script->GetRectSize(object);
}

void ModuleScriptImporter::SetRectAnchor(MonoObject * object, MonoObject * vector3)
{
	current_script->SetRectAnchor(object, vector3);
}

MonoObject * ModuleScriptImporter::GetRectAnchor(MonoObject * object)
{
	return current_script->GetRectAnchor(object);
}

mono_bool ModuleScriptImporter::GetOnClick(MonoObject * object)
{
	return current_script->GetOnClick(object);
}

mono_bool ModuleScriptImporter::GetOnClickDown(MonoObject * object)
{
	return current_script->GetOnClickDown(object);
}

mono_bool ModuleScriptImporter::GetOnClickUp(MonoObject * object)
{
	return current_script->GetOnClickUp(object);
}

mono_bool ModuleScriptImporter::GetOnMouseEnter(MonoObject * object)
{
	return current_script->GetOnMouseEnter(object);
}

mono_bool ModuleScriptImporter::GetOnMouseOver(MonoObject * object)
{
	return current_script->GetOnMouseOver(object);
}

mono_bool ModuleScriptImporter::GetOnMouseOut(MonoObject * object)
{
	return current_script->GetOnMouseOut(object);
}

void ModuleScriptImporter::SetText(MonoObject * object, MonoString* text)
{
	return current_script->SetText(object, text);
}

MonoString* ModuleScriptImporter::GetText(MonoObject * object)
{
	return current_script->GetText(object);
}

void ModuleScriptImporter::SetPercentageProgress(MonoObject * object, float progress)
{
	current_script->SetPercentageProgress(object, progress);
}

float ModuleScriptImporter::GetPercentageProgress(MonoObject * object)
{
	return current_script->GetPercentageProgress(object);
}

void ModuleScriptImporter::AddEntity(MonoObject * object, MonoObject * game_object)
{
	current_script->AddEntity(object, game_object);
}

void ModuleScriptImporter::RemoveEntity(MonoObject * object, MonoObject * game_object)
{
	current_script->RemoveEntity(object, game_object);
}

void ModuleScriptImporter::RemoveAllEntities(MonoObject * object)
{
	current_script->RemoveAllEntities(object);
}

void ModuleScriptImporter::SetMarkerToEntity(MonoObject * object, MonoObject * game_object, MonoString * marker_name)
{
	current_script->SetMarkerToEntity(object, game_object, marker_name);
}

MonoObject * ModuleScriptImporter::GetForward(MonoObject * object)
{
	return current_script->GetForward(object);
}

MonoObject * ModuleScriptImporter::GetRight(MonoObject * object)
{
	return current_script->GetRight(object);
}

MonoObject * ModuleScriptImporter::GetUp(MonoObject * object)
{
	return current_script->GetUp(object);
}

void ModuleScriptImporter::StartFactory(MonoObject * object)
{
	current_script->StartFactory(object);
}

MonoObject * ModuleScriptImporter::Spawn(MonoObject * object)
{
	return current_script->Spawn(object);
}

void ModuleScriptImporter::SetSpawnPosition(MonoObject * object, MonoObject * vector3)
{
	current_script->SetSpawnPosition(object, vector3);
}

void ModuleScriptImporter::SetSpawnRotation(MonoObject * object, MonoObject * vector3)
{
	current_script->SetSpawnRotation(object, vector3);
}

void ModuleScriptImporter::SetSpawnScale(MonoObject * object, MonoObject * vector3)
{
	current_script->SetSpawnScale(object, vector3);
}

MonoObject * ModuleScriptImporter::ToQuaternion(MonoObject * object)
{
	return current_script->ToQuaternion(object);
}

void ModuleScriptImporter::SetTimeScale(MonoObject * object, float scale)
{
	current_script->SetTimeScale(object, scale);
}

float ModuleScriptImporter::GetTimeScale()
{
	return current_script->GetTimeScale();
}

float ModuleScriptImporter::GetDeltaTime()
{
	return current_script->GetDeltaTime();
}

mono_bool ModuleScriptImporter::IsKeyDown(MonoString * key_name)
{
	return current_script->IsKeyDown(key_name);
}

mono_bool ModuleScriptImporter::IsKeyUp(MonoString * key_name)
{
	return current_script->IsKeyUp(key_name);
}

mono_bool ModuleScriptImporter::IsKeyRepeat(MonoString * key_name)
{
	return current_script->IsKeyRepeat(key_name);
}

mono_bool ModuleScriptImporter::IsMouseDown(int mouse_button)
{
	return current_script->IsMouseDown(mouse_button);
}

mono_bool ModuleScriptImporter::IsMouseUp(int mouse_button)
{
	return current_script->IsMouseUp(mouse_button);
}

mono_bool ModuleScriptImporter::IsMouseRepeat(int mouse_button)
{
	return current_script->IsMouseRepeat(mouse_button);
}

MonoObject * ModuleScriptImporter::GetMousePosition()
{
	return current_script->GetMousePosition();
}

int ModuleScriptImporter::GetMouseXMotion()
{
	return current_script->GetMouseXMotion();
}

int ModuleScriptImporter::GetMouseYMotion()
{
	return current_script->GetMouseYMotion();
}

int ModuleScriptImporter::GetControllerJoystickMove(int pad, MonoString * axis)
{
	return current_script->GetControllerJoystickMove(pad, axis);
}

int ModuleScriptImporter::GetControllerButton(int pad, MonoString * button)
{
	return current_script->GetControllerButton(pad, button);
}

void ModuleScriptImporter::RumbleController(int pad, float strength, int ms)
{
	current_script->RumbleController(pad, strength, ms);
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
	return current_script->IsMuted();
}

void ModuleScriptImporter::SetMute(bool set)
{
	current_script->SetMute(set);
}

int ModuleScriptImporter::GetVolume() 
{
	return current_script->GetVolume();
}

void ModuleScriptImporter::SetVolume(int volume)
{
	current_script->SetVolume(volume);
}

int ModuleScriptImporter::GetPitch()
{
	return current_script->GetPitch();
}

void ModuleScriptImporter::SetPitch(int pitch)
{
	current_script->SetPitch(pitch);
}

void ModuleScriptImporter::SetRTPCvalue(MonoString* name, float value)
{
	current_script->SetRTPCvalue(name, value);
}

bool ModuleScriptImporter::Play(MonoObject * object, MonoString * name)
{
	return current_script->Play(object, name);
}

bool ModuleScriptImporter::Stop(MonoObject * object, MonoString * name)
{
	return current_script->Stop(object, name);
}

bool ModuleScriptImporter::Send(MonoObject * object, MonoString * name)
{
	return current_script->Send(object, name);
}

bool ModuleScriptImporter::SetMyRTPCvalue(MonoObject * object, MonoString* name, float value)
{
	return current_script->SetMyRTPCvalue(object, name, value);
}

void ModuleScriptImporter::SetState(MonoObject* object, MonoString* group, MonoString* state)
{
	current_script->SetState(object, group, state);
}

void  ModuleScriptImporter::PlayEmmiter(MonoObject * object)
{
	current_script->PlayEmmiter(object); 
}
void  ModuleScriptImporter::StopEmmiter(MonoObject * object)
{
	current_script->StopEmmiter(object);
}

void ModuleScriptImporter::SetLinearVelocity(MonoObject * object, float x, float y, float z)
{
	current_script->SetLinearVelocity(object, x, y, z);
}

void ModuleScriptImporter::SetRBPosition(MonoObject * object, float x, float y, float z)
{
	current_script->SetRBPosition(object, x, y, z);
}

void ModuleScriptImporter::SetRBRotation(MonoObject * object, float x, float y, float z)
{
	current_script->SetRBRotation(object, x, y, z);
}

mono_bool ModuleScriptImporter::GetBlackboardVariableB(MonoString * name)
{
	return current_script->GetBlackboardVariableB(name);
}

float ModuleScriptImporter::GetBlackboardVariableF(MonoString * name)
{
	return current_script->GetBlackboardVariableF(name);
}

int ModuleScriptImporter::GetNumGoals()
{
	return current_script->GetNumGoals();
}

MonoString * ModuleScriptImporter::GetGoalName(int index)
{
	return current_script->GetGoalName(index);
}

MonoString * ModuleScriptImporter::GetGoalConditionName(int index)
{
	return current_script->GetGoalConditionName(index);
}

void ModuleScriptImporter::SetGoalPriority(int index, int priority)
{
	current_script->SetGoalPriority(index, priority);
}

int ModuleScriptImporter::GetGoalPriority(int index)
{
	return current_script->GetGoalPriority(index);
}

void ModuleScriptImporter::CompleteAction()
{
	current_script->CompleteAction();
}

void ModuleScriptImporter::FailAction()
{
	current_script->FailAction();
}

void ModuleScriptImporter::SetBlackboardVariable(MonoString * name, float value)
{
	current_script->SetBlackboardVariable(name, value);
}

void ModuleScriptImporter::SetBlackboardVariableB(MonoString * name, bool value)
{
	current_script->SetBlackboardVariable(name, value);
}

int ModuleScriptImporter::RandomInt(MonoObject * object)
{
	return current_script->RandomInt(object);
}

float ModuleScriptImporter::RandomFloat(MonoObject * object)
{
	return current_script->RandomFloat(object);
}

float ModuleScriptImporter::RandomRange(MonoObject * object, float min, float max)
{
	return current_script->RandomRange(object, min, max);
}

void ModuleScriptImporter::LoadScene(MonoString * scene_name)
{
	current_script->LoadScene(scene_name);
}

void ModuleScriptImporter::SetBoolField(MonoObject * object, MonoString * field_name, bool value)
{
	current_script->SetBoolField(object, field_name, value);
}

bool ModuleScriptImporter::GetBoolField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetBoolField(object, field_name);
}

void ModuleScriptImporter::SetIntField(MonoObject * object, MonoString * field_name, int value)
{
	current_script->SetIntField(object, field_name, value);
}

int ModuleScriptImporter::GetIntField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetIntField(object, field_name);
}

void ModuleScriptImporter::SetFloatField(MonoObject * object, MonoString * field_name, float value)
{
	current_script->SetFloatField(object, field_name, value);
}

float ModuleScriptImporter::GetFloatField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetFloatField(object, field_name);
}

void ModuleScriptImporter::SetDoubleField(MonoObject * object, MonoString * field_name, double value)
{
	current_script->SetDoubleField(object, field_name, value);
}

double ModuleScriptImporter::GetDoubleField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetDoubleField(object, field_name);
}

void ModuleScriptImporter::SetStringField(MonoObject * object, MonoString * field_name, MonoString * value)
{
	current_script->SetStringField(object, field_name, value);
}

MonoString * ModuleScriptImporter::GetStringField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetStringField(object, field_name);
}

void ModuleScriptImporter::SetGameObjectField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	current_script->SetGameObjectField(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetGameObjectField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetGameObjectField(object, field_name);
}

void ModuleScriptImporter::SetVector3Field(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	current_script->SetVector3Field(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetVector3Field(MonoObject * object, MonoString * field_name)
{
	return current_script->GetVector3Field(object, field_name);
}

void ModuleScriptImporter::SetQuaternionField(MonoObject * object, MonoString * field_name, MonoObject * value)
{
	current_script->SetQuaternionField(object, field_name, value);
}

MonoObject * ModuleScriptImporter::GetQuaternionField(MonoObject * object, MonoString * field_name)
{
	return current_script->GetQuaternionField(object, field_name);
}

void ModuleScriptImporter::CallFunction(MonoObject * object, MonoString * function_name)
{
	current_script->CallFunction(object, function_name);
}
