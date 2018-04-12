#pragma once
#include "Window.h"
#include "Script.h"
#include "Shader.h"

struct File
{
	std::string name;
	std::string extension;
	std::string path;
	long long current_modified_time = 0;
};

struct Directory
{
	std::string name;
	std::string path;
	std::vector<Directory*> sub_directories;
	std::vector<File*> directory_files;
	long long current_modified_time = 0;
};

class Texture;

class AssetsWindow :
	public Window
{

public:

	AssetsWindow();
	virtual ~AssetsWindow();

	void DrawWindow();

private:
	void DrawChilds(Directory& directory);
	void DeleteWindow(std::string path);
	void CreateDirectortWindow();
	void CreateNewScriptWindow(Script::ScriptType type);
	void CreateScript(Script::ScriptType type, std::string scriptName);
	void CreateNewPhysMatWindow();
	void CreateNewShaderWindow(Shader::ShaderType type);
	void CreateNewMaterialWindow();
	void CreateShader(Shader::ShaderType type, std::string shader_name);
	void CreateMaterial(std::string material_name);
	void CreateNewGOAPGoalWindow();
	void CreateGOAPGoal(std::string name, int priority, int inc_rate, float timestep);
	void CreateNewGOAPActionWindow();
	void CreateGOAPAction(std::string name, int cost);
	void CheckDirectory(Directory& directory);
	void FillDirectories(Directory* parent, std::string directory_path);
	void CleanUp(Directory& directory);

private:
	uint node;
	char node_name[150];
	bool show_new_folder_window;
	bool show_delete_window;
	bool show_new_script_window;
	bool show_new_phys_mat_window;
	bool show_new_shader_window;
	bool show_new_material_window = false;
	bool show_new_goap_goal_window = false;
	bool show_new_goap_action_window = false;

	bool options_is_open;

	bool asset_hovered;

	Texture* texture_icon;
	Texture* mesh_icon;
	Texture* font_icon;
	Texture* folder_icon;

	std::string selected_file_path;
	Directory* selected_folder;
	std::string assets_folder_path;
	std::string delete_path;

	Shader::ShaderType shader_type;

	std::vector<Directory*> directories;

	float current_item_y_pos = 0;
};

