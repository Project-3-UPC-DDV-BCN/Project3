#pragma once
#include "Window.h"
#include "imgui\ImGuiColorTextEdit\TextEditor.h"

class TextEditorWindow :
	public Window
{

public:

	TextEditorWindow();
	~TextEditorWindow();

	void DrawWindow();

	void SetPath(std::string string);
	void SetLanguageType(TextEditor::LanguageDefinition language_type);

private:
	TextEditor editor;
	std::string path;
	std::string text;
};

