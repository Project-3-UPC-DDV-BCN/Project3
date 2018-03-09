#pragma once
#include "Module.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <vector>

class Script;
class CSScript;
class ComponentRectTransform;

class ModuleScriptImporter :
	public Module
{
public:
	ModuleScriptImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleScriptImporter();

	bool Init(Data* editor_config = nullptr);

	//Retuns the library path if created or an empty string
	std::string ImportScript(std::string path);
	Script* LoadScriptFromLibrary(std::string path);

	MonoDomain* GetDomain() const;
	MonoImage* GetEngineImage() const;

	std::string CompileScript(std::string assets_path);
	void SetCurrentScript(CSScript* script);

private:
	CSScript* DumpAssemblyInfo(MonoAssembly* assembly);
	MonoClass* DumpClassInfo(MonoImage* image, std::string& class_name, std::string& name_space);

	void RegisterAPI();

	//GAMEOBJECT
	static void SetGameObjectName(MonoObject * object, MonoString* name);
	static MonoString* GetGameObjectName(MonoObject* object);
	static void CreateGameObject(MonoObject * object);
	static MonoObject* GetSelfGameObject();
	static void SetGameObjectActive(MonoObject * object, mono_bool active);
	static mono_bool GetGameObjectIsActive(MonoObject* object);
	static void SetGameObjectTag(MonoObject * object, MonoString* tag);
	static MonoString* GetGameObjectTag(MonoObject* object);
	static void SetGameObjectLayer(MonoObject * object, MonoString* layer);
	static MonoString* GetGameObjectLayer(MonoObject* object);
	static void SetGameObjectStatic(MonoObject * object, mono_bool value);
	static mono_bool GameObjectIsStatic(MonoObject* object);
	static MonoObject* DuplicateGameObject(MonoObject* object);
	static void SetGameObjectParent(MonoObject* object, MonoObject* parent);
	static MonoObject* GetGameObjectChild(MonoObject* object, int index);
	static MonoObject* GetGameObjectChildString(MonoObject* object, MonoString* name);
	static int GetGameObjectChildCount(MonoObject* object);

	//COMPONENT
	static MonoObject* AddComponent(MonoObject* object, MonoReflectionType* type);
	static MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type);

	//TRANSFORM
	static void SetPosition(MonoObject * object, MonoObject * vector);
	static MonoObject* GetPosition(MonoObject* object, mono_bool is_global);
	static void SetRotation(MonoObject * object, MonoObject * vector);
	static MonoObject* GetRotation(MonoObject* object, mono_bool is_global);
	static void SetScale(MonoObject * object, MonoObject * vector);
	static MonoObject* GetScale(MonoObject* object, mono_bool is_global);
	static void LookAt(MonoObject * object, MonoObject * vector);
	static MonoObject* GetForward(MonoObject* object);
	static MonoObject* GetRight(MonoObject* object);
	static MonoObject* GetUp(MonoObject* object);

	//RECTTRANSFORM
	static void SetRectPosition(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectPosition(MonoObject * object);
	static void SetRectRotation(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectRotation(MonoObject * object);
	static void SetRectSize(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectSize(MonoObject * object);
	static void SetRectAnchor(MonoObject * object, MonoObject * vector3);
	static MonoObject* GetRectAnchor(MonoObject * object);
	void OnClick(ComponentRectTransform* rect_trans);

	// TEXT
	static void SetText(MonoObject * object, MonoString* text);
	static MonoString* GetText(MonoObject * object);

	//PROGRESSBAR
	static void SetPercentageProgress(MonoObject * object, float progress);
	static float GetPercentageProgress(MonoObject * object);

	//FACTORY
	static void StartFactory(MonoObject * object);
	static MonoObject* Spawn(MonoObject* object);
	static void SetSpawnPosition(MonoObject * object, MonoObject * vector3);
	static void SetSpawnRotation(MonoObject * object, MonoObject * vector3);
	static void SetSpawnScale(MonoObject * object, MonoObject * vector3);

	static MonoObject* ToQuaternion(MonoObject * object);

	//TIME
	static void SetTimeScale(MonoObject* object, float scale);
	static float GetTimeScale();
	static float GetDeltaTime();

	//INPUT
	static mono_bool IsKeyDown(MonoString * key_name);
	static mono_bool IsKeyUp(MonoString* key_name);
	static mono_bool IsKeyRepeat(MonoString* key_name);
	static mono_bool IsMouseDown(int mouse_button);
	static mono_bool IsMouseUp(int mouse_button);
	static mono_bool IsMouseRepeat(int mouse_button);
	static MonoObject* GetMousePosition();
	static int GetMouseXMotion();
	static int GetMouseYMotion();
	static int GetControllerJoystickMove(int pad, MonoString* axis);
	static int GetControllerButton(int pad, MonoString* button);
	static void RumbleController(int pad, float strength, int ms);

	//CONSOLE
	static void Log(MonoObject* object);
	static void Warning(MonoObject* object);
	static void Error(MonoObject* object);

	//AUDIO
	static bool IsMuted();
	static void SetMute(bool set);
	static int GetVolume();
	static void SetVolume(int volume);
	static int GetPitch();
	static void SetPitch(int pitch);
	static void SetRTPvalue(MonoString* name, float value);

	static bool Play(MonoObject * object, MonoString * name);
	static bool Stop(MonoObject * object, MonoString * name);
	static bool Send(MonoObject * object, MonoString * name);

	//PARTICLE EMMITER
	static void PlayEmmiter(MonoObject * object);
	static void StopEmmiter(MonoObject * object);

	//RIGIDBODY
	static void SetLinearVelocity(MonoObject * object, float x, float y, float z);

private:
	std::string mono_path;
	MonoDomain* mono_domain;
	MonoImage* mono_engine_image;
	static CSScript* current_script;
	static bool inside_function;
	MonoImage* mono_compiler_image;
};

