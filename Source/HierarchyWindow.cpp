#include "HierarchyWindow.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"

HierarchyWindow::HierarchyWindow()
{
	active = true;
	window_name = "Hierarchy";
	gameobject_to_rename = nullptr;
	show_rename_window = false;
	show_rename_error = false;
	open_gameobject_node = nullptr;
}

HierarchyWindow::~HierarchyWindow()
{
	RELEASE(gameobject_to_rename);
}

void HierarchyWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, App->IsPlaying(), ImGuiWindowFlags_HorizontalScrollbar)) {
		if (ImGui::IsMouseClicked(1) && ImGui::IsMouseHoveringWindow() && !App->IsPlaying()) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::OpenPopup("GameObject Options");
		}

		if (ImGui::BeginPopup("GameObject Options"))
		{
			if (!App->scene->selected_gameobjects.empty()) {
				if (ImGui::MenuItem("Duplicate")) {
					for (std::list<GameObject*>::iterator it = App->scene->selected_gameobjects.begin(); it != App->scene->selected_gameobjects.end(); it++) {
						if ((*it)->GetParent() != nullptr) {
							if (std::find(App->scene->selected_gameobjects.begin(), App->scene->selected_gameobjects.end(), (*it)->GetParent()) != App->scene->selected_gameobjects.end()) {
								continue; //If parent will be duplicated skip this because parent will take care of childs;
							}
						}
						App->scene->DuplicateGameObject(*it);
					}
				}
				if (ImGui::MenuItem("Delete")) {
					for (std::list<GameObject*>::iterator it = App->scene->selected_gameobjects.begin(); it != App->scene->selected_gameobjects.end(); it++) {
						(*it)->Destroy();
					}
				}
				if (App->scene->selected_gameobjects.size() == 1) {
					if (ImGui::MenuItem("Rename")) {
						show_rename_window = true;
						gameobject_to_rename = App->scene->selected_gameobjects.front();
						rename_window_y = ImGui::GetMousePos().y;
					}
					if (ImGui::MenuItem("Create Child")) {
						GameObject* parent = nullptr;
						parent = App->scene->selected_gameobjects.front();
						App->scene->CreateGameObject(parent);
						open_gameobject_node = parent;
					}
					if (ImGui::MenuItem("Create prefab")) {
						App->scene->CreatePrefab(App->scene->selected_gameobjects.front());
					}
				}
				ImGui::Separator();
			}
			if (ImGui::MenuItem("Create Game Object")) {
				App->scene->CreateGameObject(nullptr);
			}
			if (ImGui::MenuItem("Create Light Object")) {
				App->scene->CreateLightObject(nullptr);
			}

			ImGui::EndPopup();
		}

		if (show_rename_window) {
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x + 20, rename_window_y));
			bool windowActive = true;
			ImGui::Begin("Rename Game Object", &windowActive,
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_ShowBorders |
				ImGuiWindowFlags_NoTitleBar);

			bool confirmed = false;
			ImGui::Text("Enter new name");
			static char inputText[20];
			if (ImGui::InputText("", inputText, 20, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				confirmed = true;
			}
			if (show_rename_error) {
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Name cannot be blank");
			}
			if (ImGui::Button("Confirm")) {
				confirmed = true;
			}
			if (confirmed)
			{
				bool isBlankString = true;
				for (int i = 0; inputText[i] != '\0'; i++) {
					if (inputText[i] != ' ') {
						isBlankString = false;
						break;
					}
				}
				if (!isBlankString) {
					App->scene->selected_gameobjects.front()->SetName(inputText);
					App->scene->RenameDuplicatedGameObject(App->scene->selected_gameobjects.front());
					show_rename_error = false;
					show_rename_window = false;
				}
				else {
					show_rename_error = true;
				}
				inputText[0] = '\0';
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				show_rename_window = false;
				inputText[0] = '\0';
			}
			ImGui::End();
		}

		for (std::list<GameObject*>::iterator it = App->scene->root_gameobjects.begin(); it != App->scene->root_gameobjects.end(); it++) {
			DrawSceneGameObjects(*it);
		}

		if (ImGui::IsMouseReleased(0) && ImGui::IsMouseHoveringWindow() && !ImGui::IsAnyItemHovered() && App->editor->drag_data->hasData && 
			App->editor->drag_data->fromPanel == "Hierarchy")
		{
			GameObject* go = (GameObject*)App->editor->drag_data->resource;
			if (go)
			{
				go->SetParent(nullptr);
			}
		}
	}

	ImGui::EndDock();
}

void HierarchyWindow::DrawSceneGameObjects(GameObject * gameObject)
{
	uint flag = 0;

	if (gameObject->childs.empty()) {
		flag |= ImGuiTreeNodeFlags_Leaf;
	}

	for (std::list<GameObject*>::iterator it = App->scene->selected_gameobjects.begin(); it != App->scene->selected_gameobjects.end(); it++) {
		if (gameObject == *it) {
			flag |= ImGuiTreeNodeFlags_Selected;
			break;
		}
	}

	flag |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (open_gameobject_node == gameObject) {
		ImGui::SetNextTreeNodeOpen(true);
		open_gameobject_node = nullptr;
	}

	if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), flag))
	{
		IsMouseOver(gameObject);
		for (std::list<GameObject*>::iterator it = gameObject->childs.begin(); it != gameObject->childs.end(); ++it) {
			DrawSceneGameObjects(*it);
		}
		ImGui::TreePop();
	}
	else 
	{
		IsMouseOver(gameObject);
	}
}

void HierarchyWindow::IsMouseOver(GameObject * gameObject)
{
	if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) {
		if (ImGui::IsItemRectHovered())
		{
			std::list<GameObject*>::iterator it;

			if (!App->scene->selected_gameobjects.empty()) {
				it = std::find(App->scene->selected_gameobjects.begin(), App->scene->selected_gameobjects.end(), gameObject);
				if (it == App->scene->selected_gameobjects.end()) {
					if (App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT) App->scene->selected_gameobjects.clear();
					App->scene->selected_gameobjects.push_back(gameObject);
					gameObject->SetSelected(true);
				}
			}
			else {
				App->scene->selected_gameobjects.push_back(gameObject);
				gameObject->SetSelected(true);
			}
		}
		else {
			if (ImGui::IsMouseHoveringWindow() && !ImGui::IsMouseClicked(1)) {
				if (App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT && !App->scene->selected_gameobjects.empty() &&
					!show_rename_error) {
					App->scene->selected_gameobjects.remove(gameObject);
					gameObject->SetSelected(false);
				}
			}
		}
	}

	if (ImGui::IsMouseDoubleClicked(0) && !App->IsPlaying())
	{
		if (ImGui::IsItemHoveredRect())
		{
			if (!show_rename_window) {
				show_rename_window = true;
				rename_window_y = ImGui::GetMousePos().y;
			}
		}
	}

	if (App->input->IsMouseDragging(SDL_BUTTON_LEFT) && App->scene->selected_gameobjects.size() == 1 && ImGui::IsItemHoveredRect() && !App->editor->drag_data->hasData)
	{
		App->editor->drag_data->fromPanel = "Hierarchy";
		App->editor->drag_data->hasData = true;
		App->editor->drag_data->resource = (Resource*)gameObject;
	}

	if (App->editor->drag_data->hasData && ImGui::IsItemHoveredRect())
	{
		if (App->editor->drag_data->fromPanel == "Hierarchy")
		{
			if (ImGui::IsMouseReleased(0))
			{
				GameObject* go = (GameObject*)App->editor->drag_data->resource;
				if (go != nullptr && go != gameObject)
				{
					go->SetParent(gameObject);
				}
				App->editor->drag_data->clearData();
				App->SetCustomCursor(App->ENGINE_CURSOR_ARROW);
			}
			else
			{
				App->SetCustomCursor(App->ENGINE_CURSOR_ADD);
			}
		}
	}
}
