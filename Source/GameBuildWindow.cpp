#include "GameBuildWindow.h"
#include "imgui/CustomImGui.h"
#include "tinyfiledialogs.h"

GameBuildWindow::GameBuildWindow()
{
	active = false;
	window_name = "Game Build";
	game_icon = nullptr;
}

GameBuildWindow::~GameBuildWindow()
{
}

void GameBuildWindow::DrawWindow()
{
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	static char tmp_game_name[256] = "No Title";

	ImGui::InputResourceScene("Starting Scene", starting_scene);
	ImGui::InputResourceTexture("Game Icon", &game_icon);

	ImGui::Text("Game Name");
	if (ImGui::InputText("##Game Name", tmp_game_name, 256, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		game_name = tmp_game_name;
	}

	ImGui::Text("Save Path: ");
	ImGui::SameLine();
	ImGui::Text(saving_path.c_str());
	if (ImGui::Button("Select Path"))
	{
		saving_path = tinyfd_selectFolderDialog("Select Folder...", NULL);
	}

	if (ImGui::Button("Build"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		active = false;
	}

	ImGui::End();
}
