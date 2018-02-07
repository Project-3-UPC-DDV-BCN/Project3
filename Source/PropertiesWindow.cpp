#include "PropertiesWindow.h"
#include "Application.h"
#include "GameObject.h"
#include "MathGeoLib\MathGeoLib.h"
#include "ComponentCamera.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "TagsAndLayers.h"
#include "ModuleEditor.h"
#include "TagsAndLayersWindow.h"
#include "ComponentBillboard.h"
#include "ModuleScene.h"
#include "Mesh.h"
#include "Material.h"
#include "imgui/CustomImGui.h"
#include "ComponentParticleEmmiter.h"
#include "ModuleRenderer3D.h"
#include "ComponentScript.h"
#include "Script.h"
#include "CSScript.h"
#include "ModuleResources.h"
#include "ComponentFactory.h"
#include "ShaderProgram.h"
#include "Shader.h"

PropertiesWindow::PropertiesWindow()
{
	active = true;
	window_name = "Properties";
	scripts_count = 0;
	factories_count = 0;
}

PropertiesWindow::~PropertiesWindow()
{
}

void PropertiesWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, App->IsPlaying(), ImGuiWindowFlags_HorizontalScrollbar)) {
		GameObject* selected_gameobject = nullptr;
		if (App->scene->selected_gameobjects.size() == 1) {
			selected_gameobject = App->scene->selected_gameobjects.front();
		}
		else if (App->scene->selected_gameobjects.size() > 1) {
			ImGui::Text("More than 1 GameObject selected!");
		}

		if (selected_gameobject != nullptr && !selected_gameobject->is_on_destroy) {
			ImGui::Text("Name: %s", selected_gameobject->GetName().c_str());
			bool is_gameobject_active = selected_gameobject->IsActive();

			if (ImGui::Checkbox("Active", &is_gameobject_active)) {
				selected_gameobject->SetActive(is_gameobject_active);
			}
			ImGui::SameLine();
			bool is_static = selected_gameobject->IsStatic();
			if (ImGui::Checkbox("Static", &is_static))
			{
				selected_gameobject->SetStatic(is_static);
			}
			ImGui::Text("Tag:");
			ImGui::SameLine();
			if (ImGui::SmallButton((selected_gameobject->GetTag() + "##tags").c_str())) {
				ImGui::OpenPopup("Tags##gameobject");
			}
			if (ImGui::BeginPopup("Tags##gameobject")) {
				for (int i = 0; i < App->tags_and_layers->tags_list.size(); i++) {
					std::string name = App->tags_and_layers->tags_list[i];
					if (ImGui::MenuItem(name.c_str())) {
						if (name == "Main Camera" && App->renderer3D->game_camera == nullptr)
						{
							selected_gameobject->SetTag(name);
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Add Tag##gameobject")) {
					App->editor->tags_and_layers_window->active = true;
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::Text("Layer:");
			ImGui::SameLine();
			if (ImGui::SmallButton((selected_gameobject->GetLayer() + "##layers").c_str())) {
				ImGui::OpenPopup("Layers##gameobject");
			}
			if (ImGui::BeginPopup("Layers##gameobject")) {
				for (int i = 0; i < App->tags_and_layers->layers_list.size(); i++) {
					std::string name = App->tags_and_layers->layers_list[i];
					if (ImGui::MenuItem(name.c_str())) {
						selected_gameobject->SetLayer(name);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Add Layer##gameobject")) {
					App->editor->tags_and_layers_window->active = true;
				}
				ImGui::EndPopup();
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			for (std::list<Component*>::iterator it = selected_gameobject->components_list.begin(); it != selected_gameobject->components_list.end(); it++) {
				DrawComponent((*it));
				ImGui::Separator();
				ImGui::Spacing();
			}

			if (ImGui::Button("Add Component")) {
				ImGui::OpenPopup("Components");
			}

			if (ImGui::BeginPopup("Components"))
			{
				if (ImGui::MenuItem("Mesh Renderer")) {
					if (selected_gameobject->GetComponent(Component::CompMeshRenderer) == nullptr) {
						selected_gameobject->AddComponent(Component::CompMeshRenderer);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Mesh Renderer!");
					}
				}
				if (ImGui::MenuItem("Camera")) {
					if (selected_gameobject->GetComponent(Component::CompCamera) == nullptr) {
						selected_gameobject->AddComponent(Component::CompCamera);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Camera!");
					}
				}

				if (ImGui::MenuItem("Particle Emmiter")) {
					if (selected_gameobject->GetComponent(Component::CompParticleSystem) == nullptr) {
						selected_gameobject->AddComponent(Component::CompParticleSystem);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Particle Emmiter!");
					}
				}

				if (ImGui::MenuItem("Billboard")) {
					if (selected_gameobject->GetComponent(Component::CompBillboard) == nullptr) {
						selected_gameobject->AddComponent(Component::CompBillboard);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Billboard!");
					}
				}

				if (ImGui::BeginMenu("Script")) {
					std::map<uint, Script*> scripts = App->resources->GetScriptsList();
					Script* script = nullptr;

					if (ImGui::BeginMenu("C#"))
					{
						if (scripts.empty())
						{
							ImGui::MenuItem("No Scripts##NoCSScript");
						}
						else
						{
							for (std::map<uint, Script*>::iterator it = scripts.begin(); it != scripts.end(); it++)
							{
								if (it->second->GetScriptType() == Script::CsScript)
								{
									if (ImGui::MenuItem(it->second->GetName().c_str()))
									{
										script = it->second;
									}
								}
							}
						}
						ImGui::EndMenu();
					}

					if (script != nullptr)
					{
						ComponentScript* comp_script = (ComponentScript*)selected_gameobject->AddComponent(Component::CompScript);
						if (comp_script != nullptr)
						{
							script->SetAttachedGameObject(selected_gameobject);
							comp_script->SetScript(script);
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("New Factory")) {
					static char input_text[30];
					ImGui::InputText("Factory Name", input_text, 30);
					ImGui::Spacing();
					if (ImGui::Button("Create"))
					{
						ComponentFactory* factory = (ComponentFactory*)selected_gameobject->AddComponent(Component::CompFactory);
						if (factory) factory->SetName(input_text);
					}
					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}
		}
	}

	ImGui::EndDock();
}

void PropertiesWindow::DrawComponent(Component * component)
{
	switch (component->GetType())
	{
	case Component::CompTransform:
		DrawTransformPanel((ComponentTransform*)component);
		break;
	case Component::CompCamera:
		DrawCameraPanel((ComponentCamera*)component);
		break;
	case Component::CompRigidBody:
		break;
	case Component::CompMeshRenderer:
		DrawMeshRendererPanel((ComponentMeshRenderer*)component);
		break;
	case Component::CompBoxCollider:
		break;
	case Component::CompCircleCollider:
		break;
	case Component::CompAudioSource:
		break;
	case Component::CompAnimaton:
		break;
	case Component::CompScript:
		DrawScriptPanel((ComponentScript*)component);
		break;
	case Component::CompParticleSystem:
		DrawParticleEmmiterPanel((ComponentParticleEmmiter*)component); 
		break;
	case Component::CompBillboard:
		DrawBillboardPanel((ComponentBillboard*)component);
		break;
	case Component::CompFactory:
		DrawFactoryPanel((ComponentFactory*)component);
		break;
	default:
		break;
	}
}

void PropertiesWindow::DrawTransformPanel(ComponentTransform * transform)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		float3 position;
		float3 rotation;
		float3 scale;
		if (transform->GetGameObject()->IsRoot()) {
			position = transform->GetGlobalPosition();
			rotation = transform->GetGlobalRotation();
			scale = transform->GetGlobalScale();
		}
		else {
			position = transform->GetLocalPosition();
			rotation = transform->GetLocalRotation();
			scale = transform->GetLocalScale();
		}
		bool is_static = !transform->GetGameObject()->IsStatic();
		if (ImGui::DragFloat3("Position", (float*)&position, is_static, 0.25f)) {
			transform->SetPosition(position);
		}
		if (ImGui::DragFloat3("Rotation", (float*)&rotation, is_static, 0.25f)) {
			transform->SetRotation(rotation);
		}
		if (ImGui::DragFloat3("Scale", (float*)&scale, is_static, 0.25f)) {
			transform->SetScale(scale);
		}
	}
}

void PropertiesWindow::DrawMeshRendererPanel(ComponentMeshRenderer * mesh_renderer)
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool is_active = mesh_renderer->IsActive();
		if (ImGui::Checkbox("Active##Mesh_Renderer", &is_active))
		{
			mesh_renderer->SetActive(is_active);
		}

		Mesh* mesh = mesh_renderer->GetMesh();
		if (ImGui::InputResourceMesh("Mesh", &mesh))
		{
			mesh_renderer->SetMesh(mesh);
		}

		Material* material = mesh_renderer->GetMaterial();
		if (ImGui::InputResourceMaterial("Material", &material))
		{
			mesh_renderer->SetMaterial(material);
		}

		if(ImGui::TreeNodeEx("Mesh Info", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			if (mesh_renderer->GetMesh() == nullptr)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Mesh not found");
				ImGui::TreePop();
				return;
			}
			ImGui::Text("Triangle Count: %d", mesh_renderer->GetMesh()->num_indices / 3);
			ImGui::Text("Vertex Count: %d", mesh_renderer->GetMesh()->num_vertices);
			ImGui::Text("Indices Count: %d", mesh_renderer->GetMesh()->num_indices);
			ImGui::Text("Normals: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0, 1, 0, 1), ("yes"));
			ImGui::Text("Colors: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0, 1, 0, 1), ("yes"));
			ImGui::Text("UV: "); ImGui::TextColored(ImVec4(0, 1, 0, 1), ("yes"));
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_OpenOnArrow))
		{
			if (mesh_renderer->GetMaterial() == nullptr)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not using a Material");
				ImGui::TreePop();
				return;
			}
			else
			{
				ImGui::Text("\tShaders");
				ShaderProgram* prog = mesh_renderer->GetMaterial()->GetShaderProgram();

				if (prog != nullptr)
				{
					Shader* vert = prog->GetVertexShader();
					if (ImGui::InputResourceShader("Vertex Shader", &vert, Shader::ShaderType::ST_VERTEX))
					{
						mesh_renderer->GetMaterial()->SetVertexShader(vert);
					}

					Shader* frag = prog->GetFragmentShader();
					if (ImGui::InputResourceShader("Fragment Shader", &frag, Shader::ShaderType::ST_FRAGMENT))
					{
						mesh_renderer->GetMaterial()->SetFragmentShader(frag);
					}
				}

			}
		ImGui::TreePop();
		}

		ImGui::Spacing();
	}
}

void PropertiesWindow::DrawCameraPanel(ComponentCamera * comp_camera)
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool is_active = comp_camera->IsActive();
		if (ImGui::Checkbox("Active##Camera", &is_active))
		{
			comp_camera->SetActive(is_active);
		}

		Color background_color = comp_camera->GetBackgroundColor();
		ImGui::Text("Background Color:");
		if (ImGui::ColorEdit4("##Background Color", &background_color.r))
		{
			comp_camera->SetBackgroundColor(background_color);
		}
		ImGui::Text("Culling Mask:");
		if (ImGui::Button("Select Layers")) {
			ImGui::OpenPopup("Layers##camera");
		}
		if (ImGui::BeginPopup("Layers##camera")) {
			for (int i = 0; i < App->tags_and_layers->layers_list.size(); i++) {
				std::string name = App->tags_and_layers->layers_list[i];
				bool selected = false;
				if (std::find(comp_camera->layers_to_draw.begin(), comp_camera->layers_to_draw.end(), name) != comp_camera->layers_to_draw.end()) selected = true;
				if (ImGui::MenuItem(name.c_str(), "", selected)) {
					if (selected) comp_camera->RemoveLayerToDraw(name);
					else comp_camera->AddLayerToDraw(name);
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Add Layer##camera")) {
				App->editor->tags_and_layers_window->active = true;
			}
			ImGui::EndPopup();
		}
		float fov = comp_camera->GetFOV();
		ImGui::Text("Field of View:");
		if (ImGui::SliderFloat("##Field of View", &fov, 1, 160))
		{
			comp_camera->SetFOV(fov);
		}
		float near_plane = comp_camera->GetNearPlaneDistance();
		float far_plane = comp_camera->GetFarPlanceDistance();
		ImGui::Text("Near Plane:");
		if (ImGui::DragFloat("##Near Plane", &near_plane, true, 0.025f, 0.01, far_plane - 0.1f))
		{
			comp_camera->SetNearPlaneDistance(near_plane);
		}
		ImGui::Text("Far Plane:");
		if (ImGui::DragFloat("##Far Plane", &far_plane, true, 0.025f, near_plane + 0.1f))
		{
			comp_camera->SetFarPlaneDistance(far_plane);
		}
		Rect viewport = comp_camera->GetViewport();
		ImGui::Text("Viewport:");
		if (ImGui::DragInt("X", &viewport.left, true, 0.025f, 0, 1))
		{
			comp_camera->SetViewport(viewport);
		}
		if (ImGui::DragInt("Y", &viewport.top, true, 0.025f, 0, 1))
		{
			comp_camera->SetViewport(viewport);
		}
		if (ImGui::DragInt("W", &viewport.right, true, 0.025f, 1, 0))
		{
			comp_camera->SetViewport(viewport);
		}
		if (ImGui::DragInt("H", &viewport.bottom, true, 0.025f, 1, 0))
		{
			comp_camera->SetViewport(viewport);
		}
		int render_order = comp_camera->GetRenderOrder();
		ImGui::Text("Render Order:");
		if (ImGui::DragInt("##Render Order", &render_order))
		{
			comp_camera->SetRenderOrder(render_order);
		}

		ImGui::Spacing();
	}
}

void PropertiesWindow::DrawScriptPanel(ComponentScript * comp_script)
{
	scripts_count++;
	if (ImGui::CollapsingHeader((comp_script->GetScriptName() + "##" + std::to_string(scripts_count)).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		bool is_active = comp_script->IsActive();
		std::string script_name = comp_script->GetScript() ? comp_script->GetScript()->GetName() : "0";
		if (ImGui::Checkbox(("Active##Script_" + script_name).c_str(), &is_active))
		{
			comp_script->SetActive(is_active);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Component##script"))
		{
			App->scene->selected_gameobjects.front()->DestroyComponent(comp_script);
		}

		Script* script = comp_script->GetScript();
		if (ImGui::InputResourceScript("Script", &script))
		{
			comp_script->SetScript(script);
		}
		ImGui::Spacing();

		std::vector<ScriptField*> script_fields = comp_script->GetScriptFields();

		for (std::vector<ScriptField*>::iterator it = script_fields.begin(); it != script_fields.end(); it++)
		{
			switch ((*it)->propertyType)
			{
			case ScriptField::Integer:
			{
				int i = comp_script->GetScript()->GetIntProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputInt(("##" + (*it)->fieldName).c_str(), &i)) {
					comp_script->GetScript()->SetIntProperty((*it)->fieldName.c_str(), i);
				}
			}
				break;
			case ScriptField::Decimal:
			{
				double d = comp_script->GetScript()->GetDoubleProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputFloat(("##" + (*it)->fieldName).c_str(), (float*)&d, 0.001f, 0.01f, 3)) {
					comp_script->GetScript()->SetDoubleProperty((*it)->fieldName.c_str(), d);
				}
			}
				break;
			case ScriptField::Float:
			{
				float f = comp_script->GetScript()->GetFloatProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputFloat(("##" + (*it)->fieldName).c_str(), &f, 0.01f, 0.1f, 3)) {
					comp_script->GetScript()->SetFloatProperty((*it)->fieldName.c_str(), f);
				}
			}
				break;
			case ScriptField::Bool:
			{
				bool b = comp_script->GetScript()->GetBoolProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::Checkbox(("##" + (*it)->fieldName).c_str(), &b)) {
					comp_script->GetScript()->SetBoolProperty((*it)->fieldName.c_str(), b);
				}
			}
				break;
			case ScriptField::String:
			{
				static char textToRender[256];
				std::string str = comp_script->GetScript()->GetStringProperty((*it)->fieldName.c_str());
				strncpy(textToRender, str.data(), str.size());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputText(("##" + (*it)->fieldName).c_str(), textToRender, 256)) {
					comp_script->GetScript()->SetStringProperty((*it)->fieldName.c_str(), textToRender);
				}
				memset(textToRender, 0, sizeof textToRender);
			}
				break;
			case ScriptField::GameObject:
			{
				GameObject* gameobject = comp_script->GetScript()->GetGameObjectProperty((*it)->fieldName.c_str());
				if (ImGui::InputResourceGameObject((" " + (*it)->fieldName).c_str(), &gameobject)) {
					comp_script->GetScript()->SetGameObjectProperty((*it)->fieldName.c_str(), gameobject);
				}
			}
				break;
			case ScriptField::Animation:
				break;
			case ScriptField::Vector2:
			{
				float2 v2 = comp_script->GetScript()->GetVec2Property((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::DragFloat2(("##" + (*it)->fieldName).c_str(), &v2[0], !App->IsPlaying(), 0.25f)) {
					comp_script->GetScript()->SetVec2Property((*it)->fieldName.c_str(), v2);
				}
			}
				break;
			case ScriptField::Vector3:
			{
				float3 v3 = comp_script->GetScript()->GetVec3Property((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::DragFloat3(("##" + (*it)->fieldName).c_str(), &v3[0], !App->IsPlaying(), 0.25f)) {
					comp_script->GetScript()->SetVec3Property((*it)->fieldName.c_str(), v3);
				}
			}
				break;
			case ScriptField::Vector4:
			{
				float4 v4 = comp_script->GetScript()->GetVec4Property((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::DragFloat4(("##" + (*it)->fieldName).c_str(), &v4[0], !App->IsPlaying(), 0.25f)) {
					comp_script->GetScript()->SetVec4Property((*it)->fieldName.c_str(), v4);
				}
			}
				break;
			case ScriptField::Texture:
				break;
			case ScriptField::Audio:
				break;
			}
		}
	}
}

void PropertiesWindow::DrawFactoryPanel(ComponentFactory * factory)
{
	factories_count++;
	if (ImGui::CollapsingHeader((factory->GetName() + "##" + std::to_string(factories_count)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		Prefab* prefab = factory->GetFactoryObject();
		if(ImGui::InputResourcePrefab("Factory Object", &prefab))
		{
			factory->SetFactoryObject(prefab);
		}

		int count = factory->GetObjectCount();
		if (ImGui::InputInt("Object Count", &count))
		{
			factory->SetObjectCount(count);
		}

		float life_time = factory->GetLifeTime();
		if (ImGui::DragFloat("Life Time", &life_time, true, 0.025f, 0))
		{
			factory->SetLifeTime(life_time);
		}
	}
}

void PropertiesWindow::DrawParticleEmmiterPanel(ComponentParticleEmmiter * current_emmiter)
{
	if (ImGui::CollapsingHeader("Component Particle Emmiter"))
	{
		bool active_bool = current_emmiter->IsActive();
		bool keeper = active_bool;

		ImGui::Checkbox("Active", &active_bool);

		if (keeper != active_bool)
			current_emmiter->SetActive(keeper);

		if (current_emmiter->IsActive())
		{
			ImGui::Separator();

			static int particle_template;
			ImGui::Combo("Templates", &particle_template, "Select Template\0Smoke\0Custom\0");

			if (particle_template == 0)
				return;

			switch (particle_template)
			{
			case 1:
				//Here we set properties for the particles to look like smoke 

				break;
			}

			bool update_root = false;

			if (ImGui::Button("PLAY"))
			{
				current_emmiter->SetSystemState(PARTICLE_STATE_PLAY);
				current_emmiter->Start();
			}

			ImGui::SameLine();

			if (ImGui::Button("STOP"))
			{
				current_emmiter->SetSystemState(PARTICLE_STATE_PAUSE);
			}

			ImGui::Text("Particle System State: "); ImGui::SameLine();

			if (current_emmiter->GetSystemState() == PARTICLE_STATE_PLAY)
				ImGui::TextColored({ 0,255,0,1 }, "PLAY");
			else
				ImGui::TextColored({ 255,0,0,1 }, "PAUSED");

			ImGui::Separator();

			if (ImGui::TreeNode("Emit Area"))
			{

				static bool show = current_emmiter->ShowEmmisionArea();
				ImGui::Checkbox("Show Emmiter Area", &show);
				current_emmiter->SetShowEmmisionArea(show);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Texture"))
			{
				
				static Texture* st_particle_texture = nullptr;
				ImGui::InputResourceTexture("Texture To Add", &st_particle_texture);

				if(ImGui::Button("Add To Stack"))
				{
					current_emmiter->GetRootParticle()->GetAnimationController()->AddToFrameStack(st_particle_texture); 
				}

				ImGui::Text("Frame Stack Size:"); ImGui::SameLine(); 		
				ImGui::Text(to_string(current_emmiter->GetRootParticle()->GetAnimationController()->GetNumFrames()).c_str());

				current_emmiter->GetRootParticle()->GetAnimationController()->PaintStackUI(); 

				ImGui::DragFloat("Time Step", &current_emmiter->GetRootParticle()->GetAnimationController()->timeStep, true, 0.1f, 0, 2.0f); 

				ImGui::Separator();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Color"))
			{

				static bool alpha_preview = true;
				ImGui::Checkbox("Alpha", &alpha_preview);

				int misc_flags = (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0);

				ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar;
				flags |= misc_flags;
				flags |= ImGuiColorEditFlags_RGB;

				ImGui::ColorPicker4("Current Color##4", (float*)&current_emmiter->color, flags);

				if (ImGui::TreeNode("Color Interpolation"))
				{
					current_emmiter->UpdateRootParticle();

					ImGui::DragInt4("Initial Color", &current_emmiter->initial_color[0], 1, 0, 255);

					if (ImGui::Button("Set Selected as Initial"))
					{
						current_emmiter->initial_color[0] = current_emmiter->color.r * 255;
						current_emmiter->initial_color[1] = current_emmiter->color.g * 255;
						current_emmiter->initial_color[2] = current_emmiter->color.b * 255;
						current_emmiter->initial_color[3] = current_emmiter->color.a * 255;
					}

					ImGui::DragInt4("Final Color", &current_emmiter->final_color[0], 1, 0, 255);

					if (ImGui::Button("Set Selected as Final"))
					{

						current_emmiter->final_color[0] = current_emmiter->color.r * 255;
						current_emmiter->final_color[1] = current_emmiter->color.g * 255;
						current_emmiter->final_color[2] = current_emmiter->color.b * 255;
						current_emmiter->final_color[3] = current_emmiter->color.a * 255;
					}

					ImGui::Separator();

					if (ImGui::Button("Apply"))
					{
						current_emmiter->apply_color_interpolation = true;

						Color initial(current_emmiter->initial_color[0], current_emmiter->initial_color[1], current_emmiter->initial_color[2], current_emmiter->initial_color[3]);
						Color final(current_emmiter->final_color[0], current_emmiter->final_color[1], current_emmiter->final_color[2], current_emmiter->final_color[3]);

						current_emmiter->GetRootParticle()->SetInitialColor(initial);
						current_emmiter->GetRootParticle()->SetFinalColor(final);

						current_emmiter->UpdateRootParticle();
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Motion"))
			{
				ImGui::Checkbox("Billboarding", &current_emmiter->billboarding); ImGui::SameLine(); 
				ImGui::Checkbox("Relative Position", &current_emmiter->relative_pos);

				ImGui::DragInt("Emmision Rate", &current_emmiter->emmision_rate, 1, 1, 0, 150);
				ImGui::DragFloat("Lifetime", &current_emmiter->max_lifetime, 1, 0.1f, 0, 20);
				ImGui::SliderFloat("Initial Velocity", &current_emmiter->velocity, 0.1f, 30); 
				ImGui::SliderFloat3("Gravity", &current_emmiter->gravity[0], -1, 1);
				ImGui::DragFloat("Angular Velocity", &current_emmiter->angular_v, 1, 5.0f, -1000, 1000);
				ImGui::DragFloat("Emision Angle", &current_emmiter->emision_angle, 1, 0, 360);

				current_emmiter->UpdateRootParticle();

				ImGui::Separator();

				ImGui::Text("Size Interpolation");

				ImGui::InputFloat("Initial", &current_emmiter->initial_scale.x);
				current_emmiter->initial_scale.y = current_emmiter->initial_scale.x;

				ImGui::InputFloat("Final", &current_emmiter->final_scale.x);
				current_emmiter->final_scale.y = current_emmiter->final_scale.x;

				if (ImGui::Button("Apply Scale"))
				{
					current_emmiter->apply_size_interpolation = true;
					current_emmiter->UpdateRootParticle();
				}

				ImGui::Separator();

				ImGui::Text("Rotation Interpolation");

				ImGui::Text("Initial Spins/sec");
				ImGui::DragFloat("  ", &current_emmiter->initial_angular_v, 0.5, 0, 5);

				ImGui::Text("Final Spins/sec");
				ImGui::DragFloat(" ", &current_emmiter->final_angular_v);

				if (ImGui::Button("Apply Rotation"))
				{
					current_emmiter->apply_rotation_interpolation = true;
					current_emmiter->UpdateRootParticle();
				}
				ImGui::SameLine();

				if (ImGui::Button("Stop Applying"))
				{
					current_emmiter->apply_rotation_interpolation = false;
					current_emmiter->UpdateRootParticle();
				}

				ImGui::Separator();

				if (ImGui::Button("Save as Template"))
				{

				}
				ImGui::TreePop();
			}

		}

	}
}

void PropertiesWindow::DrawBillboardPanel(ComponentBillboard * billboard)
{
	if (ImGui::CollapsingHeader("Component Billboard"))
	{
		static int billboard_type; 
		ImGui::Combo("Templates", &billboard_type, "Select Billboard Type\0Only on X\0Only on Y\0All Axis\0");
		
		if (billboard_type != 0)
		{
			billboard->SetBillboardType((BillboardingType)--billboard_type);
			++billboard_type;
		}
		else
			billboard->SetBillboardType(BILLBOARD_NONE); 

			
	}
}
