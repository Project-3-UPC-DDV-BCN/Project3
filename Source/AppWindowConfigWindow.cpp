#include "AppWindowConfigWindow.h"
#include "Application.h"
#include "ModuleWindow.h"

AppWindowConfigWindow::AppWindowConfigWindow()
{
	active = false;
	window_name = "Window Settings";
	fps_cap = 60;
}
AppWindowConfigWindow::~AppWindowConfigWindow()
{
	
}
void AppWindowConfigWindow::DrawWindow()
{
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	bool fullscreen = App->window->IsFullScreen();
	bool fullscren_desktop = App->window->IsFullScreenDesktop();
	bool borderless = App->window->IsBorderless();

	float brightness = App->window->GetBrightness();
	float width = App->window->GetWidth();
	float height = App->window->GetHeight();

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		App->window->SetFullScreen(fullscreen);
	}

	if (ImGui::Checkbox("Fullscreen Desktop", &fullscren_desktop))
	{
		App->window->SetFullScreenDesktop(fullscren_desktop);
	}

	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	{
		App->window->SetBrightness(brightness);
	}

	if (ImGui::SliderFloat("Width", &width, 100.0f, 1920.0f, "%.0f") || ImGui::SliderFloat("Height", &height, 100.0f, 1080.0f, "%.0f"))
	{
		App->window->SetSize(width, height);
	}

	ImGui::SliderFloat("FPS Limit", &fps_cap, 1.0f, 999.0f, "%.0f");
	if (ImGui::Button("Apply FPS Cap"))
	{
		App->CapFPS(fps_cap);
	}

	ImGui::End();
}
