#include "ImportWindow.h"
#include "Brofiler\Brofiler.h"

ImportWindow::ImportWindow()
{
	active = false;
	window_name = "Import";
	total_progress = 0;
	current_progress = 0;
}

ImportWindow::~ImportWindow()
{
}

void ImportWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - Import: DrawWindow", Profiler::Color::Black);

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos({ (io.DisplaySize.x / 2) - 150, (io.DisplaySize.y / 2) - 50 });
	ImGui::SetNextWindowSize({ 300,100 });
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	ImGui::ProgressBar(current_progress);
	ImGui::Text(progress_file_name.c_str());

	if (current_progress == total_progress) active = false;

	ImGui::End();
}

void ImportWindow::SetTotalProgress(int number)
{
	total_progress = number;
}

void ImportWindow::SetCurrentProgress(int number)
{
	if(number <= total_progress)
		current_progress = number;
}

void ImportWindow::SetProgressFileName(std::string name)
{
	progress_file_name = name;
}

void ImportWindow::IncreaseProgress()
{
	current_progress++;
}

void ImportWindow::IncreaseTotalProgressByAmount(int amount)
{
	total_progress += amount;
}
