#include "TextEditorWindow.h"
#include <fstream>
#include <streambuf>
#include <dinput.h>
#include <tchar.h>
#include "Application.h"
#include "ModuleScriptImporter.h"
#include "ModuleShaderImporter.h"
#include "ModuleFileSystem.h"

TextEditorWindow::TextEditorWindow()
{
	active = false;
	window_name = "TextEditor";
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CSharp());
}


TextEditorWindow::~TextEditorWindow()
{
}

void TextEditorWindow::DrawWindow()
{
	auto cpos = editor.GetCursorPosition();

	if (ImGui::BeginDock(window_name.c_str(), false, false, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar))
	{
		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				std::string text_to_save = editor.GetText();
				/// save text in t....
				std::ofstream ofs;
				ofs.open(path, std::ofstream::out | std::ofstream::trunc);
				ofs << text_to_save;
				ofs.close();
				if (editor.CanUndo())
				{
					std::string extension = App->file_system->GetFileExtension(path);

					if (extension == ".cs")
					{
						App->script_importer->ImportScript(path);
					}
					else
					{
						App->shader_importer->ImportShader(path);
					}
				}
			}
			if (ImGui::MenuItem("Quit"))
				this->active = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();

			if (ImGui::MenuItem("Undo", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();

		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushFont(io.Fonts->Fonts[1]);
		editor.Render("Render...");
		ImGui::PopFont();
	}
	ImGui::EndDock();
}

void TextEditorWindow::SetPath(std::string string)
{
	std::ifstream t(string.c_str());
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	editor.SetText(str);
	path = string;
}

void TextEditorWindow::SetLanguageType(TextEditor::LanguageDefinition language_type)
{
	editor.SetLanguageDefinition(language_type);
}
