#include "PropertiesWindow.h"
#include "Application.h"
#include "GameObject.h"
#include "MathGeoLib\MathGeoLib.h"
#include "ModuleParticleImporter.h"
#include "ComponentCamera.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "TagsAndLayers.h"
#include "tinyfiledialogs.h"
#include "ModuleEditor.h"
#include "TagsAndLayersWindow.h"
#include "ComponentBillboard.h"
#include "ParticleData.h"
#include "ModuleScene.h"
#include "Mesh.h"
#include "Material.h"
#include <algorithm>
#include "imgui/CustomImGui.h"
#include "ComponentParticleEmmiter.h"
#include "ModuleRenderer3D.h"
#include "ComponentRectTransform.h"
#include "ComponentCanvas.h"
#include "ComponentScript.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "Script.h"
#include "CSScript.h"
#include "ModuleResources.h"
#include "ComponentFactory.h"
#include "ComponentRigidBody.h"
#include "ComponentCollider.h"
#include "PhysicsMaterial.h"
#include "ComponentJointDistance.h"
#include "BlastModel.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "ComponentAudioSource.h"
#include "ComponentListener.h"
#include "ComponentDistorsionZone.h"
#include "ComponentLight.h"
#include "ComponentProgressBar.h"
#include "ModulePhysics.h"
#include "ComponentButton.h"
#include "Texture.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

PropertiesWindow::PropertiesWindow()
{
	active = true;
	window_name = "Properties";
	scripts_count = 0;
	factories_count = 0;
	colliders_count = 0;
	distance_joints_count = 0;
	lights_count = 0;
}

PropertiesWindow::~PropertiesWindow()
{
}

void PropertiesWindow::DrawWindow()
{
	if (ImGui::BeginDock(window_name.c_str(), false, false, App->IsPlaying(), ImGuiWindowFlags_HorizontalScrollbar))
	{
		GameObject* selected_gameobject = nullptr;
		if (App->scene->selected_gameobjects.size() == 1)
		{
			selected_gameobject = App->scene->selected_gameobjects.front();
		}
		else if (App->scene->selected_gameobjects.size() > 1)
		{
			ImGui::Text("More than 1 GameObject selected!");
		}

		if (selected_gameobject != nullptr && !selected_gameobject->is_on_destroy)
		{
			ImGui::Text("Name: %s", selected_gameobject->GetName().c_str());
			bool is_gameobject_active = selected_gameobject->IsActive();

			if (ImGui::Checkbox("Active", &is_gameobject_active))
			{
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
			if (ImGui::SmallButton((selected_gameobject->GetTag() + "##tags").c_str()))
			{
				ImGui::OpenPopup("Tags##gameobject");
			}
			if (ImGui::BeginPopup("Tags##gameobject"))
			{
				for (int i = 0; i < App->tags_and_layers->tags_list.size(); i++)
				{
					std::string name = App->tags_and_layers->tags_list[i];
					if (ImGui::MenuItem(name.c_str()))
					{
						if (name == "Main Camera" && App->renderer3D->game_camera == nullptr)
						{
							selected_gameobject->SetTag(name);
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Add Tag##gameobject"))
				{
					App->editor->tags_and_layers_window->active = true;
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::Text("Layer:");
			ImGui::SameLine();
			if (ImGui::SmallButton((selected_gameobject->GetLayer() + "##layers").c_str()))
			{
				ImGui::OpenPopup("Layers##gameobject");
			}
			if (ImGui::BeginPopup("Layers##gameobject"))
			{
				for (int i = 0; i < App->tags_and_layers->layers_list.size(); i++)
				{
					std::string name = App->tags_and_layers->layers_list[i];
					if (ImGui::MenuItem(name.c_str())) {
						selected_gameobject->SetLayer(name);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Add Layer##gameobject"))
				{
					App->editor->tags_and_layers_window->active = true;
				}
				ImGui::EndPopup();
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			scripts_count = 0;
			factories_count = 0;
			colliders_count = 0;
			distance_joints_count = 0;
			lights_count = 0;

			for (std::list<Component*>::iterator it = selected_gameobject->components_list.begin(); it != selected_gameobject->components_list.end(); it++)
			{
				DrawComponent((*it));
				ImGui::Separator();
				ImGui::Spacing();
			}

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("Components");
			}

			if (ImGui::BeginPopup("Components"))
			{
				if (ImGui::MenuItem("Mesh Renderer"))
				{
					if (selected_gameobject->GetComponent(Component::CompMeshRenderer) == nullptr || selected_gameobject->GetComponent(Component::CompBlast) == nullptr) {
						selected_gameobject->AddComponent(Component::CompMeshRenderer);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Mesh Renderer!");
					}
				}
				/*if (ImGui::MenuItem("Blast Mesh Renderer")) {
					if (selected_gameobject->GetComponent(Component::CompBlastMeshRenderer) == nullptr && selected_gameobject->GetComponent(Component::CompMeshRenderer) == nullptr) {
						selected_gameobject->AddComponent(Component::CompBlastMeshRenderer);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Mesh Renderer!");
					}
				}*/
				if (ImGui::MenuItem("Camera"))
				{
					if (selected_gameobject->GetComponent(Component::CompCamera) == nullptr)
					{
						selected_gameobject->AddComponent(Component::CompCamera);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 Camera!");
					}
				}

				if (ImGui::BeginMenu("UI"))
				{
					if (ImGui::MenuItem("Canvas"))
					{
						if (selected_gameobject->GetComponent(Component::CompCanvas) == nullptr)
						{
							selected_gameobject->AddComponent(Component::CompCanvas);
						}
						else
							CONSOLE_WARNING("GameObject can't have more than 1 Canvas!");
					}

					ImGui::EndMenu();
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

				if (ImGui::MenuItem("Light")) {
					if (App->renderer3D->GetDirectionalLightCount() < 2 || App->renderer3D->GetSpotLightCount() < 8 || App->renderer3D->GetPointLightCount() < 8)
					{
						if (selected_gameobject->GetComponent(Component::CompLight) == nullptr)
						{
							ComponentLight* light = (ComponentLight*)selected_gameobject->AddComponent(Component::CompLight);
							if (App->renderer3D->GetSpotLightCount() == 8)
							{
								if (App->renderer3D->GetDirectionalLightCount() == 2)
								{
									light->SetTypeToDirectional();
								}
								else
								{
									light->SetTypeToPoint();
								}
							}
						}
						else
						{
							CONSOLE_ERROR("GameObject can't have more than 1 Light for now...");
						}
					}
					else
					{
						CONSOLE_ERROR("Max lights created. Can't add more lights");
					}
				}

				if (ImGui::BeginMenu("Script"))
				{
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

				if (ImGui::BeginMenu("Audio"))
				{
					if (ImGui::MenuItem("Audio Listener"))
					{
						if (App->audio->GetDefaultListener() != nullptr)
						{

						}
						else
							ComponentListener* listener = (ComponentListener*)selected_gameobject->AddComponent(Component::CompAudioListener);
					}
					if (ImGui::MenuItem("Audio Source"))
					{
						ComponentAudioSource* audio_source = (ComponentAudioSource*)selected_gameobject->AddComponent(Component::CompAudioSource);
					}
					if (ImGui::MenuItem("Distorsion Zone"))
					{
						ComponentDistorsionZone* dist_zone = (ComponentDistorsionZone*)selected_gameobject->AddComponent(Component::CompAudioDistZone);
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("New Factory"))
				{
					static char input_text[30];
					ImGui::InputText("Factory Name", input_text, 30, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::Spacing();
					if (ImGui::Button("Create"))
					{
						ComponentFactory* factory = (ComponentFactory*)selected_gameobject->AddComponent(Component::CompFactory);
						if (factory) factory->SetName(input_text);
						input_text[0] = 0;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("RigidBody"))
				{
					if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
						ComponentRigidBody* rb = (ComponentRigidBody*)selected_gameobject->AddComponent(Component::CompRigidBody);
						App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
						App->physics->AddActorToList(rb->GetRigidBody(), selected_gameobject);
					}
					else
					{
						CONSOLE_WARNING("GameObject can't have more than 1 RigidBody!");
					}
				}

				if (ImGui::BeginMenu("Colliders"))
				{
					if (ImGui::MenuItem("Box"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr)
						{
							ComponentRigidBody* rb = (ComponentRigidBody*)selected_gameobject->AddComponent(Component::CompRigidBody);
							App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
							App->physics->AddActorToList(rb->GetRigidBody(), selected_gameobject);
						}
						selected_gameobject->AddComponent(Component::CompBoxCollider);
					}
					if (ImGui::MenuItem("Sphere"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr)
						{
							ComponentRigidBody* rb = (ComponentRigidBody*)selected_gameobject->AddComponent(Component::CompRigidBody);
							App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
							App->physics->AddActorToList(rb->GetRigidBody(), selected_gameobject);
						}
						selected_gameobject->AddComponent(Component::CompSphereCollider);
					}
					if (ImGui::MenuItem("Capsule"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr)
						{
							ComponentRigidBody* rb = (ComponentRigidBody*)selected_gameobject->AddComponent(Component::CompRigidBody);
							App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
							App->physics->AddActorToList(rb->GetRigidBody(), selected_gameobject);
						}
						selected_gameobject->AddComponent(Component::CompCapsuleCollider);
					}
					if (ImGui::MenuItem("Mesh"))
					{
						if (selected_gameobject->GetComponent(Component::CompMeshRenderer))
						{
							if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr)
							{
								ComponentRigidBody* rb = (ComponentRigidBody*)selected_gameobject->AddComponent(Component::CompRigidBody);
								App->physics->AddRigidBodyToScene(rb->GetRigidBody(), nullptr);
								App->physics->AddActorToList(rb->GetRigidBody(), selected_gameobject);
							}
							selected_gameobject->AddComponent(Component::CompMeshCollider);
						}
						else
						{
							CONSOLE_ERROR("Can't have Mesh Collider without a mesh in the GameObject");
						}
					}
					ImGui::EndMenu();
				}

				/*if (ImGui::BeginMenu("Joints")) {
					if (ImGui::MenuItem("Fixed"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
							selected_gameobject->AddComponent(Component::CompRigidBody);
						}
						selected_gameobject->AddComponent(Component::CompFixedJoint);
					}
					if (ImGui::MenuItem("Distance"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
							selected_gameobject->AddComponent(Component::CompRigidBody);
						}
						selected_gameobject->AddComponent(Component::CompDistanceJoint);
					}
					if (ImGui::MenuItem("Revolute"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
							selected_gameobject->AddComponent(Component::CompRigidBody);
						}
						selected_gameobject->AddComponent(Component::CompRevoluteJoint);
					}
					if (ImGui::MenuItem("Spherical"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
							selected_gameobject->AddComponent(Component::CompRigidBody);
						}
						selected_gameobject->AddComponent(Component::CompSphericalJoint);
					}
					if (ImGui::MenuItem("Prismatic"))
					{
						if (selected_gameobject->GetComponent(Component::CompRigidBody) == nullptr) {
							selected_gameobject->AddComponent(Component::CompRigidBody);
						}
						selected_gameobject->AddComponent(Component::CompPrismaticJoint);
					}
					ImGui::EndMenu();
				}*/

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
		DrawRigidBodyPanel((ComponentRigidBody*)component);
		break;
	case Component::CompMeshRenderer:
		DrawMeshRendererPanel((ComponentMeshRenderer*)component);
		break;
	case Component::CompBoxCollider:
		DrawColliderPanel((ComponentCollider*)component);
		break;
	case Component::CompSphereCollider:
		DrawColliderPanel((ComponentCollider*)component);
		break;
	case Component::CompCapsuleCollider:
		DrawColliderPanel((ComponentCollider*)component);
		break;
	case Component::CompMeshCollider:
		DrawColliderPanel((ComponentCollider*)component);
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
	case Component::CompRectTransform:
		DrawRectTransformPanel((ComponentRectTransform*)component);
		break;
	case Component::CompCanvas:
		DrawCanvasPanel((ComponentCanvas*)component);
		break;
	case Component::CompImage:
		DrawImagePanel((ComponentImage*)component);
		break;
	case Component::CompText:
		DrawTextPanel((ComponentText*)component);
		break;
	case Component::CompProgressBar:
		DrawProgressBarPanel((ComponentProgressBar*)component);
		break;
	case Component::CompDistanceJoint:
		DrawJointDistancePanel((ComponentJointDistance*)component);
		break;
	case Component::CompAudioListener:
		DrawAudioListener((ComponentListener*)component);
		break;
	case Component::CompAudioSource:
		DrawAudioSource((ComponentAudioSource*)component);
		break;
	case Component::CompAudioDistZone:
		DrawAudioDistZone((ComponentDistorsionZone*)component);
		break;
	case Component::CompLight:
		DrawLightPanel((ComponentLight*)component);
		break;
	case Component::CompButton:
		DrawButtonPanel((ComponentButton*)component);
		break;
	default:
		break;
	}
}

void PropertiesWindow::DrawTransformPanel(ComponentTransform * transform)
{
	if (transform->GetGameObject()->GetIsUI())
		return;

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
		if (ImGui::DragFloat3("Scale", (float*)&scale, is_static, 0.25f, 0.01f)) {
			transform->SetScale(scale);
		}
	}
}

void PropertiesWindow::DrawRectTransformPanel(ComponentRectTransform * rect_transform)
{
	if (ImGui::CollapsingHeader("RectTransform", ImGuiTreeNodeFlags_DefaultOpen)) 
	{
		bool is_canvas = false;
		ComponentCanvas* c_canvas = rect_transform->GetCanvas(is_canvas);

		if (c_canvas != nullptr)
		{
			if (c_canvas->GetRenderMode() == CanvasRenderMode::RENDERMODE_WORLD_SPACE || !is_canvas)
			{
				float2 position = rect_transform->GetPos();
				float z_position = rect_transform->GetZPos();
				float3 rotation = rect_transform->GetLocalRotation();
				bool interactable = rect_transform->GetInteractable();
				float2 anchor = rect_transform->GetAnchor();
				float2 size = rect_transform->GetSize();
				float scale = rect_transform->GetScale();

				bool snap_up = rect_transform->GetSnapUp();
				bool snap_down = rect_transform->GetSnapDown();
				bool snap_left = rect_transform->GetSnapLeft();
				bool snap_right = rect_transform->GetSnapRight();

				if (ImGui::DragFloat2("Position", (float*)&position, true, 0.25f))
				{
					rect_transform->SetPos(position);
				}

				if (ImGui::DragFloat("Z Position", &z_position, true))
				{
					rect_transform->SetZPos(z_position);
				}

				if (ImGui::DragFloat3("Rotation", (float*)&rotation, true, 0.25f))
				{
					rect_transform->SetRotation(rotation);
				}

				if (!is_canvas)
				{
					if (ImGui::DragFloat2("Size", (float*)&size, true, 0.1f, 0))
					{
						rect_transform->SetSize(size);
					}

					if (rect_transform->GetFixedAspectRatio())
					{
						ImGui::SameLine();
						ImGui::Text("(Fixed ratio)");
					}

					if (ImGui::DragFloat2("Anchor", (float*)&anchor, true, 0.01f, 0, 1))
					{
						rect_transform->SetAnchor(anchor);
					}

					if (ImGui::Checkbox("Snap Up", &snap_up))
					{
						rect_transform->SetSnapUp(snap_up);
					}

					ImGui::SameLine();
					if (ImGui::Checkbox("Snap Down", &snap_down))
					{
						rect_transform->SetSnapDown(snap_down);
					}

					if (ImGui::Checkbox("Snap Left", &snap_left))
					{
						rect_transform->SetSnapLeft(snap_left);
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("Snap Right", &snap_right))
					{
						rect_transform->SetSnapRight(snap_right);
					}

					if (ImGui::Checkbox("Interactable", &interactable))
					{
						rect_transform->SetInteractable(interactable);
					}

					ImGui::Text("Has canvas");
				}
				else
					ImGui::Text("Size is given by canvas");
			}
			else
				ImGui::Text("Values are given by screen space");

	/*		ImGui::Separator();
			ImGui::Text("Debug");

			ImGui::Text("Global origin: x%f, y%f", rect_transform->GetOriginGlobalPos().x, rect_transform->GetOriginGlobalPos().y);
			ImGui::Text("Global anchor: x:%f, y:%f", rect_transform->GetAnchorGlobalPos().x, rect_transform->GetAnchorGlobalPos().y);
			ImGui::Text("Global pos: x:%f, y:%f", rect_transform->GetGlobalPos().x, rect_transform->GetGlobalPos().y);
			ImGui::Text("Local pos: x%f, y%f", rect_transform->GetLocalPos().x, rect_transform->GetLocalPos().y);
			ImGui::Text("local origin: x%f, y%f", rect_transform->GetOriginLocalPos().x, rect_transform->GetOriginLocalPos().y);*/
		}
		else
			ImGui::Text("No canvas found");
	}
}

void PropertiesWindow::DrawCanvasPanel(ComponentCanvas * canvas)
{
	if (ImGui::CollapsingHeader("Canvas", ImGuiTreeNodeFlags_DefaultOpen))
	{	
		const char* mode_names[] = { "Screen Space", "World Space" };
		int render_mode = canvas->GetRenderMode();

		const char* scale_names[] = { "Constant Size", "Scale with screen size" };
		int scale_mode = canvas->GetScaleMode();

		ImGui::Combo("Render Mode", &render_mode, mode_names, 2);

		if (render_mode == 0)
		{
			canvas->SetRenderMode(CanvasRenderMode::RENDERMODE_SCREEN_SPACE);

			ImGui::Text("Size: %d %d", (int)canvas->GetSize().x, (int)canvas->GetSize().y);
		}
		else if (render_mode == 1)
		{
			canvas->SetRenderMode(CanvasRenderMode::RENDERMODE_WORLD_SPACE);

			float2 size = canvas->GetSize();
			if (ImGui::DragFloat2("Canvas Size", (float*)&size, true, 0.25f, 0))
			{
				canvas->SetSize(size);
			}
		}

		ImGui::Separator();

		ImGui::Combo("Scale Mode", &scale_mode, scale_names, 2);

		if (scale_mode == 0)
		{
			canvas->SetScaleMode(CanvasScaleMode::SCALEMODE_CONSTANT_SIZE);

			float scale = canvas->GetScale();
			if (ImGui::DragFloat("Scale", &scale, true, 0.01f, 0.0f))
			{
				canvas->SetScale(scale);
			}
		}
		else if (scale_mode == 1)
		{
			canvas->SetScaleMode(CanvasScaleMode::SCALEMODE_WITH_SCREEN_SIZE);

			float2 reference_size = canvas->GetReferenceSize();

			if (ImGui::DragFloat2("Reference Size", (float*)&reference_size, true, 0.1f, 0))
			{
				canvas->SetReferenceSize(reference_size);
			}

			ImGui::Text("Scale: %f", canvas->GetScale());
		}
	}
}

void PropertiesWindow::DrawImagePanel(ComponentImage * image)
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* mode_names[] = { "Single", "Animation" };
		int mode = image->GetMode();

		ImGui::Combo("Image mode", &mode, mode_names, 2);

		if (mode == 0)
		{
			image->SetMode(ImageMode::IM_SINGLE);

			Texture* tex = image->GetTexture();

			if (ImGui::InputResourceTexture("Texture", &tex));
			image->SetTexture(tex);

			if (image->GetTexture() != nullptr)
			{
				image->SetTexture(tex);

				if (ImGui::Button("Set native size"))
				{
					image->SetNativeSize();
				}
			}

			float colour[4] = { image->GetColour().x, image->GetColour().y, image->GetColour().z, image->GetColour().w };

			ImGui::Text("Colour");
			if (ImGui::ColorEdit4("", colour))
			{
				image->SetColour(float4(colour[0], colour[1], colour[2], colour[3]));
			}
		}
		else if (mode == 1)
		{
			image->SetMode(ImageMode::IM_ANIMATION);

			float animation_speed = image->GetAnimSpeed();
			int animation_images = image->GetNumAnimTextures();
			bool preview_play = image->GetAnimationPreviewPlay();
			bool loop = image->GetLoop();

			std::string preview = "Preview: ";

			if (preview_play)
				preview += "ON";
			else
				preview += "OFF";

			if (ImGui::Checkbox(preview.c_str(), &preview_play))
			{
				image->SetAnimationPreviewPlay(preview_play);
			}

			if (ImGui::Checkbox("Loop", &loop))
			{
				image->SetLoop(loop);
			}

			if (ImGui::DragFloat("Speed", &animation_speed, true, 0.01f, 0.0f))
			{
				image->SetAnimSpeed(animation_speed);
			}

			if (ImGui::DragInt("Size", &animation_images, true, 1, 0, 100))
			{
				image->SetNumAnimTextures(animation_images);
			}

			std::vector<Texture*> textures = image->GetAnimTextures();
			
			for (int i = 0; i < textures.size(); ++i)
			{
				Texture* curr_text = textures[i];

				std::string name = "Texture" + std::to_string(i);
				if (ImGui::InputResourceTexture(name.c_str(), &curr_text));
				{
					image->AddAnimTexture(curr_text, i);
				}
			}
		}

		ImGui::Separator();

		bool flip = image->GetFlip();
		if (ImGui::Checkbox("Flip", &flip))
		{
			image->SetFlip(flip);
		}
	}
}

void PropertiesWindow::DrawTextPanel(ComponentText * text)
{
	if (ImGui::CollapsingHeader("Label", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Font* font = text->GetFont();
		if (ImGui::InputResourceFont("Font", &font))
		{
			text->SetFont(font);
		}

		if (text->HasFont())
		{
			float colour[4] = { text->GetColour().x, text->GetColour().y, text->GetColour().w, text->GetColour().z };

			int size = text->GetFontSize();

			bool bold = text->GetStyleBold();
			bool italic = text->GetStyleItalic();
			bool underline = text->GetStyleUnderline();
			bool strikethrough = text->GetStyelStrikethrough();

			int grow_dir = text->GetGrowDirection();
			const char* grow_dir_names[] = { "Left", "Right", "Center" };

			char buffer[255];
			strcpy(buffer, text->GetText().c_str());
			if (ImGui::InputTextMultiline("Show Text", buffer, 255, ImVec2(310, 100)))
			{
				text->SetText(buffer);
			}

			if (ImGui::Combo("Grow Direction", &grow_dir, grow_dir_names, 3))
			{
				text->SetGrowDirection(static_cast<TextGrow>(grow_dir));
			}

			if (ImGui::DragInt("Font Size", &size, true, 1, 0, 1000))
			{
				text->SetFontSize(size);
			}

			ImGui::Text("Colour");
			if (ImGui::ColorEdit4("", colour, ImGuiColorEditFlags_AlphaBar))
			{
				text->SetColour(float4(colour[0], colour[1], colour[3], colour[2]));
			}

			ImGui::Text("Character");
			if (ImGui::Checkbox("Bold", &bold))
			{
				text->SetStyleBold(bold);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Italic", &italic))
			{
				text->SetStyleItalic(italic);
			}
	
			if (ImGui::Checkbox("Underline", &underline))
			{
				text->SetStyleUnderline(underline);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Strikethrough", &strikethrough))
			{
				text->SetStyelStrikethrough(strikethrough);
			}
		}
	}
}

void PropertiesWindow::DrawProgressBarPanel(ComponentProgressBar * bar)
{
	if (ImGui::CollapsingHeader("Progress Bar", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float progress = bar->GetProgressPercentage();
		if (ImGui::DragFloat("Progress %", &progress, true, 1, 0, 100))
		{
			bar->SetProgressPercentage(progress);
		}

		float base_colour[4] = { bar->GetBaseColour().x, bar->GetBaseColour().y,bar->GetBaseColour().z, bar->GetBaseColour().w };
		float progress_colour[4] = { bar->GetProgressColour().x,bar->GetProgressColour().y,bar->GetProgressColour().z, bar->GetProgressColour().w };

		ImGui::Text("Base Colour");
		if (ImGui::ColorEdit4("Base", base_colour, ImGuiColorEditFlags_AlphaBar))
		{
			bar->SetBaseColour(float4(base_colour[0], base_colour[1], base_colour[2], base_colour[3]));
		}

		ImGui::Text("Progress Colour");
		if (ImGui::ColorEdit4("Progress", progress_colour, ImGuiColorEditFlags_AlphaBar))
		{
			bar->SetProgressColour(float4(progress_colour[0], progress_colour[1], progress_colour[2], progress_colour[3]));
		}
	}
}

void PropertiesWindow::DrawButtonPanel(ComponentButton * button)
{
	if (ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float4 idle_colour = button->GetIdleColour();
		float4 over_colour = button->GetOverColour();
		float4 pressed_colour = button->GetPressedColour();

		Texture* idle_texture = button->GetIdleTexture();
		Texture* over_texture = button->GetOverTexture();
		Texture* pressed_texture = button->GetPressedTexture();

		const char* mode_names[] = { "Colour", "Image" };
		int mode = button->GetButtonMode();

		ImGui::Combo("Button Mode", &mode, mode_names, 2);

		if (mode == 0)
		{
			button->SetButtonMode(ButtonMode::BM_COLOUR);

			ImGui::Text("Idle Colour");
			if (ImGui::ColorEdit4("Idle", (float*)&idle_colour, ImGuiColorEditFlags_AlphaBar))
			{
				button->SetIdleColour(idle_colour);
			}

			ImGui::Text("Over Colour");
			if (ImGui::ColorEdit4("Over", (float*)&over_colour, ImGuiColorEditFlags_AlphaBar))
			{
				button->SetOverColour(over_colour);
			}

			ImGui::Text("Pressed Colour");
			if (ImGui::ColorEdit4("Pressed", (float*)&pressed_colour, ImGuiColorEditFlags_AlphaBar))
			{
				button->SetPressedColour(pressed_colour);
			}
		}
		else if (mode == 1)
		{
			button->SetButtonMode(ButtonMode::BM_IMAGE);

			if (ImGui::InputResourceTexture("Idle Texture", &idle_texture))
			{
				button->SetIdleTexture(idle_texture);
			}

			if (ImGui::InputResourceTexture("Over Texture", &over_texture))
			{
				button->SetOverTexture(over_texture);
			}

			if (ImGui::InputResourceTexture("Pressed Texture", &pressed_texture))
			{
				button->SetPressedTexture(pressed_texture);
			}
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

				ImGui::Separator();

				ImGui::Text("Material name: "); ImGui::SameLine();
				ImGui::Text(mesh_renderer->GetMaterial()->GetName().c_str());


				// NORMAL MAP
				ImGui::Text("Normal: "); ImGui::SameLine();
				if (mesh_renderer->GetMaterial()->GetNormalMapTexture() != nullptr)
				{
					ImGui::Text(mesh_renderer->GetMaterial()->GetNormalMapTexture()->GetName().c_str());
				}
				else ImGui::Text("none");

				Texture* normalmap = material->GetNormalMapTexture();
				if (ImGui::InputResourceTexture("Change Normal Map", &normalmap))
				{
					material->SetNormalMapTexture(normalmap);
				}

				// DIFFUSE
				ImGui::Text("Diffuse: "); ImGui::SameLine();
				if (mesh_renderer->GetMaterial()->GetDiffuseTexture() != nullptr)
				{
					ImGui::Text(mesh_renderer->GetMaterial()->GetDiffuseTexture()->GetName().c_str());
				}
				else ImGui::Text("none");

				Texture* diffuse = material->GetDiffuseTexture();
				if (ImGui::InputResourceTexture("Change Diffuse", &diffuse))
				{
					material->SetDiffuseTexture(diffuse);
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
		if (ImGui::Button(("Delete Component##Script_" + script_name).c_str()))
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
				if (ImGui::InputInt(("##" + (*it)->fieldName + script_name).c_str(), &i)) {
					comp_script->GetScript()->SetIntProperty((*it)->fieldName.c_str(), i);
				}
			}
				break;
			case ScriptField::Decimal:
			{
				double d = comp_script->GetScript()->GetDoubleProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputFloat(("##" + (*it)->fieldName + script_name).c_str(), (float*)&d, 0.001f, 0.01f, 3)) {
					comp_script->GetScript()->SetDoubleProperty((*it)->fieldName.c_str(), d);
				}
			}
				break;
			case ScriptField::Float:
			{
				float f = comp_script->GetScript()->GetFloatProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::InputFloat(("##" + (*it)->fieldName + script_name).c_str(), &f, 0.01f, 0.1f, 3)) {
					comp_script->GetScript()->SetFloatProperty((*it)->fieldName.c_str(), f);
				}
			}
				break;
			case ScriptField::Bool:
			{
				bool b = comp_script->GetScript()->GetBoolProperty((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::Checkbox(("##" + (*it)->fieldName + script_name).c_str(), &b)) {
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
				if (ImGui::InputText(("##" + (*it)->fieldName + script_name).c_str(), textToRender, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
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
				if (ImGui::DragFloat2(("##" + (*it)->fieldName + script_name).c_str(), &v2[0], !App->IsPlaying(), 0.25f)) {
					comp_script->GetScript()->SetVec2Property((*it)->fieldName.c_str(), v2);
				}
			}
				break;
			case ScriptField::Vector3:
			{
				float3 v3 = comp_script->GetScript()->GetVec3Property((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::DragFloat3(("##" + (*it)->fieldName + script_name).c_str(), &v3[0], !App->IsPlaying(), 0.25f)) {
					comp_script->GetScript()->SetVec3Property((*it)->fieldName.c_str(), v3);
				}
			}
				break;
			case ScriptField::Vector4:
			{
				float4 v4 = comp_script->GetScript()->GetVec4Property((*it)->fieldName.c_str());
				ImGui::Text(" %s", (*it)->fieldName.c_str());
				ImGui::SameLine();
				if (ImGui::DragFloat4(("##" + (*it)->fieldName + script_name).c_str(), &v4[0], !App->IsPlaying(), 0.25f)) {
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
		if (ImGui::InputInt(("Object Count##" + std::to_string(factories_count)).c_str(), &count))
		{
			factory->SetObjectCount(count);
		}

		float life_time = factory->GetLifeTime();
		if (ImGui::DragFloat(("Life Time##" + std::to_string(factories_count)).c_str(), &life_time, true, 0.025f, 0))
		{
			factory->SetLifeTime(life_time);
		}
	}
}

void PropertiesWindow::DrawRigidBodyPanel(ComponentRigidBody * rigidbody)
{
	if (ImGui::CollapsingHeader(rigidbody->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		float mass = rigidbody->GetMass();
		if (ImGui::DragFloat("Mass", &mass))
		{
			rigidbody->SetMass(mass);
		}
		float l_damping = rigidbody->GetLinearDamping();
		if (ImGui::DragFloat("Linear Damping", &l_damping))
		{
			rigidbody->SetLinearDamping(l_damping);
		}
		float a_damping = rigidbody->GetAngularDamping();
		if (ImGui::DragFloat("Angular Damping", &a_damping))
		{
			rigidbody->SetAngularDamping(a_damping);
		}
		float3 linear_velocity = rigidbody->GetLinearVelocity();
		if (ImGui::DragFloat3("Linear Velo", (float*)&linear_velocity))
		{
			rigidbody->SetLinearVelocity(linear_velocity);
		}
		bool use_gravity = rigidbody->IsUsingGravity();
		if (ImGui::Checkbox("Gravity", &use_gravity))
		{
			rigidbody->SetUseGravity(use_gravity);
		}
		bool is_kinematic = rigidbody->IsKinematic();
		if (ImGui::Checkbox("Kinematic", &is_kinematic))
		{
			rigidbody->SetKinematic(is_kinematic);
		}
		bool is_ccd = rigidbody->IsCCDMode();
		if (ImGui::Checkbox("CCD", &is_ccd))
		{
			rigidbody->SetCCDMode(is_ccd);
		}

		ImGui::Text("Axis Lock");
		ImGui::Separator();
		ImGui::Text("Movement:");
		bool linear_x = rigidbody->GetDynamicLocks(ComponentRigidBody::LinearX);
		if (ImGui::Checkbox("X##LinearX", &linear_x))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::LinearX, linear_x);
		}
		ImGui::SameLine();
		bool linear_y = rigidbody->GetDynamicLocks(ComponentRigidBody::LinearY);
		if (ImGui::Checkbox("Y##LinearY", &linear_y))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::LinearY, linear_y);
		}
		ImGui::SameLine();
		bool linear_z = rigidbody->GetDynamicLocks(ComponentRigidBody::LinearZ);
		if (ImGui::Checkbox("Z##LinearZ", &linear_z))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::LinearZ, linear_z);
		}

		ImGui::Text("Rotation:");
		bool angular_x = rigidbody->GetDynamicLocks(ComponentRigidBody::AngularX);
		if (ImGui::Checkbox("X##AngularX", &angular_x))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::AngularX, angular_x);
		}
		ImGui::SameLine();
		bool angular_y = rigidbody->GetDynamicLocks(ComponentRigidBody::AngularY);
		if (ImGui::Checkbox("Y##AngularY", &angular_y))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::AngularY, angular_y);
		}
		ImGui::SameLine();
		bool angular_z = rigidbody->GetDynamicLocks(ComponentRigidBody::AngularZ);
		if (ImGui::Checkbox("Z##AngularZ", &angular_z))
		{
			rigidbody->SetDynamicLocks(ComponentRigidBody::AngularZ, angular_z);
		}
	}
}

void PropertiesWindow::DrawColliderPanel(ComponentCollider * comp_collider)
{
	colliders_count++;
	if (ImGui::CollapsingHeader((comp_collider->GetName() + "##" + std::to_string(colliders_count)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool is_active = comp_collider->IsActive();
		if (ImGui::Checkbox(("Active##Collider" + std::to_string(colliders_count)).c_str(), &is_active))
		{
			comp_collider->SetActive(is_active);
		}
		ImGui::SameLine();
		if (ImGui::Button(("Delete Component##Collider" + std::to_string(colliders_count)).c_str()))
		{
			App->scene->selected_gameobjects.front()->DestroyComponent(comp_collider);
		}

		PhysicsMaterial* material = comp_collider->GetColliderMaterial();
		if (ImGui::InputResourcePhysMaterial("Material", &material))
		{
			comp_collider->SetColliderMaterial(material);
		}
		ImGui::Spacing();

		bool is_trigger = comp_collider->IsTrigger();
		if (ImGui::Checkbox("Trigger", &is_trigger))
		{
			comp_collider->SetTrigger(is_trigger);
		}

		//material

		float3 center = comp_collider->GetColliderCenter();
		if (ImGui::DragFloat3("Center", (float*)&center, true, 0.25f)) {
			comp_collider->SetColliderCenter(center);
		}

		float3 box_size;
		float sphere_radius;
		float capsule_radius;
		float capsule_height;
		const char* directions[3];
		int current_direction;
		bool is_convex = false;

		switch (comp_collider->GetColliderType())
		{
		case ComponentCollider::BoxCollider:
			box_size = comp_collider->GetBoxSize();
			if (ImGui::DragFloat3("Size##box", (float*)&box_size, true, 0.25f)) {
				comp_collider->SetBoxSize(box_size);
			}
			break;
		case ComponentCollider::SphereCollider:
			sphere_radius = comp_collider->GetSphereRadius();
			if (ImGui::DragFloat("Radius##sphere", &sphere_radius, true, 0.25f)) {
				comp_collider->SetSphereRadius(sphere_radius);
			}
			break;
		case ComponentCollider::CapsuleCollider:
			capsule_radius = comp_collider->GetCapsuleRadius();
			if (ImGui::DragFloat("Radius##capsule", &capsule_radius, true, 0.25f)) {
				comp_collider->SetCapsuleRadius(capsule_radius);
			}
			capsule_height = comp_collider->GetCapsuleHeight();
			if (ImGui::DragFloat("Height##capsule", &capsule_height, true, 0.25f)) {
				comp_collider->SetCapsuleHeight(capsule_height);
			}

			directions[0] = "X Axis";
			directions[1] = "Y Axis";
			directions[2] = "Z Axis";
			current_direction = comp_collider->GetCapsuleDirection();

			if (ImGui::Combo("Direction", &current_direction, directions, IM_ARRAYSIZE(directions)))
			{
				comp_collider->SetCapsuleDirection((ComponentCollider::CapsuleDirection) current_direction);
			}
			break;
		case ComponentCollider::MeshCollider:
			is_convex = comp_collider->IsConvex();
			if (ImGui::Checkbox("Convex", &is_convex))
			{
				comp_collider->ChangeMeshToConvex(is_convex);
			}
			break;
		default:
			break;
		}
	}
}

void PropertiesWindow::DrawParticleEmmiterPanel(ComponentParticleEmmiter * current_emmiter)
{
	if (ImGui::CollapsingHeader("Component Particle Emmiter"))
	{
		bool active_bool = current_emmiter->IsActive();
		bool keeper = active_bool;
		static bool rename_template = false;

		ImGui::Checkbox("Active", &active_bool);

		if (keeper != active_bool)
			current_emmiter->SetActive(keeper);

		if (current_emmiter->IsActive())
		{
			ImGui::Separator();

			if (ImGui::TreeNode("Templates"))
			{
				if (!App->resources->GetParticlesList().empty())
				{
					map<uint, ParticleData*> tmp_map = App->resources->GetParticlesList();

					for (map<uint, ParticleData*>::const_iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
					{
						if (ImGui::MenuItem(it->second->GetName().c_str()))
						{
							current_emmiter->data = it->second;
						}
					}
				}


				ImGui::TreePop();
			}

			if (ImGui::TreeNode("System Stats"))
			{
				ImGui::Text("Template Loaded:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), current_emmiter->data->GetName().c_str());

				ImGui::Text("Rendering Particles: "); ImGui::SameLine(); 
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", current_emmiter->GetParticlesNum());

				ImGui::Text("Auto Pause:"); ImGui::SameLine();

				if (current_emmiter->data->autopause)
				{
					float time_left = current_emmiter->data->time_to_stop;

					if (current_emmiter->GetSystemState() == PARTICLE_STATE_PLAY)
					{
						time_left = current_emmiter->data->time_to_stop * 1000 - current_emmiter->global_timer.Read();
						ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f sec", time_left / 1000);
					}
					else
						ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f sec", time_left);
				}
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::Text("Shock Wave: "); ImGui::SameLine();

				if (current_emmiter->show_shockwave)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "ON");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::Text("Alpha Interpolation: "); ImGui::SameLine();

				if (current_emmiter->data->change_alpha_interpolation)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "ON");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::Text("Size Interpolation: "); ImGui::SameLine();

				if (current_emmiter->data->change_size_interpolation)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "ON");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::Text("Rotation Interpolation: "); ImGui::SameLine();

				if (current_emmiter->data->change_rotation_interpolation)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "ON");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::Text("Color Interpolation: "); ImGui::SameLine();

				if (current_emmiter->data->change_color_interpolation)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "ON");
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");

				ImGui::TreePop(); 
			}

			ImGui::Separator();	

			if (ImGui::Button("PLAY"))
			{
				current_emmiter->PlayEmmiter();

				//if(current_emmiter->show_shockwave)
				//	current_emmiter->CreateShockWave(current_emmiter->data->shock_wave.wave_texture, current_emmiter->data->shock_wave.duration, current_emmiter->data->shock_wave.final_scale);
			}

			ImGui::SameLine();

			if (ImGui::Button("STOP"))
			{
				current_emmiter->StopEmmiter();
			}

			ImGui::SameLine();
			ImGui::Text("Particle System State: "); ImGui::SameLine();

			if (current_emmiter->GetSystemState() == PARTICLE_STATE_PLAY)
				ImGui::TextColored({ 0,255,0,1 }, "PLAY");
			else
				ImGui::TextColored({ 255,0,0,1 }, "PAUSED");


			ImGui::Separator();

			static int runtime_behaviour_combo; 
			ImGui::Combo("Runtime Behaviour", &runtime_behaviour_combo, "Always Emit\0Manual Mode (From Script)\0");

			if (runtime_behaviour_combo == 0)
				current_emmiter->runtime_behaviour = "Auto"; 
			
			else if (runtime_behaviour_combo == 1)
				current_emmiter->runtime_behaviour = "Manual";

			int emmision_behaviour_combo = current_emmiter->data->emmision_type;

			ImGui::Combo("Emision Behaviour", &emmision_behaviour_combo, "Continuous Emmision\0Simultaneous Emmision\0");

			if (emmision_behaviour_combo == 0)
				current_emmiter->data->emmision_type = EMMISION_CONTINUOUS;

			else if (emmision_behaviour_combo == 1)
			{
				current_emmiter->data->emmision_type = EMMISION_SIMULTANEOUS;
				ImGui::TextColored(ImVec4(0, 1, 1, 1), "Setting the time-step to 0 will cause a unique emission.");
				ImGui::DragFloat("Particle Amount", &current_emmiter->data->amount_to_emmit, 1, 1.0f, 1, 500.0f);
				ImGui::DragFloat("Emision TimeStep", &current_emmiter->data->time_step_sim, 1, 1.0f, 0, 10.0f);
			}
								
			ImGui::Separator(); 

			if (ImGui::TreeNode("Position Type"))
			{
				static int selection = 0; 
				ImGui::Combo("", &selection, "World\0Local\0"); 

				if (selection == 0) current_emmiter->data->relative_pos = false; 
				else if (selection == 1) current_emmiter->data->relative_pos = true;

				ImGui::TreePop(); 
			}

			if (ImGui::TreeNode("Auto-Pause"))
			{		
				static float turnoff_time = 0; 

				ImGui::DragFloat("Turn off timer", &turnoff_time, 1, .1f, 0, 1000.0f);
			
				if (ImGui::Button("Apply"))
				{
					if (turnoff_time != 0)
					{
						current_emmiter->data->autopause = true;
						current_emmiter->data->time_to_stop = turnoff_time;
					}
				}

				ImGui::SameLine(); 
				if (ImGui::Button("Delete"))
					current_emmiter->data->autopause = false;

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Shock Wave"))
			{
				static Texture* st_particle_texture = nullptr;
				ImGui::InputResourceTexture("Wave Texture", &st_particle_texture);

				static float wave_duration; 
				static float final_wave_scale; 

				ImGui::SliderFloat("Duration", &wave_duration, 0.1f, 5.0f);
				ImGui::SliderFloat("Final Scale", &final_wave_scale, 1.0f, 10.0f); 

				if(ImGui::Button("Apply"))
				{
					current_emmiter->show_shockwave = true; 

					current_emmiter->data->shock_wave.wave_texture = st_particle_texture; 
					current_emmiter->data->shock_wave.duration = wave_duration; 
					current_emmiter->data->shock_wave.final_scale = final_wave_scale;
				}

				ImGui::TreePop(); 
			}

			float prev_width = current_emmiter->data->emmit_width;
			float prev_height = current_emmiter->data->emmit_height;
			float prev_depth = current_emmiter->data->emmit_depth;

			if (ImGui::TreeNode("Emit Area"))
			{
				static bool show = current_emmiter->ShowEmmisionArea();
				ImGui::Checkbox("Show Emmiter Area", &show);
				current_emmiter->SetShowEmmisionArea(show);

				ImGui::DragFloat("Width (X)", &current_emmiter->data->emmit_width, 0.1f, 0.1f, 1.0f, 50.0f, "%.2f");
				ImGui::DragFloat("Height (X)", &current_emmiter->data->emmit_height, 0.1f, 0.1f, 1.0f, 50.0f, "%.2f");
				ImGui::DragFloat("Depth (X)", &current_emmiter->data->emmit_depth, 0.1f, 0.1f, 1.0f, 50.0f, "%.2f");

				ImGui::TreePop();
			}

			current_emmiter->data->width_increment = current_emmiter->data->emmit_width - prev_width;
			current_emmiter->data->height_increment = current_emmiter->data->emmit_height - prev_height;
			current_emmiter->data->depth_increment = current_emmiter->data->emmit_depth - prev_depth;

			if (ImGui::TreeNode("Texture"))
			{
				static Texture* st_particle_texture = nullptr;
				ImGui::InputResourceTexture("Texture To Add", &st_particle_texture);

				if (ImGui::Button("Add To Stack"))
				{
					current_emmiter->data->animation_system.AddToFrameStack(st_particle_texture);
				}

				ImGui::Text("Frame Stack Size:"); ImGui::SameLine();
				ImGui::Text(to_string(current_emmiter->data->animation_system.GetNumFrames()).c_str());

				current_emmiter->data->animation_system.PaintStackUI();

				ImGui::DragFloat("Time Step", &current_emmiter->data->animation_system.timeStep, true, 0.1f, 0, 2.0f);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Motion"))
			{
				ImGui::DragInt("Emmision Rate", &current_emmiter->data->emmision_rate, 1, 1, 1, 1000);
				ImGui::DragFloat("Lifetime", &current_emmiter->data->max_lifetime, 1, 0.1f, 0.1f, 20);
				ImGui::SliderFloat("Initial Velocity", &current_emmiter->data->velocity, 0.1f, 30);
				ImGui::SliderFloat3("Gravity", &current_emmiter->data->gravity[0], -1, 1);
				ImGui::SliderFloat("Emision Angle", &current_emmiter->data->emision_angle, 0, 179);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Billboard"))
			{
				ImGui::Checkbox("Billboarding", &current_emmiter->data->billboarding);

				static int billboard_type;
				ImGui::Combo("Templates", &billboard_type, "Select Billboard Type\0Only on X\0Only on Y\0All Axis\0");

				if (billboard_type != 0)
				{
					current_emmiter->data->billboard_type = (BillboardingType)--billboard_type;
					++billboard_type;
				}
				else
					current_emmiter->data->billboard_type = BILLBOARD_NONE;

				ImGui::TreePop();
			}

			
				if (ImGui::TreeNode("Size"))
				{
					ImGui::DragFloat("Initial Size", &current_emmiter->data->global_scale, 1, 0.1f, 0, 20.0f);
					CONSOLE_LOG("%f", current_emmiter->data->global_scale); 

					if (ImGui::TreeNode("Interpolation"))
					{
						static float3 init_scale = { 1,1,1 };
						static float3 fin_scale = { 1,1,1 };

						ImGui::DragFloat("Initial", &init_scale.x, 1, 1, 0.1f, 1000.0f);
						init_scale.y = init_scale.x;

						ImGui::DragFloat("Final", &fin_scale.x, 1, 1, 0.1f, 1000.0f);
						fin_scale.y = fin_scale.x;

						if (ImGui::Button("Apply Scale Interpolation"))
						{
							current_emmiter->data->change_size_interpolation = true;

							if (init_scale.x == fin_scale.x)
								current_emmiter->data->change_size_interpolation = false;

							current_emmiter->data->initial_scale = init_scale;
							current_emmiter->data->final_scale = fin_scale;
						}

						ImGui::SameLine();

						if (ImGui::Button("Delete"))
						{
							current_emmiter->data->change_size_interpolation = false;
						}

						ImGui::TreePop(); 
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Rotation"))
				{
					static float init_angular_v = 0;
					static float fin_angular_v = 0;

					ImGui::DragFloat("Angular Velocity", &current_emmiter->data->angular_v, 1, 1.0f, -1000.0f, 1000.0f);
				
					if (ImGui::TreeNode("Interpolation"))
					{
						ImGui::DragFloat("Initial", &init_angular_v, 1, 0.5f, 0, 150);

						ImGui::DragFloat("Final", &fin_angular_v, 1, 0.5f, 0, 150);

						if (ImGui::Button("Apply Rotation Interpolation"))
						{
							current_emmiter->data->change_rotation_interpolation = true;

							if (init_angular_v == fin_angular_v)
								current_emmiter->data->change_rotation_interpolation = false;

							current_emmiter->data->initial_angular_v = init_angular_v;
							current_emmiter->data->final_angular_v = fin_angular_v;
						}

						ImGui::SameLine();

						if (ImGui::Button("Delete"))
						{
							current_emmiter->data->change_rotation_interpolation = false;
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}


				if (ImGui::TreeNode("Alpha"))
				{
					static int current_interpolation_type; 
					ImGui::Combo("Interpolation Type", &current_interpolation_type, "Alpha Interpolation\0Match Lifetime\0Manual Start\0");

					switch (current_interpolation_type)
					{
					case 1:
						current_emmiter->data->init_alpha_interpolation_time = 0;
						break; 

					case 2:
						ImGui::DragFloat("Start at", &current_emmiter->data->init_alpha_interpolation_time, 1, 0.1f, 0, current_emmiter->data->max_lifetime); 
						break; 
					}

					if (ImGui::Button("Apply Alpha Interpolation"))
					{
						current_emmiter->data->change_alpha_interpolation = true;
			
						if (current_interpolation_type == 2 && current_emmiter->data->init_alpha_interpolation_time != 0) 
							current_emmiter->data->alpha_interpolation_delayed = true;
					}

					ImGui::SameLine();

					if (ImGui::Button("Delete"))
					{
						current_emmiter->data->change_alpha_interpolation = false;
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Color"))
				{
					ImGui::TextColored(ImVec4(0, 1, 1, 1), "If textures are used, (0,0,0) will show the original color"); 

					static bool alpha_preview = true;
					ImGui::Checkbox("Alpha", &alpha_preview);

					int misc_flags = (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0);

					ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar;
					flags |= misc_flags;
					flags |= ImGuiColorEditFlags_RGB;

					ImGui::ColorPicker4("Current Color##4", (float*)&current_emmiter->data->color, flags);

					if (ImGui::TreeNode("Interpolation"))
					{
						static int temp_initial_vec[3] = { current_emmiter->data->initial_color.r , current_emmiter->data->initial_color.g , current_emmiter->data->initial_color.b };

						ImGui::DragInt3("Initial Color", temp_initial_vec, 1, 1.0f, 0, 255);

						static int temp_final_vec[3] = { current_emmiter->data->final_color.r , current_emmiter->data->final_color.g , current_emmiter->data->final_color.b };

						ImGui::DragInt3("Final Color", temp_final_vec, 1, 1.0f, 0, 255);

						if (ImGui::Button("Apply Color Interpolation"))
						{
							current_emmiter->data->change_color_interpolation = true;

							Color initial(temp_initial_vec[0], temp_initial_vec[1], temp_initial_vec[2], 1);
							Color final(temp_final_vec[0], temp_final_vec[1], temp_final_vec[2], 1);

							current_emmiter->data->initial_color = initial;
							current_emmiter->data->final_color = final;
						}

						ImGui::TreePop();
					}
					ImGui::TreePop(); 
				}

			current_emmiter->SetEmmisionRate(current_emmiter->data->emmision_rate);

			if (ImGui::Button("Save Template"))
			{
				rename_template = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Update Template"))
			{
				string template_name = current_emmiter->data->GetName(); 
				ParticleData* to_mod = App->resources->GetParticleTemplate(template_name); 

				to_mod->Copy(current_emmiter->data); 
			}

		}

		if (rename_template)
		{
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x + 20, 150));
			bool windowActive = true;
			ImGui::Begin("Rename Game Object", &windowActive,
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_ShowBorders |
				ImGuiWindowFlags_NoTitleBar);

			ImGui::Text("Enter new name");
			static char inputText[20];
			ImGui::InputText("", inputText, 20);

			if (ImGui::Button("Confirm"))
			{
				App->resources->AddParticleTemplate(current_emmiter->data);

				Data template_to_save; 

				ParticleData* new_template = new ParticleData(); 
				new_template->Copy(current_emmiter->data); 
				new_template->SetName(inputText);

				new_template->Save(template_to_save);
				new_template->SaveTextures(template_to_save); 
				
				string new_path_name(EDITOR_PARTICLE_FOLDER); 
				new_path_name += inputText; 
				new_path_name += ".particle"; 

				template_to_save.SaveAsBinary(new_path_name);
				App->resources->CreateResource(new_path_name);
				App->resources->AddParticleTemplate(new_template);
				
				rename_template = false;
			}

			ImGui::SameLine();
			
			if (ImGui::Button("Cancel")) {
				inputText[0] = '\0';
				rename_template = false;
			}
			ImGui::End();
		}

		ImGui::SameLine();
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

void PropertiesWindow::DrawAudioListener(ComponentListener * listener)
{
	if (ImGui::CollapsingHeader("Listener"))
	{
	}
}

void PropertiesWindow::DrawAudioSource(ComponentAudioSource * audio_source)
{
	if (audio_source->GetEventsVector().empty())
		audio_source->GetEvents();

	if (ImGui::CollapsingHeader("Audio Source")) {
		if (audio_source->soundbank != nullptr) {
			std::string soundbank_name = "SoundBank: ";
			soundbank_name += audio_source->soundbank->name.c_str();
			if (ImGui::TreeNode(soundbank_name.c_str()))
			{		
				for (int i = 0; i < audio_source->GetEventsVector().size(); i++) 
				{
					ImGui::Text(audio_source->GetEventsVector()[i]->name.c_str());
					audio_source->GetEventsVector()[i]->UIDraw(audio_source);
				}	
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Settings##Event"))
			{
				ImGui::SliderInt("Volume", App->audio->GetVolumePtr(), 0, 100);
				ImGui::SliderInt("Pitch", App->audio->GetPitchPtr(), 0, 100);
				ImGui::Checkbox("Mute", App->audio->IsMutedPtr());

				ImGui::TreePop();
			}
		}
	}
}

void PropertiesWindow::DrawJointDistancePanel(ComponentJointDistance * joint)
{
	distance_joints_count++;
	if (ImGui::CollapsingHeader((joint->GetName() + "##" + std::to_string(distance_joints_count)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		GameObject* gameobject = joint->GetConnectedBody();
		if (ImGui::InputResourceGameObject("Connected Body", &gameobject, ResourcesWindow::GameObjectFilter::GoFilterRigidBody)) {
			joint->SetConnectedBody(gameobject);
		}
	}
}

void PropertiesWindow::DrawAudioDistZone(ComponentDistorsionZone * dist_zone)
{
	if (ImGui::CollapsingHeader("Distorsion Zone")) {
		char* bus_name = new char[41];

		std::copy(dist_zone->bus.begin(), dist_zone->bus.end(), bus_name);
		bus_name[dist_zone->bus.length()] = '\0';

		ImGui::InputText("Target bus", bus_name, 40);
		dist_zone->bus = bus_name;

		ImGui::DragFloat("Value", &dist_zone->distorsion_value, true, 0.1, 0.0, 12.0, "%.1f");

		delete[] bus_name;
	}
}

void PropertiesWindow::DrawLightPanel(ComponentLight* comp_light)
{
	if (comp_light != nullptr)
	{
		lights_count++;
		if (ImGui::CollapsingHeader((comp_light->GetName() + "##" + std::to_string(lights_count)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool is_active = comp_light->IsActive();
			if (ImGui::Checkbox(("Active##Light" + std::to_string(lights_count)).c_str(), &is_active))
			{
				comp_light->SetActive(is_active);
			}

			ImGui::Text("Type:");
			ImGui::SameLine();
			if (ImGui::SmallButton((comp_light->GetTypeString() + "##types" + std::to_string(lights_count)).c_str())) {
				ImGui::OpenPopup(("Types##light" + std::to_string(lights_count)).c_str());
			}
			if (ImGui::BeginPopup(("Types##light" + std::to_string(lights_count)).c_str())) {
				if (comp_light->GetLightType() != DIRECTIONAL_LIGHT && ImGui::MenuItem(("Directional##" + std::to_string(lights_count)).c_str()))
				{
					if (App->renderer3D->GetDirectionalLightCount() < 2)
					{
						comp_light->SetTypeToDirectional();
					}
					else
					{
						CONSOLE_WARNING("Exceeded limit of directional lights.");
					}
				}
				if (comp_light->GetLightType() != POINT_LIGHT && ImGui::MenuItem(("Point##" + std::to_string(lights_count)).c_str()))
				{
					if (App->renderer3D->GetPointLightCount() < 8)
					{
						comp_light->SetTypeToPoint();
					}
					else
					{
						CONSOLE_WARNING("Exceeded limit of point lights.");
					}
				}
				if (comp_light->GetLightType() != SPOT_LIGHT && ImGui::MenuItem(("Spot##" + std::to_string(lights_count)).c_str()))
				{
					if (App->renderer3D->GetPointLightCount() < 8)
					{
						comp_light->SetTypeToSpot();
					}
					else
					{
						CONSOLE_WARNING("Exceeded limit of spot lights.");
					}
				}
				ImGui::EndPopup();
			}

			float3 light_pos = comp_light->GetPositionOffset();
			float3 light_rot = comp_light->GetDirectionOffset();

			switch (comp_light->GetLightType())
			{
			case DIRECTIONAL_LIGHT:
				/*if (ImGui::DragFloat3(("Direction##directional_light_rotation" + std::to_string(lights_count)).c_str(), (float*)&light_rot, is_active, 0.25f, 0.0f)) {
					comp_light->SetDirectionOffset(light_rot);
				}*/
				if (ImGui::DragFloat(("Diffuse##directional_" + std::to_string(lights_count)).c_str(), comp_light->GetDiffuseToEdit(), is_active, 0.25f, 0.0f)) {
				}
				break;
			case SPOT_LIGHT:
				/*if (ImGui::DragFloat3(("Position##spot_light_pos" + std::to_string(lights_count)).c_str(), (float*)&light_pos, is_active, 0.25f, 0.0f)) {
					comp_light->SetPositionOffset(light_pos);
				}
				if (ImGui::DragFloat3(("Direction##spot_light_rotation" + std::to_string(lights_count)).c_str(), (float*)&light_rot, is_active, 0.25f, 0.0f)) {
					comp_light->SetDirectionOffset(light_rot);
				}*/
				if (ImGui::DragFloat(("Diffuse##spot_" + std::to_string(lights_count)).c_str(), comp_light->GetDiffuseToEdit(), is_active, 0.25f, 0.0f)) {
				}
				if (ImGui::DragFloat(("Specular##spot_" + std::to_string(lights_count)).c_str(), comp_light->GetSpecularToEdit(), is_active, 0.25f, 0.0f)) {
				}
				if (ImGui::DragFloat(("CutOff##spot_" + std::to_string(lights_count)).c_str(), comp_light->GetCutOffToEdit(), is_active, 0.25f, 0.0f)) {
				}
				if (ImGui::DragFloat(("OuterCutOff##spot_" + std::to_string(lights_count)).c_str(), comp_light->GetOuterCutOffToEdit(), is_active, 0.25f, 0.0f)) {
				}
				break;
			case POINT_LIGHT:
				/*if (ImGui::DragFloat3(("Position##point_light_pos" + std::to_string(lights_count)).c_str(), (float*)&light_pos, is_active, 0.25f, 0.0f)) {
					comp_light->SetPositionOffset(light_pos);
				}*/
				if (ImGui::DragFloat(("Diffuse##point_" + std::to_string(lights_count)).c_str(), comp_light->GetDiffuseToEdit(), is_active, 0.25f, 0.0f)) {
				}
				if (ImGui::DragFloat(("Specular##point_" + std::to_string(lights_count)).c_str(), comp_light->GetSpecularToEdit(), is_active, 0.25f, 0.0f)) {
				}
				if (ImGui::DragFloat(("Ambient##point_" + std::to_string(lights_count)).c_str(), comp_light->GetAmbientToEdit(), is_active, 0.25f, 0.0f)) {
				}
			}
			ImGui::Text(("Light Color" + std::to_string(lights_count)).c_str());
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar;
			flags |= ImGuiColorEditFlags_RGB;
			ImGui::ColorPicker4(("Current Color##" + std::to_string(lights_count)).c_str(), comp_light->GetColorToEdit(), flags);
		}
	}
}

