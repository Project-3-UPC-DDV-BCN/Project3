#pragma once

#include "Module.h"
#include "Window.h"
#include <list>
#include <string>

union SDL_Event;

class HierarchyWindow;
class PropertiesWindow;
class AssetsWindow;
class ConsoleWindow;
class SceneWindow;
class HardwareWindow;
class PerformanceWindow;
class AboutWindow;
class AppWindowConfigWindow;
class EditorStyleWindow;
class RendererConfigWindow;
class InputConfigWindow;
class TagsAndLayersWindow;
class ImportWindow;
class ResourcesWindow;
class GameWindow;
class ResourcesConfigWindow;
class Resource;
class TextEditorWindow;
class AudioWindow;

struct DragData {
	std::string fromPanel;
	std::string path;
	std::string extension;
	std::string name;
	Resource* resource = nullptr;
	bool hasData = false;
	void clearData() {
		fromPanel.clear();
		path.clear();
		extension.clear();
		name.clear();
		hasData = false;
	}
};

class ModuleEditor :
	public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleEditor();

	bool Init(Data* editor_config = nullptr);
	update_status PreUpdate(float delta_time);
	update_status Update(float dt);
	bool DrawEditor();
	bool CleanUp();

	void HandleInput(SDL_Event* event);
	void OpenBrowserPage(const char* url);

	void AddData_Editor(float ms, float fps);

	void LoadEditorStyle();

private:
	ImFont* font = nullptr;
	std::list<Window*> editor_windows;

public:
	HierarchyWindow* hierarchy_window = nullptr;
	AssetsWindow* assets_window = nullptr;
	PropertiesWindow* properties_window = nullptr;
	SceneWindow* scene_window = nullptr;
	ConsoleWindow* console_window = nullptr;
	HardwareWindow* hardware_window = nullptr;
	PerformanceWindow* performance_window = nullptr;
	AboutWindow* about_window = nullptr;
	AppWindowConfigWindow* config_window = nullptr;
	EditorStyleWindow* style_editor_window = nullptr;
	RendererConfigWindow* renderer_config_window = nullptr;
	InputConfigWindow* input_config_window = nullptr;
	TagsAndLayersWindow* tags_and_layers_window = nullptr;
	ImportWindow* import_window = nullptr;
	ResourcesWindow* resources_window = nullptr;
	GameWindow* game_window = nullptr;
	ResourcesConfigWindow* resources_config_window = nullptr;
	DragData* drag_data;
	TextEditorWindow* text_editor_window = nullptr;
	AudioWindow* audio_window = nullptr;
};

