#include "SceneWindow.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "RenderTexture.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "RenderTextureMSAA.h"
#include "ComponentRectTransform.h"
#include "ModuleInput.h"

SceneWindow::SceneWindow()
{
	active = true;
	window_name = "Scene";
	scene_width = 0;
	scene_height = 0;
	wireframe_mode = false;
	last_used_scale = { 1,1,1 };
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - Scene: DrawWindow", Profiler::Color::Black);

	if (ImGui::BeginDock(window_name.c_str(), false, false, false,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar)) 
	{

		ImVec2 size = ImGui::GetContentRegionAvail();
		if (scene_width != size.x || scene_height != size.y) {
			App->renderer3D->OnResize(size.x, size.y, App->renderer3D->editor_camera);
			scene_width = size.x;
			scene_height = size.y;
		}
		window_pos = ImGui::GetWindowPos();
		DrawMenuBar();

		if (App->renderer3D->editor_camera != nullptr && App->renderer3D->editor_camera->GetViewportTexture() != nullptr)
		{
			ImGui::Image((void*)App->renderer3D->editor_camera->GetViewportTexture()->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
		}
		
		if (!App->renderer3D->rendering_cameras.empty() && App->renderer3D->rendering_cameras.back() != nullptr && 
			App->renderer3D->rendering_cameras.back()->GetViewportTexture() != nullptr && App->renderer3D->rendering_cameras.back()->GetGameObject()->IsSelected())
		{
			ImVec2 preview_x_y = { window_pos.x + size.x - 300, window_pos.y + size.y - 100 };
			ImVec2 preview_w_h = { window_pos.x + size.x, window_pos.y + size.y + 25};
			ImVec2 preview_back_x_y = { preview_x_y.x - 5, preview_x_y.y - 10 };
			ImVec2 preview_back_w_h = { preview_w_h.x + 5, preview_w_h.y + 5 };

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(preview_back_x_y, preview_back_w_h, ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1)));
			draw_list->AddImage((void*)App->renderer3D->rendering_cameras.back()->GetViewportTexture()->GetTextureID(), preview_x_y, preview_w_h, ImVec2(0, 1), ImVec2(1, 0));
		}

		is_mouse_hovering_window = ImGui::IsMouseHoveringWindow();
		//Necessary because left-click doesn't give focus to a window
		if (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))
		{
			if(is_mouse_hovering_window) is_window_focused = true;
			else is_window_focused = false;
		}


		//GUIZMO
		if (!App->scene->selected_gameobjects.empty())
		{
			ImGuizmo::Enable(true);
			if(App->scene->selected_gameobjects.front()->IsStatic()) ImGuizmo::Enable(false);
			
			float4x4 selected_matrix = App->scene->selected_gameobjects.front()->GetGlobalTransfomMatrix().Transposed();
			float transformation[16];

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, size.y);
			ImGuizmo::SetDrawlist();
			ImGuizmo::Manipulate(App->renderer3D->editor_camera->GetViewMatrix(), App->renderer3D->editor_camera->GetProjectionMatrix(),
					App->scene->mCurrentGizmoOperation, App->scene->mCurrentGizmoMode, selected_matrix.ptr(), transformation);

			if (ImGuizmo::IsOver() && ImGuizmo::IsUsing())
			{
				float translation[3];
				float rotation[3];
				float scale[3];

				ImGuizmo::DecomposeMatrixToComponents(transformation, translation, rotation, scale);

				ComponentTransform* transform = (ComponentTransform*)App->scene->selected_gameobjects.front()->GetComponent(Component::CompTransform);

				float3 go_position;
				float3 go_rotation;
				float3 go_scale;
				if (transform->GetGameObject()->IsRoot()) {
					go_position = transform->GetGlobalPosition();
					go_rotation = transform->GetGlobalRotation();
					go_scale = transform->GetGlobalScale();
				}
				else {
					go_position = transform->GetLocalPosition();
					go_rotation = transform->GetLocalRotation();
					go_scale = transform->GetLocalScale();
				}

				float3 f3_translate(translation[0], translation[1], translation[2]);
				float3 f3_rotation(rotation[0], rotation[1], rotation[2]);
				float3 f3_scale(scale[0], scale[1], scale[2]);

				bool update_ui = false;
				switch (App->scene->mCurrentGizmoOperation)
				{
					case ImGuizmo::OPERATION::TRANSLATE:
						if (App->scene->selected_gameobjects.front()->GetParent() != nullptr)
						{
							f3_translate = App->scene->selected_gameobjects.front()->GetParent()->GetGlobalTransfomMatrix().Inverted().TransformPos(f3_translate);
						}
						transform->SetPosition(go_position + f3_translate);

						update_ui = true;
						break;
					case ImGuizmo::OPERATION::ROTATE:
						if (App->scene->selected_gameobjects.front()->GetParent() != nullptr)
						{
							f3_rotation = App->scene->selected_gameobjects.front()->GetParent()->GetGlobalTransfomMatrix().Inverted().TransformPos(f3_rotation);
						}
						transform->SetRotation(go_rotation + f3_rotation);

						update_ui = true;
						break;
					case ImGuizmo::OPERATION::SCALE:
						if (last_used_scale.x != f3_scale.x || last_used_scale.y != f3_scale.y || last_used_scale.z != f3_scale.z)
						{
							float3 tmp = f3_scale;
							f3_scale -= last_used_scale;
							last_used_scale = tmp;
							transform->SetScale(go_scale + f3_scale);
						}

						update_ui = true;
						break;
				}

				if (update_ui)
				{
					// Update rect transform on UI
					if (App->scene->selected_gameobjects.front()->GetIsUI())
					{
						ComponentRectTransform* c_rect_trans = (ComponentRectTransform*)App->scene->selected_gameobjects.front()->GetComponent(Component::CompRectTransform);

						if (c_rect_trans != nullptr)
							c_rect_trans->UpdateRectTransform();
					}
				}
			}
			else
			{
				last_used_scale = { 1,1,1 };
			}
		}
	}

	ImGui::EndDock();
}

void SceneWindow::DrawMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Shading Mode"))
		{
			if (ImGui::MenuItem("Shaded", "", !wireframe_mode))
			{
				wireframe_mode = false;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			if (ImGui::MenuItem("Wireframe", "", wireframe_mode))
			{
				wireframe_mode = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Anti Aliasing level"))
		{
			int curr_msaa_level = App->renderer3D->editor_camera->GetViewportTexture()->GetCurrentMSAALevel();
			bool selected = false;
			for (int i = 0; i <= 8; i *= 2) //cap the max MSAA level at x8
			{
				char msaa_level[5];
				sprintf(msaa_level, "x%d", i);
				if (i == curr_msaa_level) selected = true;
				if (ImGui::MenuItem(msaa_level, "", selected))
				{
					App->renderer3D->editor_camera->GetViewportTexture()->ChangeMSAALevel(i);
				}
				if (i == 0) i++;
				selected = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Gizmos"))
		{
			bool selected = App->scene->draw_octree;
			if (ImGui::MenuItem("Octree", "", selected))
			{
				App->scene->draw_octree = !selected;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Time"))
		{
			if (ImGui::SliderFloat("Dt Scale", &App->time->time_scale, 0.0f, 100.0f))
			{
				if (App->time->time_scale < 0.0f) App->time->time_scale = 0.0f;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Effects"))
		{
			bool skybox = App->renderer3D->IsUsingSkybox();
			if (ImGui::MenuItem("Skybox", "", skybox))
			{
				App->renderer3D->ActiveSkybox(!skybox);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera Speed"))
		{
			ImGui::DragFloat("##camera speed", &App->camera->speed);
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (wireframe_mode)
	{
		App->renderer3D->SetWireframeMode();
	}
}

bool SceneWindow::IsWindowFocused() const
{
	return is_window_focused;
}

bool SceneWindow::IsMouseHoveringWindow() const
{
	return is_mouse_hovering_window;
}

ImVec2 SceneWindow::GetWindowSize() const
{
	return ImVec2(scene_width, scene_height);
}

ImVec2 SceneWindow::GetWindowPos() const
{
	return window_pos;
}
