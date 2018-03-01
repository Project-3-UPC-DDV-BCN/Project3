#include "GameWindow.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "RenderTextureMSAA.h"
#include "ComponentCamera.h"

GameWindow::GameWindow()
{
	active = true;
	window_name = "Game";
	game_scene_width = 0;
	game_scene_height = 0;
	window_pos = float2::zero;
}

GameWindow::~GameWindow()
{
}

void GameWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, false,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) 
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		window_pos = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

		if (game_scene_width != size.x || game_scene_height != size.y) 
		{
			if (App->renderer3D->game_camera != nullptr)
			{
				App->renderer3D->OnResize(size.x, size.y, App->renderer3D->game_camera);
			}
			game_scene_width = size.x;
			game_scene_height = size.y;
		}

		if (App->renderer3D->game_camera != nullptr && App->renderer3D->game_camera->GetViewportTexture() != nullptr)
		{
			ImGui::Image((void*)App->renderer3D->game_camera->GetViewportTexture()->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
		}
	}
	ImGui::EndDock();
}

float2 GameWindow::GetPos() const
{
	return window_pos;
}

float2 GameWindow::GetSize() const
{
	return float2(game_scene_width, game_scene_height);
}
