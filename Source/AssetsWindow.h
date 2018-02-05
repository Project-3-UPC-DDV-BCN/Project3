#pragma once
#include "Window.h"
#include "Script.h"
#include "Shader.h"

class Texture;

class AssetsWindow :
	public Window
{

public:

	AssetsWindow();
	virtual ~AssetsWindow();

	void DrawWindow();

private:
	void DrawChilds(std::string path);
	void DeleteWindow(std::string path);
	void CreateDirectortWindow();
	void CreateNewScriptWindow(Script::ScriptType type);
	void CreateScript(Script::ScriptType type, std::string scriptName);
	void CreateNewPhysMatWindow();
	void CreateNewShaderWindow(Shader::ShaderType type);
	void CreateNewMaterialWindow();
	void CreateShader(Shader::ShaderType type, std::string shader_name);
	void CreateMaterial(std::string material_name);

private:
	uint node;
	char node_name[150];
	bool show_new_folder_window;
	bool show_delete_window;
	bool show_new_script_window;
	bool show_new_phys_mat_window;
	bool show_new_shader_window;
	bool show_new_material_window = false;

	bool options_is_open;

	bool asset_hovered;

	Texture* texture_icon;
	Texture* mesh_icon;
	Texture* font_icon;
	Texture* folder_icon;

	std::string selected_file_path;
	std::string selected_folder;
	std::string assets_folder_path;
	std::string delete_path;

	Shader::ShaderType shader_type;
};

