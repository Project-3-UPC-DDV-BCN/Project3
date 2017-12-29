#include "ConsoleWindow.h"

ConsoleWindow::ConsoleWindow()
{
	active = true;
	window_name = "Console";
	UpdateLabels();

	scroll_to_bottom = false;
	show_logs = true;
	show_warnings = true;
	show_errors = true;
	show_debug_logs = false;
	log_text_color = { 1.00f, 1.00f ,1.00f ,1.00f };
	warning_text_color = { 1.00f, 1.00f, 0.00f ,1.00f };
	error_text_color = { 1.00f, 0.00f, 0.00f ,1.00f };
	debug_text_color = { 0.40f, 0.90f, 0.90f ,1.00f };
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, false, ImGuiWindowFlags_HorizontalScrollbar)) {
		if (ImGui::Button(logs_label.c_str())) {
			show_logs = !show_logs;
		}
		ImGui::SameLine();
		if (ImGui::Button(warnings_label.c_str())) {
			show_warnings = !show_warnings;
		}
		ImGui::SameLine();
		if (ImGui::Button(errors_label.c_str())) {
			show_errors = !show_errors;
		}
		ImGui::SameLine();
		if (ImGui::Button(debug_label.c_str())) {
			show_debug_logs = !show_debug_logs;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			Clear();
		}

		ImGui::Separator();
		ImGui::BeginChild("scrolling");

		if (show_errors && !errors_list.empty()) {
			for (int i = 0; i < errors_list.size(); i++) {
				ImGui::TextColored(error_text_color, "%s", errors_list[i].c_str());
			}
		}
		if (show_warnings && !warnings_list.empty()) {
			for (int i = 0; i < warnings_list.size(); i++) {
				ImGui::TextColored(warning_text_color, "%s", warnings_list[i].c_str());
			}
		}
		if (show_logs && !logs_list.empty()) {
			for (int i = 0; i < logs_list.size(); i++) {
				ImGui::TextColored(log_text_color, "%s", logs_list[i].c_str());
			}
		}
		if (show_debug_logs && !debug_list.empty()) {
			for (int i = 0; i < debug_list.size(); i++) {
				ImGui::TextColored(debug_text_color, "%s", debug_list[i].c_str());
			}
		}

		if (scroll_to_bottom)
			ImGui::SetScrollHere(1.0f);
		scroll_to_bottom = false;
		ImGui::EndChild();
	}
	ImGui::EndDock();
}

void ConsoleWindow::Clear()
{
	logs_list.clear();
	warnings_list.clear();
	errors_list.clear();
	debug_list.clear();
	UpdateLabels();
}

void ConsoleWindow::AddLog(std::string log, bool is_error, bool is_warning, bool is_debug)
{

	if (is_error) {
		if (errors_list.size() < 99) {
			errors_list.push_back("[Error] " + log);
		}
		else {
			errors_list.erase(errors_list.begin());
		}
	}
	else if (is_warning) {
		if (warnings_list.size() < 99) {
			warnings_list.push_back("[Warning] " + log);
		}
		else {
			warnings_list.erase(warnings_list.begin());
		}
	}
	else if (is_debug) {
		if (debug_list.size() < 99) {
			debug_list.push_back("[Debug] " + log);
		}
		else {
			debug_list.erase(debug_list.begin());
		}
	}
	else {
		if (logs_list.size() < 99) {
			logs_list.push_back("[Log] " + log);
		}
		else {
			logs_list.erase(logs_list.begin());
		}
	}

	scroll_to_bottom = true;
	UpdateLabels();
}

void ConsoleWindow::UpdateLabels()
{
	errors_label = "Errors (" + std::to_string(errors_list.size()) + ")";
	warnings_label = "Warnings (" + std::to_string(warnings_list.size()) + ")";
	logs_label = "Logs (" + std::to_string(logs_list.size()) + ")";
	debug_label = "Debug (" + std::to_string(debug_list.size()) + ")";
}
