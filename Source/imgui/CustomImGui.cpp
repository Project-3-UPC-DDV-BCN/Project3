#include "CustomImGui.h"
#include "imgui_internal.h"
#include "../Texture.h"
#include "../Application.h"
#include "../ModuleEditor.h"
#include "../Mesh.h"
#include "../Prefab.h"
#include "../Material.h"
#include "../GameObject.h"
#include "../Script.h"
#include "../Shader.h"
#include "../Font.h"
#include "../PhysicsMaterial.h"
#include "../BlastModel.h"
#include "../SoundBankResource.h"
#include "../GOAPGoal.h"
#include "../GOAPAction.h"

namespace ImGui
{

	bool ImGui::InputResourceTexture(const char * label, Texture ** texture)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		Texture* tmp_texture = *texture;
		if (tmp_texture != nullptr)
		{
			buf_display = tmp_texture->GetName();
		}
		else
		{
			buf_display = "None(Texture)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::TextureResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*texture = (Texture*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();
		std::string button_id("+##texture_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::TextureResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Texture* new_texture = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->texture_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_texture = App->editor->resources_window->GetTexture();
			if (new_texture != tmp_texture)
			{
				*texture = new_texture;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}


	bool ImGui::InputResourceMesh(const char * label, Mesh ** mesh)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		Mesh* tmp_mesh = *mesh;
		if (tmp_mesh != nullptr)
		{
			buf_display = tmp_mesh->GetName();
		}
		else
		{
			buf_display = "None(Mesh)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());
		
		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::MeshResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*mesh = (Mesh*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		std::string button_id("+##mesh_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::MeshResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Mesh* new_mesh = nullptr;
		
		if (App->editor->resources_window->active && App->editor->resources_window->mesh_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_mesh = App->editor->resources_window->GetMesh();
			if (new_mesh != tmp_mesh)
			{
				*mesh = new_mesh;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool ImGui::InputResourcePrefab(const char * label, Prefab ** prefab)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		Prefab* tmp_prefab = *prefab;
		if (tmp_prefab != nullptr)
		{
			buf_display = tmp_prefab->GetName();
		}
		else
		{
			buf_display = "None(Prefab)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;
		ImGui::SameLine();

		std::string button_id("+##prefab_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::PrefabResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Prefab* new_prefab = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->prefab_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_prefab = App->editor->resources_window->GetPrefab();
			if (new_prefab != tmp_prefab)
			{
				*prefab = new_prefab;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool ImGui::InputResourceGameObject(const char * label, GameObject ** gameobject, ResourcesWindow::GameObjectFilter filter)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		GameObject* tmp_gameobject = *gameobject;
		if (tmp_gameobject != nullptr)
		{
			buf_display = tmp_gameobject->GetName();
		}
		else
		{
			buf_display = "None(GameObject)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::GameObjectResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*gameobject = (GameObject*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		std::string button_id("+##gameobject_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::GameObjectResource);
			App->editor->resources_window->go_filter = filter;
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		GameObject* new_gameoject = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->gameobject_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_gameoject = App->editor->resources_window->GetGameobject();
			if (new_gameoject != tmp_gameobject)
			{
				*gameobject = new_gameoject;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool ImGui::InputResourceMaterial(const char * label, Material ** material)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		Material* tmp_material = *material;
		if (tmp_material != nullptr)
		{
			buf_display = tmp_material->GetName();
		}
		else
		{
			buf_display = "None(Material)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::MaterialResource)
			{
				RenderFrame(rect.Min, rect.Max, ImGui::ColorConvertFloat4ToU32({ 0,0.8f,1,0.2f }));
				if (ImGui::IsMouseReleased(0))
				{
					*material = (Material*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		std::string button_id("+##material_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::MaterialResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Material* new_material = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->material_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_material = App->editor->resources_window->GetMaterial();
			if (new_material != tmp_material)
			{
				*material = new_material;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourceScript(const char * label, Script ** script)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		Script* tmp_script = *script;
		if (tmp_script != nullptr)
		{
			buf_display = tmp_script->GetName();
		}
		else
		{
			buf_display = "None(Script)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;
		ImGui::SameLine();

		std::string button_id("+##script_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::ScriptResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Script* new_script = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->script_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_script = App->editor->resources_window->GetScript();
			if (new_script != tmp_script)
			{
				*script = new_script;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourcePhysMaterial(const char * label, PhysicsMaterial ** phys_mat)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		PhysicsMaterial* tmp_material = *phys_mat;
		if (tmp_material != nullptr)
		{
			buf_display = tmp_material->GetName();
		}
		else
		{
			buf_display = "None(Phys Material)";
		}

		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::PhysicsMatResource)
			{
				RenderFrame(rect.Min, rect.Max, ImGui::ColorConvertFloat4ToU32({ 0,0.8f,1,0.2f }));
				if (ImGui::IsMouseReleased(0))
				{
					*phys_mat = (PhysicsMaterial*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		if (Button("+##phys_material", { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::PhysicsMatResource);
			App->editor->resources_window->SetActive(true);
		}

		PhysicsMaterial* new_material = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->phys_mat_changed)
		{
			new_material = App->editor->resources_window->GetPhysMat();
			if (new_material != tmp_material)
			{
				*phys_mat = new_material;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourceShader(const char * label, Shader ** shader, Shader::ShaderType type)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		std::string buf_display;
		Shader* tmp_shader = *shader;
		if (tmp_shader != nullptr)
		{
			buf_display = tmp_shader->GetName();
		}
		else
		{
			buf_display = "None(Shader)";
		}

		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::PhysicsMatResource)
			{
				RenderFrame(rect.Min, rect.Max, ImGui::ColorConvertFloat4ToU32({ 0,0.8f,1,0.2f }));
				if (ImGui::IsMouseReleased(0))
				{
					*shader = (Shader*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		char name[15];
		sprintf(name,"+##shader%d_", type);

		std::string button_id(name);
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetShaderType(type);
			App->editor->resources_window->SetResourceType(Resource::ShaderResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Shader* new_shader = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->script_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_shader = App->editor->resources_window->GetShader();
			if (new_shader != tmp_shader)
			{
				*shader = new_shader;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourceFont(const char * label, Font ** font)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		
		Font* tmp_font = *font;
		if (tmp_font != nullptr)
		{
			buf_display = tmp_font->GetFamilyName();
		}
		else
		{
			buf_display = "None(Font)";
		}
		
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::FontResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*font = (Font*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();
		
		std::string button_id("+##font_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::FontResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		Font* new_font = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->font_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_font = App->editor->resources_window->GetFont();
			if (new_font != tmp_font)
			{
				*font = new_font;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourceAudio(const char * label, SoundBankResource ** soundbank)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		
		SoundBankResource* tmp_soundbank = *soundbank;
		if (tmp_soundbank != nullptr)
		{
			buf_display = tmp_soundbank->GetName();
		}
		else
		{
			buf_display = "None(SoundBank)";
		}
		
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;
		
		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::SoundBankResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*soundbank = (SoundBankResource*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		std::string button_id("+##soundbank_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::SoundBankResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}
	
		SoundBankResource* new_soundbank = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->soundbank_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_soundbank = App->editor->resources_window->GetSoundBank();
			if (new_soundbank != tmp_soundbank)
			{
				*soundbank = new_soundbank;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}
		
		return false;
	}

	bool InputResourceGOAPGoal(const char * label, GOAPGoal ** goal)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;

		GOAPGoal* tmp_goal = *goal;
		if (tmp_goal != nullptr)
		{
			buf_display = tmp_goal->GetName();
		}
		else
		{
			buf_display = "None(GOAPGoal)";
		}

		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		ImGui::SameLine();

		std::string button_id("+##GOAPGoal_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::GOAPGoalResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		GOAPGoal* new_goal = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->goal_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_goal = App->editor->resources_window->GetGOAPGoal();
			if (new_goal != tmp_goal)
			{
				*goal = new_goal;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}

	bool InputResourceGOAPAction(const char * label, GOAPAction ** action)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;
		GOAPAction* tmp_action = *action;
		if (tmp_action != nullptr)
		{
			buf_display = tmp_action->GetName();
		}
		else
		{
			buf_display = "None(GOAPAction)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;
		ImGui::SameLine();

		std::string button_id("+##GOAPAction_");
		button_id += label;
		if (Button(button_id.c_str(), { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::GOAPActionResource);
			App->editor->resources_window->SetActive(true);
			App->editor->resources_window->SetCurrentInputName(button_id);
		}

		GOAPAction* new_action = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->action_changed && App->editor->resources_window->GetCurrentInputName() == button_id)
		{
			new_action = App->editor->resources_window->GetGOAPAction();
			if (new_action != tmp_action)
			{
				*action = new_action;
				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}
}


	bool ImGui::InputResourceBlastModel(const char * label, BlastModel ** mesh)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		Text(label);
		ImGui::SameLine();
		ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, label_size.y + style.FramePadding.y*2.0f));
		//window->Flags |= ImGuiWindowFlags_ShowBorders;
		RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_FrameBg));
		//window->Flags ^= ImGuiWindowFlags_ShowBorders;
		std::string buf_display;

		BlastModel* tmp_mesh = *mesh;
		if (tmp_mesh != nullptr)
		{
			buf_display = tmp_mesh->GetName();
		}
		else
		{
			buf_display = "None(Mesh)";
		}
		window->DrawList->AddText(g.Font, g.FontSize, window->DC.CursorPos, GetColorU32(ImGuiCol_Text), buf_display.c_str());

		ItemSize(rect, style.FramePadding.y);
		if (!ItemAdd(rect, &id))
			return false;

		if (ImGui::IsItemHovered() && App->editor->drag_data->hasData)
		{
			if (App->editor->drag_data->resource->GetType() == Resource::BlastMeshResource)
			{
				RenderFrame(rect.Min, rect.Max, GetColorU32(ImGuiCol_ButtonHovered));
				if (ImGui::IsMouseReleased(0))
				{
					*mesh = (BlastModel*)App->editor->drag_data->resource;
					return true;
				}
			}
		}
		ImGui::SameLine();

		if (Button("+##mesh", { 20, 20 }))
		{
			App->editor->resources_window->SetResourceType(Resource::MeshResource);
			App->editor->resources_window->SetActive(true);
		}

		BlastModel* new_mesh = nullptr;

		if (App->editor->resources_window->active && App->editor->resources_window->mesh_changed)
		{
			new_mesh = App->editor->resources_window->GetBlastModel();
			if (new_mesh != tmp_mesh)
			{
				*mesh = new_mesh;

				App->editor->resources_window->SetActive(false);
				App->editor->resources_window->Reset();
				return true;
			}
		}

		return false;
	}
