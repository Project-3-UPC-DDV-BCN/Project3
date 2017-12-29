#pragma once
#include "Window.h"
#include <map>

class ConsoleWindow :
	public Window
{
public:

	enum MessageType
	{
		MessageLog, MessageWarning, MessageError, MessageDebug
	};

	ConsoleWindow();
	virtual ~ConsoleWindow();

	void DrawWindow();
	void Clear();
	void AddLog(std::string log, bool isError = false, bool isWarning = false, bool is_debug = false);
	void UpdateLabels();

private:
	ImGuiTextBuffer	buf;
	ImGuiTextFilter filter;
	ImVector<int> line_offsets;
	bool scroll_to_bottom;
	std::string logs_label;
	std::string warnings_label;
	std::string errors_label;
	std::string debug_label;
	bool show_logs;
	bool show_warnings;
	bool show_errors;
	bool show_debug_logs;
	int error_count;
	int warning_count;
	int log_count;
	int debug_count;
	MessageType type;
	std::multimap<MessageType, std::string> message_list;
	ImVec4 log_text_color;
	ImVec4 warning_text_color;
	ImVec4 error_text_color;
	ImVec4 debug_text_color;
};

