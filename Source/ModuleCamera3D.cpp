#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "SceneWindow.h"
#include "ModuleScene.h"
#include "PerformanceWindow.h"
#include "Data.h"
#include "ComponentCamera.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ComponentMeshRenderer.h"
#include "GameObject.h"
#include "ModuleWindow.h"
#include "Mesh.h"
#include <vector>

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{

	name = "Camera";
	can_update = false;
	camera_is_orbital = false;
	camera_sensitivity = 0.25f;

	key_speed = 38;//DEFAULT LSHIFT
	key_forward = 22;//DEFAULT W
	key_backward = 18;//DEFAULT S
	key_up = 16;//DEFAULT Q
	key_down = 4;//DEFAULT E
	key_left = 0;//DEFAULT A
	key_right = 3;//DEFAULT D
}

ModuleCamera3D::~ModuleCamera3D()
{}
// -----------------------------------------------------------------
bool ModuleCamera3D::Init(Data * editor_config)
{
	CONSOLE_DEBUG("Setting up the camera");
	
	bool ret = true;

	if (editor_config->EnterSection("Camera_Config"))
	{

		key_speed = App->input->StringToKey(editor_config->GetString("key_speed"));
		key_forward = App->input->StringToKey(editor_config->GetString("key_forward"));
		key_backward = App->input->StringToKey(editor_config->GetString("key_backward"));
		key_up = App->input->StringToKey(editor_config->GetString("key_up"));
		key_down = App->input->StringToKey(editor_config->GetString("key_down"));
		key_left = App->input->StringToKey(editor_config->GetString("key_left"));
		key_right = App->input->StringToKey(editor_config->GetString("key_right"));

		editor_config->LeaveSection();
	}
	
	return ret;
}


void ModuleCamera3D::CreateEditorCamera()
{
	editor_camera = new ComponentCamera(nullptr);
	App->renderer3D->editor_camera = editor_camera;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	CONSOLE_DEBUG("Cleaning camera");
	RELEASE(editor_camera);
	App->renderer3D->editor_camera = nullptr;
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	ms_timer.Start();
	if (can_update)
	{
		// Implement a debug camera with keys and mouse
		// Now we can make this movememnt frame rate independant!
		math::Frustum* tmp_camera_frustum = &editor_camera->camera_frustum;
		float3 new_pos(0, 0, 0);
		float speed = 20.0f * dt;
		if (App->input->GetKey(key_speed) == KEY_REPEAT)
			speed = 70.0f * dt;

		if (App->input->GetKey(key_up) == KEY_REPEAT) new_pos.y += speed;
		if (App->input->GetKey(key_down) == KEY_REPEAT) new_pos.y -= speed;

		if (App->input->GetKey(key_forward) == KEY_REPEAT) new_pos += tmp_camera_frustum->front * speed;
		if (App->input->GetKey(key_backward) == KEY_REPEAT) new_pos -= tmp_camera_frustum->front * speed;
		if (App->input->GetMouseZ() > 0) new_pos += tmp_camera_frustum->front * speed;
		if (App->input->GetMouseZ() < 0) new_pos -= tmp_camera_frustum->front * speed;

		if (App->input->GetKey(key_left) == KEY_REPEAT) new_pos -= tmp_camera_frustum->WorldRight() * speed;
		if (App->input->GetKey(key_right) == KEY_REPEAT) new_pos += tmp_camera_frustum->WorldRight() * speed;

		if (!new_pos.IsZero())
		{
			tmp_camera_frustum->Translate(new_pos);
		}

		// Mouse motion ----------------

		if ((App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && !camera_is_orbital) || App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && camera_is_orbital)
		{
			float dx = -(float)App->input->GetMouseXMotion() * camera_sensitivity * dt;
			float dy = -(float)App->input->GetMouseYMotion() * camera_sensitivity * dt;

			if (dx != 0)
			{
				Quat rotation_x = Quat::RotateY(dx);
				tmp_camera_frustum->front = rotation_x.Mul(tmp_camera_frustum->front).Normalized();
				tmp_camera_frustum->up = rotation_x.Mul(tmp_camera_frustum->up).Normalized();
			}

			if (dy != 0)
			{
				Quat rotation_y = Quat::RotateAxisAngle(tmp_camera_frustum->WorldRight(), dy);

				float3 new_up = rotation_y.Mul(tmp_camera_frustum->up).Normalized();

				if (new_up.y > 0.0f)
				{
					tmp_camera_frustum->up = new_up;
					tmp_camera_frustum->front = rotation_y.Mul(tmp_camera_frustum->front).Normalized();
				}
			}
		}

	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsOver())
	{
		int mouse_x = App->input->GetMouseX();
		int mouse_y = App->input->GetMouseY();
		MousePickRay(mouse_x, mouse_y);
	}

	App->editor->performance_window->AddModuleData(this->name, ms_timer.ReadMs());
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3 &spot)
{
	float3 direction = spot - editor_camera->camera_frustum.pos;

	float3x3 matrix = float3x3::LookAt(editor_camera->camera_frustum.front, direction.Normalized(), editor_camera->camera_frustum.up, float3::unitY);

	editor_camera->camera_frustum.front = matrix.MulDir(editor_camera->camera_frustum.front).Normalized();
	editor_camera->camera_frustum.up = matrix.MulDir(editor_camera->camera_frustum.up).Normalized();
}

void ModuleCamera3D::OrbitAt(const float3 & spot)
{

}

void ModuleCamera3D::FocusOnObject(AABB& box)
{
	editor_camera->camera_frustum.pos.x = box.maxPoint.x;
	editor_camera->camera_frustum.pos.y = box.maxPoint.y + 20;
	editor_camera->camera_frustum.pos.z = box.maxPoint.z;

	float3 look_at_pos;

	look_at_pos.x = box.CenterPoint().x;
	look_at_pos.y = box.CenterPoint().y;
	look_at_pos.z = box.CenterPoint().z;

	LookAt(look_at_pos);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return editor_camera->GetViewMatrix();
}

void ModuleCamera3D::SetOrbital(bool is_orbital)
{
	camera_is_orbital = is_orbital;
}

bool ModuleCamera3D::IsOrbital() const
{
	return camera_is_orbital;
}

math::float3 ModuleCamera3D::GetPosition() const
{
	return editor_camera->camera_frustum.pos;
}

void ModuleCamera3D::SetPosition(math::float3 position)
{
	editor_camera->camera_frustum.Translate(position);
}

ComponentCamera * ModuleCamera3D::GetCamera() const
{
	return editor_camera;
}

void ModuleCamera3D::SetCameraSensitivity(float sensivity)
{
	camera_sensitivity = sensivity;
}

float ModuleCamera3D::GetCameraSensitivity() const
{
	return camera_sensitivity;
}

void ModuleCamera3D::MousePickRay(int mouse_x, int mouse_y)
{
	ImVec2 window_pos = App->editor->scene_window->GetWindowPos();
	ImVec2 window_size = App->editor->scene_window->GetWindowSize();

	if (mouse_x > window_pos.x && mouse_x < window_pos.x + window_size.x && mouse_y > window_pos.y && mouse_y < window_pos.y + window_size.y)//If mouse is in scene 
	{
		//Ray needs x and y between [-1,1]
		float normalized_mouse_x = (((mouse_x - window_pos.x) / window_size.x) * 2) - 1;

		float normalized_mouse_y = 1 - ((mouse_y - window_pos.y) / window_size.y) * 2;


		Ray ray = this->GetCamera()->camera_frustum.UnProject(normalized_mouse_x, normalized_mouse_y);
		//------------------------------------------------------------------------------

		float min_dist = NULL;
		GameObject* closest_object = nullptr;

		for (std::list<GameObject*>::iterator it = App->scene->scene_gameobjects.begin(); it != App->scene->scene_gameobjects.end(); it++)
		{
			Ray inv_ray = ray.ReturnTransform((*it)->GetGlobalTransfomMatrix().Inverted()); //Triangle intersection needs the inverted ray

			ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)(*it)->GetComponent(Component::CompMeshRenderer);
			if (mesh_renderer != nullptr && mesh_renderer->GetMesh() != nullptr)
			{
				float dist_near;
				float dist_far;
				if (ray.Intersects(mesh_renderer->GetMesh()->box, dist_near, dist_far))//Try intersection with AABB, if it intersects, then try with triangles
				{
					float* mesh_vertices = mesh_renderer->GetMesh()->vertices;
					uint* mesh_indices = mesh_renderer->GetMesh()->indices;
					for (int i = 0; i < mesh_renderer->GetMesh()->num_indices; i += 3)//Create Triangles
					{
						Triangle temp;
						temp.a.Set(mesh_vertices[(3 * mesh_indices[i])], mesh_vertices[(3 * mesh_indices[i] + 1)], mesh_vertices[(3 * mesh_indices[i] + 2)]);
						temp.b.Set(mesh_vertices[(3 * mesh_indices[i + 1])], mesh_vertices[(3 * mesh_indices[i + 1] + 1)], mesh_vertices[(3 * mesh_indices[i + 1] + 2)]);
						temp.c.Set(mesh_vertices[(3 * mesh_indices[i + 2])], mesh_vertices[(3 * mesh_indices[i + 2] + 1)], mesh_vertices[(3 * mesh_indices[i + 2] + 2)]);

						if (inv_ray.Intersects(temp))//If it intersects, save the distance 
						{
							if (min_dist == NULL || dist_near < min_dist)
							{
								min_dist = dist_near;
								if (closest_object != nullptr)
								{
									closest_object->SetSelected(false);
								}
								closest_object = *it;
							}
						}
					}
				}
			}
		}
		if (closest_object != nullptr)
		{
			if (App->editor->scene_window->IsWindowFocused())
			{
				App->scene->selected_gameobjects.clear();
				for (std::list<GameObject*>::iterator it = App->scene->scene_gameobjects.begin(); it != App->scene->scene_gameobjects.end(); it++)
				{
					(*it)->SetSelected(false);
				}
				closest_object->SetSelected(true);
				App->scene->selected_gameobjects.push_back(closest_object);
			}
		}
		else//If clicks but doesn't intersect an object, remove selected objects
		{
			if (App->editor->scene_window->IsWindowFocused())
			{
				App->scene->selected_gameobjects.clear();
				for (std::list<GameObject*>::iterator it = App->scene->scene_gameobjects.begin(); it != App->scene->scene_gameobjects.end(); it++)
				{
					(*it)->SetSelected(false);
				}
			}
		}
	}
}

void ModuleCamera3D::SaveData(Data * data)
{
	data->CreateSection("Camera_Config");

	data->AddString("key_speed", App->input->KeyToString(key_speed));
	data->AddString("key_forward", App->input->KeyToString(key_forward));
	data->AddString("key_backward", App->input->KeyToString(key_backward));
	data->AddString("key_up", App->input->KeyToString(key_up));
	data->AddString("key_down", App->input->KeyToString(key_down));
	data->AddString("key_left", App->input->KeyToString(key_left));
	data->AddString("key_right", App->input->KeyToString(key_right));

	data->CloseSection();
}