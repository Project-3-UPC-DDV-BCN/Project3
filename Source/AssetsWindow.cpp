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
#include "PhysicsMaterial.h"

AssetsWindow::AssetsWindow()
{
	active = true;
	window_name = "Assets";

	node = 0;
	show_new_folder_window = false;
	texture_icon = nullptr;
	show_delete_window = false;
	show_new_script_window = false;
	options_is_open = false;
	asset_hovered = false;
	show_new_phys_mat_window = false;

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
	selected_folder = assets_folder_path;
}

AssetsWindow::~AssetsWindow()
{
	RELEASE(mesh_icon);
	RELEASE(font_icon);
	RELEASE(folder_icon)
}

void AssetsWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, App->IsPlaying(), ImGuiWindowFlags_HorizontalScrollbar)) {
		ImGui::Columns(2);
		node = 0;
		ImGui::Spacing();
		DrawChilds(assets_folder_path);

		if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringWindow()) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::CloseCurrentPopup();
			ImGui::OpenPopup("Folder Options");
		}

		if (!App->file_system->DirectoryIsEmpty(selected_folder)) {
			if (ImGui::BeginPopup("Folder Options"))
			{
				if (ImGui::MenuItem("Create Folder")) {
					show_new_folder_window = true;
					show_delete_window = false;
					show_new_script_window = false;
				}
				if (App->file_system->GetDirectoryName(selected_folder) != "Assets") {
					if (ImGui::MenuItem("Delete")) {
						show_delete_window = true;
						show_new_folder_window = false;
						show_new_script_window = false;
						delete_path = selected_folder;
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
			if (!selected_folder.empty()) 
			{
				std::vector<std::string> files = App->file_system->GetFilesInDirectory(selected_folder);
				for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
				{
					bool selected = false;
					float font_size = ImGui::GetFontSize();
					std::string file_extension = App->file_system->GetFileExtension(*it);
					std::string file_name = App->file_system->GetFileNameWithoutExtension(*it);
					Resource::ResourceType type = (Resource::ResourceType)App->resources->AssetExtensionToResourceType(file_extension);
					switch (type)
					{
					case Resource::TextureResource:
						texture_icon = App->resources->GetTexture(file_name);
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

					if (*it == selected_file_path) {
						if (App->scene->selected_gameobjects.empty()) {
							selected = true;
						}
						else {
							selected_file_path.clear();
						}
					}
					ImGui::Selectable((file_name + file_extension).c_str(), &selected);
					if (ImGui::IsItemHoveredRect()) 
					{
						asset_hovered = true;
						
						if (ImGui::IsMouseDragging() && !App->editor->drag_data->hasData)
						{
							Resource::ResourceType type = App->resources->AssetExtensionToResourceType(file_extension);
							Resource* resource = nullptr;
							switch (type)
							{
							case Resource::TextureResource:
								resource = (Resource*)App->resources->GetTexture(file_name);
								break;
							case Resource::MeshResource:
								resource = (Resource*)App->resources->GetMesh(file_name);
								break;
							case Resource::SceneResource:
								break;
							case Resource::AnimationResource:
								break;
							case Resource::PrefabResource:
								resource = (Resource*)App->resources->GetPrefab(file_name);
								break;
							case Resource::ScriptResource:
								resource = (Resource*)App->resources->GetScript(file_name);
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
								resource = (Resource*)App->resources->GetMaterial(file_name);
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
								selected_file_path = *it;
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
					show_delete_window = false;
					show_new_folder_window = false;
					options_is_open = false;
					show_new_phys_mat_window = false;
				}

				if (ImGui::MenuItem("Create Physics Material"))
				{
					show_new_phys_mat_window = true;
					show_new_script_window = false;
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
					options_is_open = false;
				}
				if (ImGui::MenuItem("Delete")) {
					delete_path = selected_file_path;
					show_delete_window = true;
					show_new_folder_window = false;
					show_new_script_window = false;
					options_is_open = false;
				}

				std::string extension = App->file_system->GetFileExtension(selected_file_path);
				if (extension == ".prefab" || extension == ".fbx" || extension == ".FBX")
				{
					if (ImGui::MenuItem("Load to scene")) {
						std::string file_name = App->file_system->GetFileNameWithoutExtension(selected_file_path);
						Prefab* prefab = App->resources->GetPrefab(file_name);
						App->scene->LoadPrefab(prefab);
						options_is_open = false;
					}
				}

				if (extension == ".cs")
				{
					if (ImGui::MenuItem("Edit")) {
						App->editor->text_editor_window->SetPath(selected_file_path);
						App->editor->text_editor_window->SetActive(true);
						options_is_open = false;
					}
				}

				if (extension == ".scene")
				{
					if (ImGui::MenuItem("Load")) {
						App->scene->LoadScene(selected_file_path);
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

		if (show_new_phys_mat_window)
		{
			CreateNewPhysMatWindow();
		}
	}
	ImGui::EndDock();
}

void AssetsWindow::DrawChilds(std::string path)
{
	std::string path_name;
	path_name = App->file_system->GetDirectoryName(path);
	sprintf_s(node_name, 150, "%s##node_%i", path_name.c_str(), node++);
	uint flag = 0;

	if (!App->file_system->DirectoryHasSubDirectories(path))
	{
		flag |= ImGuiTreeNodeFlags_Leaf;
	}

	flag |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (selected_folder == path && !show_new_folder_window) {
		flag |= ImGuiTreeNodeFlags_Selected;
	}

	if (ImGui::TreeNodeExI(node_name, (ImTextureID)folder_icon->GetID(), flag))
	{
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
			selected_folder = path;
		}
		std::vector<std::string> sub_directories = App->file_system->GetSubDirectories(path);
		for (std::vector<std::string>::iterator it = sub_directories.begin(); it != sub_directories.end(); it++)
		{
			DrawChilds(*it);
		}
		ImGui::TreePop();
	}
	else {
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
			selected_folder = path;
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
	ImGui::Text("New Folder Name");
	static char inputText[20];
	ImGui::InputText("", inputText, 20);
	ImGui::Spacing();
	if (ImGui::Button("Confirm")) {
		std::string str(inputText);
		std::string temp = selected_folder;
		if (App->file_system->Create_Directory(selected_folder += ("\\" + str))) {
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
	ImGui::Text("New Script Name");
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

		std::ofstream output_file(selected_folder + "\\" + new_file_name);
		output_file << str;
		output_file.close();

		App->resources->CreateResource(selected_folder + "\\" + new_file_name);
	}
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
			data.SaveAsBinary(selected_folder + "\\" + str + ".pmat");
			App->resources->CreateResource(selected_folder + "\\" + str + ".pmat");
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
