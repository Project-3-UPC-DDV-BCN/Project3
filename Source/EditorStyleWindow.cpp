#include "EditorStyleWindow.h"
#include "Brofiler\Brofiler.h"

EditorStyleWindow::EditorStyleWindow()
{
	active = false;
	window_name = "Style Editor";
}

EditorStyleWindow::~EditorStyleWindow()
{
}

void EditorStyleWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - Editor Style: DrawWindow", Profiler::Color::Black);

	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	ImGui::ShowStyleEditor();

	ImGui::End();
}
