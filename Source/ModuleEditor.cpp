#include "ModuleEditor.h"
#include "Application.h"
#include "imgui\imgui_impl_sdl_gl3.h"
#include "HierarchyWindow.h"
#include "PropertiesWindow.h"
#include "AssetsWindow.h"
#include "ConsoleWindow.h"
#include "SceneWindow.h"
#include "ModuleWindow.h"
#include "HardwareWindow.h"
#include "PerformanceWindow.h"
#include "AboutWindow.h"
#include "AppWindowConfigWindow.h"
#include "EditorStyleWindow.h"
#include "Data.h"
#include "RendererConfigWindow.h"
#include "InputConfigWindow.h"
#include "TagsAndLayersWindow.h"
#include "ImportWindow.h"
#include "tinyfiledialogs.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "imgui\ImGuizmo\ImGuizmo.h"
#include "ModuleFileSystem.h"
#include "ResourcesWindow.h"
#include "GameWindow.h"
#include "ResourcesConfigWindow.h"
#include "CSScript.h"
#include "LuaScript.h"
#include "TextEditorWindow.h"
#include "AudioWindow.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, false)
{
	name = "Editor";
	drag_data = nullptr;
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init(Data* editor_config)
{
	ImGui_ImplSdlGL3_Init(App->window->window);

	if (!App->IsGame())
	{
		// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF(EDITOR_FONTS_FOLDER"Cousine-Regular.ttf", 15.0f);
		io.Fonts->AddFontFromFileTTF(EDITOR_FONTS_FOLDER"DroidSans.ttf", 16.0f);
		io.Fonts->AddFontFromFileTTF(EDITOR_FONTS_FOLDER"ProggyClean.ttf", 13.0f);
		io.Fonts->AddFontFromFileTTF(EDITOR_FONTS_FOLDER"ProggyTiny.ttf", 10.0f);
		font = io.Fonts->AddFontFromFileTTF(EDITOR_FONTS_FOLDER"OpenSans-Semibold.ttf", 16.0f);

		LoadEditorStyle();

		editor_windows.push_back(scene_window = new SceneWindow());
		editor_windows.push_back(assets_window = new AssetsWindow());
		editor_windows.push_back(hierarchy_window = new HierarchyWindow());
		editor_windows.push_back(properties_window = new PropertiesWindow());
		editor_windows.push_back(console_window = new ConsoleWindow());
		editor_windows.push_back(hardware_window = new HardwareWindow());
		editor_windows.push_back(performance_window = new PerformanceWindow());
		editor_windows.push_back(about_window = new AboutWindow());
		editor_windows.push_back(config_window = new AppWindowConfigWindow());
		editor_windows.push_back(style_editor_window = new EditorStyleWindow());
		editor_windows.push_back(renderer_config_window = new RendererConfigWindow());
		editor_windows.push_back(input_config_window = new InputConfigWindow());
		editor_windows.push_back(tags_and_layers_window = new TagsAndLayersWindow());
		editor_windows.push_back(import_window = new ImportWindow());
		editor_windows.push_back(resources_window = new ResourcesWindow());
		editor_windows.push_back(resources_config_window = new ResourcesConfigWindow());
		editor_windows.push_back(text_editor_window = new TextEditorWindow());
		editor_windows.push_back(audio_window = new AudioWindow());

		//editor_panels.push_back(animator_panel = new PanelAnimator());
		//editor_panels.push_back(particle_editor_panel = new PanelParticleEditor());
		ImGui::LoadDocks();
		drag_data = new DragData();
	}
	
	editor_windows.push_back(game_window = new GameWindow());

	return true;
}

update_status ModuleEditor::PreUpdate(float delta_time)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuizmo::BeginFrame();
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float deltaTime)
{
	bool ret = true;
	if (!App->IsGame())
	{
		ms_timer.Start();
		ImGui::PushFont(font);
		if (ImGui::BeginMainMenuBar())
		{
			bool selected = false;
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 previous_color = style.Colors[ImGuiCol_Text];
			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			if (ImGui::BeginMenu("File"))
			{
				style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

				if (ImGui::MenuItem("New Scene"))
				{
					App->scene->NewScene(false);
				}
				if (ImGui::MenuItem("Load Scene"))
				{
					char const * lFilterPatterns[2] = { "*.scene; *.json"};
					const char* path = tinyfd_openFileDialog("Load Scene...", NULL, 1, lFilterPatterns, NULL, 0);
					if (path != NULL) 
						App->scene->LoadScene(path);
					
				}
				if (ImGui::MenuItem("Load Scene on current"))
				{
					char const * lFilterPatterns[2] = { "*.scene; *.json" };
					const char* path = tinyfd_openFileDialog("Load Scene...", NULL, 1, lFilterPatterns, NULL, 0);
					if (path != NULL)
						App->scene->LoadSceneWithoutDestroying(path);
				}

				if (ImGui::MenuItem("Save Scene (JSON)"))
				{
					char const * lFilterPatterns[1] = { "*.json" };
					const char* path = tinyfd_saveFileDialog("Save Scene...", (App->scene->scene_name).c_str(), 1, lFilterPatterns, NULL);
					if (path != NULL)
					{
						std::string new_scene_name = App->file_system->GetFileNameWithoutExtension(path);
						App->scene->scene_name = new_scene_name;
						App->window->SetTitle((SCENE_TITLE_PREFIX + new_scene_name).c_str());
						std::string final_path = App->file_system->ChangeFileExtension(path, "json");
						App->scene->SaveScene(final_path, SceneFileType::SF_JSON);
					}
				}
				if (ImGui::MenuItem("Save Scene (BINARY) -> (SAVE YOUR SCENES AS JSON NOW)"))
				{
					char const * lFilterPatterns[1] = { "*.scene" };
					const char* path = tinyfd_saveFileDialog("Save Scene...", (App->scene->scene_name).c_str(), 1, lFilterPatterns, NULL);
					if (path != NULL) 
					{
						std::string new_scene_name = App->file_system->GetFileNameWithoutExtension(path);
						App->scene->scene_name = new_scene_name;
						App->window->SetTitle((SCENE_TITLE_PREFIX + new_scene_name).c_str());
						std::string final_path = App->file_system->ChangeFileExtension(path, "scene");
						App->scene->SaveScene(final_path, SceneFileType::SF_BINARY);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {
					return UPDATE_STOP;
				}

				ImGui::EndMenu();
				style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			}

			if (ImGui::BeginMenu("Windows"))
			{
				style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				if (ImGui::BeginMenu("Configuration"))
				{
					if (ImGui::MenuItem("Window Config"))
					{
						config_window->active = !config_window->active;
					}
					if (ImGui::MenuItem("Renderer Config"))
					{
						renderer_config_window->active = !renderer_config_window->active;
					}
					if (ImGui::MenuItem("Input Config"))
					{
						input_config_window->active = !input_config_window->active;
					}
					if (ImGui::MenuItem("Tags & Layers"))
					{
						tags_and_layers_window->active = !tags_and_layers_window->active;
					}
					if (ImGui::MenuItem("Resource Management"))
					{
						resources_config_window->active = !resources_config_window->active;
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Hardware"))
				{
					hardware_window->active = !hardware_window->active;
				}
				if (ImGui::MenuItem("Performance"))
				{
					performance_window->active = !performance_window->active;
				}
				if (ImGui::MenuItem("Editor Style"))
				{
					style_editor_window->active = !style_editor_window->active;
				}
				if (ImGui::MenuItem("Audio Engine"))
				{
					audio_window->active = !audio_window->active;
				}

				ImGui::EndMenu();
				style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			}

			if (ImGui::BeginMenu("Help"))
			{
				style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				if (ImGui::MenuItem("About..."))
				{
					about_window->active = !about_window->active;
				}
				if (ImGui::MenuItem("Documentation"))
				{
					OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/wiki");
				}
				if (ImGui::MenuItem("Download Latest"))
				{
					OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/releases");
				}
				if (ImGui::MenuItem("Report a bug"))
				{
					OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/issues");
				}
				ImGui::EndMenu();
				style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			}
			ImGui::EndMainMenuBar();
			style.Colors[ImGuiCol_Text] = previous_color;
		}

		ImVec2 display_size = ImGui::GetIO().DisplaySize;
		display_size.y -= 10;
		ImGui::SetNextWindowSize(display_size);
		ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImGui::Begin("PanelEditor", NULL, ImVec2(0, 0), 1.0f, ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

		ImGui::SameLine(ImGui::GetIO().DisplaySize.x / 2 - 75);
		if (!App->IsPlaying())
		{
			if (App->IsPaused())
			{
				if (ImGui::Button("Paused", { 50,40 })) {
					App->UnPause();
					App->time->time_scale = 1.0f;
				}
			}
			else
			{
				if (ImGui::Button("Play", { 50,40 })) {
					App->Play();
					App->time->time_scale = 1.0f;
				}
			}
		}
		else
		{
			if (ImGui::Button("Pause", { 50,40 })) {
				App->Pause();
				App->time->time_scale = 0.0f;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", { 50,40 })) {
			App->Stop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Step", { 50,40 })) {
			App->UpdateStep();
		}

		ImGui::Separator();
		ImGui::BeginDockspace();
		for (std::list<Window*>::iterator it = editor_windows.begin(); it != editor_windows.end(); it++) {
			if ((*it)->IsActive())
			{
				(*it)->DrawWindow();
			}
		}
		ImGui::EndDockspace();
		ImGui::End();
		ImGui::PopFont();
		if (ImGui::IsMouseReleased(0) && drag_data->hasData)
		{
			drag_data->clearData();
			App->SetCustomCursor(App->ENGINE_CURSOR_ARROW);
		}
		performance_window->AddModuleData(this->name, ms_timer.ReadMs());
	}
	else
	{
		if (game_window)
		{
			game_window->DrawWindow();
		}
	}
	return UPDATE_CONTINUE;
}

bool ModuleEditor::DrawEditor()
{
	ImGui::Render();
	return true;
}

bool ModuleEditor::CleanUp()
{
	for (std::list<Window*>::iterator it = editor_windows.begin(); it != editor_windows.end(); ++it) {
		RELEASE(*it);
	}
	RELEASE(drag_data);
	editor_windows.clear();
	if (!App->IsGame())
	{
		ImGui::SaveDocks();
	}
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleEditor::HandleInput(SDL_Event * event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

void ModuleEditor::OpenBrowserPage(const char * url)
{
	//ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void ModuleEditor::AddData_Editor(float ms, float fps)
{
	performance_window->AddData(ms, fps);
}

void ModuleEditor::LoadEditorStyle()
{
	Data data;

	if (data.LoadJSON(EDITOR_STYLE_FILE))
	{
		ImGuiStyle * style = &ImGui::GetStyle();
		if (data.EnterSection("Editor_Style"))
		{
			style->Alpha = data.GetFloat("Alpha");
			style->WindowPadding = data.GetImVector2("WindowPadding");
			style->WindowMinSize = data.GetImVector2("WindowMinSize");
			style->WindowRounding = data.GetFloat("WindowRounding");
			style->WindowTitleAlign = data.GetImVector2("WindowTitleAlign");
			style->ChildWindowRounding = data.GetFloat("ChildWindowRounding");
			style->FramePadding = data.GetImVector2("FramePadding");
			style->FrameRounding = data.GetFloat("FrameRounding");
			style->ItemSpacing = data.GetImVector2("ItemSpacing");
			style->ItemInnerSpacing = data.GetImVector2("ItemInnerSpacing");
			style->TouchExtraPadding = data.GetImVector2("TouchExtraPadding");
			style->IndentSpacing = data.GetFloat("IndentSpacing");
			style->ColumnsMinSpacing = data.GetFloat("ColumnsMinSpacing");
			style->ScrollbarSize = data.GetFloat("ScrollbarSize");
			style->ScrollbarRounding = data.GetFloat("ScrollbarRounding");
			style->GrabMinSize = data.GetFloat("GrabMinSize");
			style->GrabRounding = data.GetFloat("GrabRounding");
			style->ButtonTextAlign = data.GetImVector2("ButtonTextAlign");
			style->DisplayWindowPadding = data.GetImVector2("DisplayWindowPadding");
			style->DisplaySafeAreaPadding = data.GetImVector2("DisplaySafeAreaPadding");
			style->AntiAliasedLines = data.GetBool("AntiAliasedLines");
			style->AntiAliasedShapes = data.GetBool("AntiAliasedShapes");
			style->CurveTessellationTol = data.GetFloat("CurveTessellationTol");

			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				style->Colors[i] = data.GetImVector4(ImGui::GetStyleColorName(i));
			}
			data.LeaveSection();
		}
	}
}

