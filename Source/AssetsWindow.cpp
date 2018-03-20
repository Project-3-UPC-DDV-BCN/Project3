#include "AssetsWindow.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "Resource.h"
#include "ModuleResources.h"
#include "ModuleTextureImporter.h"
#include "tinyfiledialogs.h"
#include "Texture.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "Data.h"
#include "CSScript.h"
#include "LuaScript.h"
#include "ModuleScriptImporter.h"
#include "TextEditorWindow.h"
#include "Material.h"
#include "PhysicsMaterial.h"
#include "BlastModel.h"
#include "ModuleBlast.h"
#include "ModuleBlastMeshImporter.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"

AssetsWindow::AssetsWindow()
{
	active = true;
	window_name = "Assets";

	node = 0;
	show_new_folder_window = false;
	texture_icon = nullptr;
	show_delete_window = false;
	show_new_script_window = false;
	show_new_shader_window = false;
	show_new_phys_mat_window = false;
	options_is_open = false;
	asset_hovered = false;

	shader_type = Shader::ShaderType::ST_NULL;

	mesh_icon = App->texture_importer->LoadTextureFromLibrary(EDITOR_IMAGES_FOLDER"mesh_icon.png");
	font_icon = App->texture_importer->LoadTextureFromLibrary(EDITOR_IMAGES_FOLDER"font_icon.png");
	folder_icon = App->texture_importer->LoadTextureFromLibrary(EDITOR_IMAGES_FOLDER"folder_icon.png");

	if (!App->file_system->DirectoryExist(ASSETS_FOLDER_PATH)) {
		if (!App->file_system->Create_Directory(ASSETS_FOLDER_PATH)) {
			CONSOLE_ERROR("Assets folder is not found and can't create new folder");
			return;
		}
	}
	assets_folder_path = App->file_system->StringToPathFormat(ASSETS_FOLDER_PATH);
	FillDirectories(nullptr, assets_folder_path);
	selected_folder = directories.front();
}

AssetsWindow::~AssetsWindow()
{
	RELEASE(mesh_icon);
	RELEASE(font_icon);
	RELEASE(folder_icon);

	CleanUp(*directories.front());
	directories.clear();
}

void AssetsWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, App->IsPlaying(), ImGuiWindowFlags_HorizontalScrollbar)) {
		ImGui::Columns(2);
		node = 0;
		ImGui::Spacing();

		for (int i = 0; i < directories.size(); i++)
		{
			CheckDirectory(*directories[i]);
		}

		DrawChilds(*directories.front());

		if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringWindow()) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::CloseCurrentPopup();
			ImGui::OpenPopup("Folder Options");
		}

		if (!selected_folder->sub_directories.empty()) {
			if (ImGui::BeginPopup("Folder Options"))
			{
				if (ImGui::MenuItem("Create Folder")) {
					show_new_folder_window = true;
					show_delete_window = false;
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_phys_mat_window = false;
				}
				if (selected_folder->name != "Assets") {
					if (ImGui::MenuItem("Delete")) {
						show_delete_window = true;
						show_new_folder_window = false;
						show_new_script_window = false;
						show_new_shader_window = false;
						show_new_phys_mat_window = false;
						delete_path = selected_folder->path;
					}
				}
				ImGui::EndPopup();
			}
		}

		if (show_new_folder_window) {
			CreateDirectortWindow();
		}

		if (show_delete_window)
		{
			DeleteWindow(delete_path);
		}

		ImGui::NextColumn();

		if (ImGui::BeginChild("Files", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar, App->IsPlaying()))
		{
			if (!selected_folder->directory_files.empty())
			{
				for (std::vector<File*>::iterator it = selected_folder->directory_files.begin(); it != selected_folder->directory_files.end(); it++)
				{
					bool selected = false;
					float font_size = ImGui::GetFontSize();
					Resource::ResourceType type = (Resource::ResourceType)App->resources->AssetExtensionToResourceType((*it)->extension);
					switch (type)
					{
					case Resource::TextureResource:
						texture_icon = App->resources->GetTexture((*it)->name);
						ImGui::Image((ImTextureID)texture_icon->GetID(), { font_size, font_size }, ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						break;
					case Resource::MeshResource:
						ImGui::Image((ImTextureID)mesh_icon->GetID(), { font_size, font_size }, ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						break;
					case Resource::FontResource:
						ImGui::Image((ImTextureID)font_icon->GetID(), { font_size, font_size }, ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						break;
					case Resource::Unknown:
						continue; //if the type is unknown skip and don't draw the file in the panel
						break;
					}

					if ((*it)->path == selected_file_path)
					{
						if (App->scene->selected_gameobjects.empty()) {
							selected = true;
						}
						else {
							selected_file_path.clear();
						}
					}
					ImGui::Selectable(((*it)->name + (*it)->extension).c_str(), &selected);
					if (ImGui::IsItemHoveredRect())
					{
						asset_hovered = true;

						if (ImGui::IsMouseDragging() && !App->editor->drag_data->hasData)
						{
							Resource::ResourceType type = App->resources->AssetExtensionToResourceType((*it)->extension);
							Resource* resource = nullptr;
							switch (type)
							{
							case Resource::TextureResource:
								resource = (Resource*)App->resources->GetTexture((*it)->name);
								break;
							case Resource::MeshResource:
								resource = (Resource*)App->resources->GetMesh((*it)->name);
								break;
							case Resource::SceneResource:
								break;
							case Resource::AnimationResource:
								break;
							case Resource::PrefabResource:
								resource = (Resource*)App->resources->GetPrefab((*it)->name);
								break;
							case Resource::ScriptResource:
								resource = (Resource*)App->resources->GetScript((*it)->name);
								break;
							case Resource::AudioResource:
								break;
							case Resource::ParticleFXResource:
								break;
							case Resource::FontResource:
								break;
							case Resource::RenderTextureResource:
								break;
							case Resource::GameObjectResource:
								break;
							case Resource::MaterialResource:
								resource = (Resource*)App->resources->GetMaterial((*it)->name);
								break;
							case Resource::ShaderResource:
								resource = (Resource*)App->resources->GetShader((*it)->name);
								break;
							case Resource::Unknown:
								break;
							default:
								break;
							}
							if (resource != nullptr)
							{
								App->editor->drag_data->hasData = true;
								App->editor->drag_data->fromPanel = "Assets";
								App->editor->drag_data->resource = resource;
							}
						}
						else
						{
							if (!options_is_open && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))) {
								selected_file_path = (*it)->path;
								App->scene->selected_gameobjects.clear();
								if (ImGui::IsMouseClicked(1)) {
									ImGui::SetNextWindowPos(ImGui::GetMousePos());
									ImGui::CloseCurrentPopup();
									ImGui::OpenPopup("File Options");
									options_is_open = true;
								}
							}
						}
					}
					else
					{
						if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseReleased(0) && !asset_hovered)
						{
							selected = false;
							selected_file_path = "";
							options_is_open = false;
						}
					}
				}
			}

			if (!ImGui::IsMouseHoveringWindow() && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
			{
				options_is_open = false;
			}

			if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseReleased(1) && !asset_hovered)
			{
				ImGui::CloseCurrentPopup();
				ImGui::SetNextWindowPos(ImGui::GetMousePos());
				ImGui::OpenPopup("Files Window Options");
			}

			if (ImGui::BeginPopup("Files Window Options"))
			{
				if (ImGui::MenuItem("Create C# Script"))
				{
					show_new_script_window = true;
					show_new_shader_window = false;
					show_new_material_window = false;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
				}

				if (ImGui::MenuItem("Create Vertex Shader"))
				{
					show_new_script_window = false;
					show_new_shader_window = true;
					show_new_material_window = false;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
					shader_type = Shader::ST_VERTEX;
				}

				if (ImGui::MenuItem("Create Fragment Shader"))
				{
					show_new_script_window = false;
					show_new_shader_window = true;
					show_new_material_window = false;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
					shader_type = Shader::ST_FRAGMENT;
				}

				if (ImGui::MenuItem("Create Material"))
				{
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_material_window = true;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
				}

				if (ImGui::MenuItem("Create Physics Material"))
				{
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_material_window = false;
					show_new_phys_mat_window = true;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
				}

				if (ImGui::MenuItem("Create GOAP Goal"))
				{
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_material_window = false;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = true;
					show_new_goap_action_window = false;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
				}

				if (ImGui::MenuItem("Create GOAP Action"))
				{
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_material_window = false;
					show_new_phys_mat_window = false;
					show_new_goap_goal_window = false;
					show_new_goap_action_window = true;

					show_delete_window = false;
					show_new_folder_window = false;

					options_is_open = false;
				}

				ImGui::EndPopup();
			}

			if (ImGui::IsMouseHoveringWindow() && !ImGui::IsAnyItemHovered())
			{
				asset_hovered = false;
			}

			if (ImGui::BeginPopup("File Options"))
			{
				if (ImGui::MenuItem("Rename")) {
					show_new_folder_window = false;
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_phys_mat_window = false;
					options_is_open = false;
				}
				if (ImGui::MenuItem("Delete")) {
					delete_path = selected_file_path;
					show_delete_window = true;
					show_new_folder_window = false;
					show_new_script_window = false;
					show_new_shader_window = false;
					show_new_phys_mat_window = false;
					options_is_open = false;
				}

				std::string extension = App->file_system->GetFileExtension(selected_file_path);
				if (extension == ".prefab" || extension == ".fbx" || extension == ".FBX")
				{
					if (ImGui::MenuItem("Load to scene")) {
						std::string file_name = App->file_system->GetFileNameWithoutExtension(selected_file_path);
						Prefab* prefab = App->resources->GetPrefab(file_name);
						if (prefab)
						{
							App->scene->LoadPrefab(prefab);
						}
						else
						{
							CONSOLE_ERROR("Cannot load %s. It's null", file_name.c_str());
						}
						options_is_open = false;
					}
				}

				if (extension == ".cs" || extension == ".vshader" || extension == ".fshader")
				{
					if (ImGui::MenuItem("Edit")) {
						App->editor->text_editor_window->SetPath(selected_file_path);
						if (extension == ".cs")
						{
							App->editor->text_editor_window->SetLanguageType(TextEditor::LanguageDefinition::CSharp());
						}
						else if (extension == ".vshader" || extension == ".fshader")
						{
							App->editor->text_editor_window->SetLanguageType(TextEditor::LanguageDefinition::GLSL());
						}

						App->editor->text_editor_window->SetActive(true);
						options_is_open = false;
					}
				}

				if (extension == ".scene" || extension == ".json")
				{
					if (ImGui::MenuItem("Load")) {
						App->scene->LoadScene(selected_file_path);
						options_is_open = false;
					}
				}

				if (extension == ".bmesh")
				{
					if (ImGui::MenuItem("Load to scene##bmesh")) {
						std::string file_name = App->file_system->GetFileNameWithoutExtension(selected_file_path);
						BlastModel* model = App->blast_mesh_importer->LoadModelFromLibrary(selected_file_path, false);
						if (model)
						{
							App->blast->CreateFamily(model);
							App->blast->SpawnFamily(model, false);
						}
						else
						{
							CONSOLE_ERROR("model '%s' is null", file_name.c_str());
						}
						options_is_open = false;
					}
				}

				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();

		if (show_new_script_window)
		{
			CreateNewScriptWindow(Script::CsScript);
		}

		if (show_new_shader_window)
		{
			CreateNewShaderWindow(shader_type);
		}

		if (show_new_material_window)
		{
			CreateNewMaterialWindow();
		}

		if (show_new_phys_mat_window)
		{
			CreateNewPhysMatWindow();
		}
		
		if (show_new_goap_goal_window)
		{
			CreateNewGOAPGoalWindow();
		}

		if (show_new_goap_action_window)
		{
			CreateNewGOAPActionWindow();
		}
	}
	ImGui::EndDock();
}

void AssetsWindow::DrawChilds(Directory& directory)
{
	sprintf_s(node_name, 150, "%s##node_%i", directory.name.c_str(), node++);
	uint flag = 0;

	if (directory.sub_directories.empty())
	{
		flag |= ImGuiTreeNodeFlags_Leaf;
	}

	flag |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (selected_folder->path == directory.path && !show_new_folder_window)
	{
		flag |= ImGuiTreeNodeFlags_Selected;
	}

	if (ImGui::TreeNodeExI(node_name, (ImTextureID)folder_icon->GetID(), flag))
	{
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
			selected_folder = &directory;
		}
		for (std::vector<Directory*>::iterator it = directory.sub_directories.begin(); it != directory.sub_directories.end(); it++)
		{
			DrawChilds(*(*it));
		}
		ImGui::TreePop();
	}
	else {
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
			selected_folder = &directory;
		}
	}
}

void AssetsWindow::CreateDirectortWindow()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("New Folder Name", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	bool confirmed = false;
	ImGui::Text("New Folder Name");
	static char inputText[20];
	if (ImGui::InputText("", inputText, 20, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		confirmed = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		confirmed = true;
	}
	if (confirmed)
	{
		std::string str(inputText);
		Directory* temp = selected_folder;
		if (App->file_system->Create_Directory(selected_folder->path += ("\\" + str)))
		{
			show_new_folder_window = false;
		}
		else {
			selected_folder = temp;
		}
		strcpy(inputText, "");
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		strcpy(inputText, "");
		show_new_folder_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateNewScriptWindow(Script::ScriptType type)
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();

	ImGui::Begin("New Script Name", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	bool confirmed = false;
	ImGui::Text("New Script Name");
	static char inputText[30];
	if (ImGui::InputText("", inputText, 30, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		confirmed = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		confirmed = true;
	}
	if (confirmed)
	{
		std::string str(inputText);
		if (!str.empty()) {
			for (std::string::iterator it = str.begin(); it != str.end(); it++)
			{
				if (*it == ' ') *it = '_';
			}
			CreateScript(type, str);

			strcpy(inputText, "");
			show_new_script_window = false;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		strcpy(inputText, "");
		show_new_script_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateScript(Script::ScriptType type, std::string scriptName)
{
	std::ifstream in_file;
	std::string new_file_name;

	switch (type)
	{
	case Script::CsScript:
		in_file.open(CS_TEMPLATE_FILE_PATH);
		new_file_name = scriptName + ".cs";
		break;
	case Script::LuaScript:
		in_file.open(LUA_TEMPLATE_FILE_PATH);
		new_file_name = scriptName + ".lua";
		break;
	}

	if (in_file.is_open()) {
		std::stringstream str_stream;
		str_stream << in_file.rdbuf();//read the file
		std::string str = str_stream.str();//str holds the content of the file

		if (str.empty())
			return;
		size_t start_pos = 0;
		std::string class_name_template = "#CLASS_NAME#";
		while ((start_pos = str.find(class_name_template, start_pos)) != std::string::npos) {
			str.replace(start_pos, class_name_template.length(), scriptName);
			start_pos += scriptName.length();
		}

		in_file.close();

		std::ofstream output_file(selected_folder->path + "\\" + new_file_name);
		output_file << str;
		output_file.close();

		App->resources->CreateResource(selected_folder->path + "\\" + new_file_name);
	}
}

void AssetsWindow::CreateNewShaderWindow(Shader::ShaderType type)
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();

	ImGui::Begin("New Shader Name", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	bool confirmed = false;
	ImGui::Text("New Shader Name");
	static char inputText[30];
	if (ImGui::InputText("", inputText, 30, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		confirmed = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		confirmed = true;
	}
	if (confirmed)
	{
		std::string str(inputText);
		if (!str.empty()) {
			for (std::string::iterator it = str.begin(); it != str.end(); it++)
			{
				if (*it == ' ') *it = '_';
			}
			CreateShader(type, str);
			shader_type = Shader::ShaderType::ST_NULL;
			strcpy(inputText, "");
			show_new_shader_window = false;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		strcpy(inputText, "");
		show_new_shader_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateNewMaterialWindow()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();

	ImGui::Begin("New Material Name", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	bool confirmed = false;
	ImGui::Text("New Material Name");
	static char inputText[30];
	if (ImGui::InputText("", inputText, 30, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		confirmed = true;
	}
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		confirmed = true;
	}
	if (confirmed)
	{
		std::string str(inputText);
		if (!str.empty()) {
			for (std::string::iterator it = str.begin(); it != str.end(); it++)
			{
				if (*it == ' ') *it = '_';
			}
			CreateMaterial(str);
			strcpy(inputText, "");
			show_new_material_window = false;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		strcpy(inputText, "");
		show_new_material_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateShader(Shader::ShaderType type, std::string shader_name)
{
	std::ifstream in_file;
	std::string new_file_name;

	switch (type)
	{
	case Shader::ST_VERTEX:
		in_file.open(VERTEX_SHADER_TEMPLATE_FILE_PATH);
		new_file_name = shader_name + ".vshader";
		break;
	case Shader::ST_FRAGMENT:
		in_file.open(FRAGMENT_SHADER_TEMPLATE_FILE_PATH);
		new_file_name = shader_name + ".fshader";
		break;
	}

	if (in_file.is_open()) {
		std::stringstream str_stream;
		str_stream << in_file.rdbuf();//read the file
		std::string str = str_stream.str();//str holds the content of the file

		if (str.empty())
			return;

		in_file.close();

		std::ofstream output_file(selected_folder->path + "\\" + new_file_name);
		output_file << str;
		output_file.close();

		App->resources->CreateResource(selected_folder->path + "\\" + new_file_name);
	}
}

void AssetsWindow::CreateMaterial(std::string material_name)
{
	std::string new_file_name = material_name + ".mat";

	Material* new_mat = new Material();
	new_mat->SetName(material_name);
	Data d;
	new_mat->Save(d);

	d.SaveAsBinary(selected_folder->path + "\\" + new_file_name);

	RELEASE(new_mat);
	App->resources->CreateResource(selected_folder->path + "\\" + new_file_name);
}

void AssetsWindow::CreateNewGOAPGoalWindow()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("New GOAP Goal", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	ImGui::Text("New GOAP Goal");
	static char inputText[30];
	ImGui::InputText("", inputText, 30);
	static int priority = 1;
	ImGui::InputInt("Priority (1 to 100)", &priority);
	static int increment = 0;
	ImGui::InputInt("Auto Increment", &increment);
	static float t_step = 0.0f;
	ImGui::SameLine();
	ImGui::InputFloat("TimeStep (0.0 disabled)", &t_step, 0.1f);

	if (ImGui::Button("Create##GOAPGoal"))
	{
		CreateGOAPGoal(inputText, priority, increment, t_step);
		show_new_goap_goal_window = false;
		strcpy(inputText, "");
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##GOAPGoal")) {
		strcpy(inputText, "");
		show_new_goap_goal_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateGOAPGoal(std::string name, int priority, int inc_rate, float timestep)
{
	std::string file_name = name + "_GOAPGoal";
	std::string file_path = file_name + ".json";

	GOAPGoal* new_goal = new GOAPGoal();
	new_goal->SetName(file_name);
	new_goal->SetPriority(priority);
	new_goal->SetIncrement(inc_rate, timestep);
	new_goal->SetAssetsPath(selected_folder->path + "\\" + file_path);
	std::string lib_path = LIBRARY_GOAP_FOLDER + file_path;
	new_goal->SetLibraryPath(lib_path);
	Data d;
	new_goal->Save(d);
	d.SaveAsJSON(new_goal->GetAssetsPath());
	App->file_system->Copy(new_goal->GetAssetsPath(), lib_path);
	//new_goal->CreateMeta();

	App->resources->AddGOAPGoal(new_goal);
}

void AssetsWindow::CreateNewGOAPActionWindow()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("New GOAP Action", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	ImGui::Text("New GOAP Action");
	static char inputText[30];
	ImGui::InputText("", inputText, 30);
	static int cost = 1;
	ImGui::InputInt("Cost", &cost);

	if (ImGui::Button("Create##GOAPAction"))
	{
		CreateGOAPAction(inputText, cost);
		show_new_goap_action_window = false;
		strcpy(inputText, "");
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##GOAPAction")) {
		strcpy(inputText, "");
		show_new_goap_action_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CreateGOAPAction(std::string name, int cost)
{
	std::string file_name = name + "_GOAPAction";
	std::string file_path = file_name + ".json";

	GOAPAction* new_action = new GOAPAction(file_name.c_str(),cost);

	new_action->SetAssetsPath(selected_folder->path + "\\" + file_path);
	std::string lib_path = LIBRARY_GOAP_FOLDER + file_path;
	new_action->SetLibraryPath(lib_path);
	Data d;
	new_action->Save(d);
	d.SaveAsJSON(new_action->GetAssetsPath());
	App->file_system->Copy(new_action->GetAssetsPath(), lib_path);
	//new_goal->CreateMeta();

	App->resources->AddGOAPAction(new_action);
}

void AssetsWindow::CreateNewPhysMatWindow()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin("New Physics Material", &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	ImGui::Text("New Physics Material");
	static char inputText[30];
	ImGui::InputText("", inputText, 30);
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		std::string str(inputText);
		if (!str.empty()) {
			for (std::string::iterator it = str.begin(); it != str.end(); it++)
			{
				if (*it == ' ') *it = '_';
			}
			Data data;
			PhysicsMaterial* mat = new PhysicsMaterial();
			mat->SetName(str);
			mat->Save(data);
			data.SaveAsBinary(selected_folder->path + "\\" + str + ".pmat");
			App->resources->CreateResource(selected_folder->path + "\\" + str + ".pmat");
			show_new_phys_mat_window = false;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		strcpy(inputText, "");
		show_new_phys_mat_window = false;
	}
	ImGui::End();
}

void AssetsWindow::CheckDirectory(Directory& directory)
{
	//Check if the directory has been modified
	long long current_modified_time = App->file_system->GetModifiedTime(directory.path);
	if (current_modified_time > directory.current_modified_time)
	{
		directory.current_modified_time = current_modified_time;
		for (std::vector<File*>::iterator it = directory.directory_files.begin(); it != directory.directory_files.end();)
		{
			//Check if the file has been modified
			long long file_current_modified_time = App->file_system->GetModifiedTime((*it)->path);
			if (file_current_modified_time != 0)
			{
				if (file_current_modified_time > (*it)->current_modified_time)
				{
					//If it's modified, delete the previous resource and create a new one
					App->resources->DeleteResource((*it)->path);
					App->resources->CreateResource((*it)->path);
					(*it)->current_modified_time = file_current_modified_time;
				}
				it++;
			}
			else
			{
				//File doesn't exist, remove it and delete the resource;
				App->resources->DeleteResource((*it)->path);
				RELEASE(*it);
				it = directory.directory_files.erase(it);
			}
		}

		//Check if directory have new files
		std::vector<std::string> new_files = App->file_system->GetFilesInDirectory(directory.path);
		for (std::vector<std::string>::iterator it = new_files.begin(); it != new_files.end(); it++)
		{
			bool file_exist = false;
			for (std::vector<File*>::iterator it2 = directory.directory_files.begin(); it2 != directory.directory_files.end(); *it2++)
			{
				if (*it == (*it2)->path)
				{
					file_exist = true;
					break;
				}
			}

			if (!file_exist)
			{
				File* file = new File();
				file->path = *it;
				file->extension = App->file_system->GetFileExtension(*it);
				file->name = App->file_system->GetFileNameWithoutExtension(*it);
				file->current_modified_time = App->file_system->GetModifiedTime(*it);
				directory.directory_files.push_back(file);
				App->resources->CreateResource(*it);
			}
		}

		//Check if directory have new sub directories
		std::vector<std::string> new_directories = App->file_system->GetSubDirectories(directory.path);
		for (std::vector<std::string>::iterator it = new_directories.begin(); it != new_directories.end(); it++)
		{
			bool directory_exist = false;
			for (std::vector<Directory*>::iterator it2 = directory.sub_directories.begin(); it2 != directory.sub_directories.end(); *it2++)
			{
				if (*it == (*it2)->path)
				{
					directory_exist = true;
					break;
				}
			}

			if (!directory_exist)
			{
				FillDirectories(&directory, *it);
			}
		}
	}
}

void AssetsWindow::FillDirectories(Directory* parent, std::string directory_path)
{
	Directory* dir = new Directory();
	dir->path = directory_path;
	dir->name = App->file_system->GetDirectoryName(directory_path);
	dir->current_modified_time = App->file_system->GetModifiedTime(directory_path);
	std::vector<std::string> files = App->file_system->GetFilesInDirectory(directory_path);
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		File* file = new File();
		file->path = *it;
		file->extension = App->file_system->GetFileExtension(*it);
		file->name = App->file_system->GetFileNameWithoutExtension(*it);
		file->current_modified_time = App->file_system->GetModifiedTime(*it);
		dir->directory_files.push_back(file);
	}
	directories.push_back(dir);
	std::vector<std::string> sub_directories = App->file_system->GetSubDirectories(directory_path);
	for (std::vector<std::string>::iterator it = sub_directories.begin(); it != sub_directories.end(); it++)
	{
		FillDirectories(dir, *it);
	}

	if (parent)
	{
		parent->sub_directories.push_back(dir);
	}
}

void AssetsWindow::CleanUp(Directory & directory)
{
	for (File* file : directory.directory_files)
	{
		RELEASE(file);
	}
	for (Directory* dir : directory.sub_directories)
	{
		CleanUp(*dir);
		RELEASE(dir);
	}
}

void AssetsWindow::DeleteWindow(std::string path)
{
	std::string tittle;
	if (App->file_system->IsDirectory(path))
	{
		tittle = "Delete Diretory";
	}
	else
	{
		tittle = "Delete File";
	}
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
	ImGui::SetNextWindowPosCenter();
	ImGui::Begin(tittle.c_str(), &active,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoTitleBar);
	ImGui::Spacing();
	if (App->file_system->IsDirectory(path))
	{
		ImGui::Text("Directory %s will be deleted from disk with all the files inside. Continue?", App->file_system->GetDirectoryName(path).c_str());
	}
	else
	{
		ImGui::Text("File %s will be deleted from disk. Continue?", App->file_system->GetFileName(path).c_str());
	}
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		if (App->file_system->IsDirectory(path))
		{
			std::vector<std::string> files = App->file_system->GetFilesInDirectory(path);
			for (int i = 0; i < files.size(); i++)
			{
				App->file_system->Delete_File(path);
				App->resources->DeleteResource(path);
			}
			App->file_system->DeleteDirectory(path);
		}
		else
		{
			App->file_system->Delete_File(path);
			App->resources->DeleteResource(path);
		}
		show_delete_window = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		show_delete_window = false;
	}
	ImGui::End();

}