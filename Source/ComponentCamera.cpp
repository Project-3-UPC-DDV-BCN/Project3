#include "ComponentCamera.h"
#include "MathGeoLib/Plane.h"
#include "RenderTexture.h"
#include "Application.h"
#include "ModuleScene.h"
#include "TagsAndLayers.h"
#include <algorithm>
#include "GameObject.h"
#include "ModuleWindow.h"
#include "RenderTextureMSAA.h"
#include "ModuleRenderer3D.h"

ComponentCamera::ComponentCamera(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Camera");
	SetType(ComponentType::CompCamera);
	SetGameObject(attached_gameobject);

	(attached_gameobject) ? camera_frustum.SetPos(attached_gameobject->GetGlobalTransfomMatrix().TranslatePart()) : camera_frustum.SetPos(float3::zero);
	(attached_gameobject) ? camera_frustum.SetFront(attached_gameobject->GetGlobalTransfomMatrix().WorldZ()) : camera_frustum.SetFront(float3::unitZ);
	(attached_gameobject) ? camera_frustum.SetUp(attached_gameobject->GetGlobalTransfomMatrix().WorldY()) : camera_frustum.SetUp(float3::unitY);

	camera_frustum.SetKind(math::FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	camera_frustum.SetViewPlaneDistances(0.3f, 1000.0f);
	current_fov = 60;
	aspect_ratio = 1.3f;
	SetFOV(60);
	//camera_frustum.SetVerticalFovAndAspectRatio(60.f * DEGTORAD, 1.3f);

	background_color = Black;

	camera_viewport_texture = new RenderTextureMSAA();
	camera_viewport_texture->Create(App->window->GetWidth(), App->window->GetHeight(), 2);
	camera_target_texture = nullptr;

	for (int i = 0; i < App->tags_and_layers->layers_list.size(); i++)
	{
		layers_to_draw.push_back(App->tags_and_layers->layers_list[i]);
	}
}

ComponentCamera::~ComponentCamera()
{
	RELEASE(camera_viewport_texture);
	RELEASE(camera_target_texture);

	if (GetGameObject() != nullptr)
	{
		if (App->renderer3D)
		{
			if (std::find(App->renderer3D->rendering_cameras.begin(), App->renderer3D->rendering_cameras.end(), this) != App->renderer3D->rendering_cameras.end())
			{
				App->renderer3D->rendering_cameras.remove(this);
				if (GetGameObject()->GetTag() == "Main Camera")
				{
					App->renderer3D->game_camera = nullptr;
				}
			}
		}
	}
}

math::Frustum ComponentCamera::GetFrustum() const
{
	return camera_frustum;
}

bool ComponentCamera::ContainsGameObjectAABB(AABB& gameobject_bounding_box)
{
	int vertex_num = gameobject_bounding_box.NumVertices();
	for (int i = 0; i < 6; i++) //planes of frustum
	{
		int points_out = 0;
		for (int j = 0; j < vertex_num; j++) //vertex number of box
		{
			Plane plane = camera_frustum.GetPlane(i);
			if (plane.IsOnPositiveSide(gameobject_bounding_box.CornerPoint(j)))
			{
				points_out++;
			}
		}
		//if all the points are outside of a plane, the gameobject is not inside the frustum
		if (points_out == 8) return false;
	}
	
	return true;
}

void ComponentCamera::UpdatePosition()
{
	BROFILER_CATEGORY("Component - Camera - Update", Profiler::Color::Beige);

	camera_frustum.SetPos(GetGameObject()->GetGlobalTransfomMatrix().TranslatePart());
	camera_frustum.SetFront(GetGameObject()->GetGlobalTransfomMatrix().WorldZ().Normalized());
	camera_frustum.SetUp(GetGameObject()->GetGlobalTransfomMatrix().WorldY().Normalized());
}

float * ComponentCamera::GetProjectionMatrix() const
{
	static float4x4 matrix;

	matrix = camera_frustum.ProjectionMatrix();
	matrix.Transpose();

	return matrix.ptr();
}

float * ComponentCamera::GetViewMatrix()
{
	//ViewMatrix is 3x4 and Transposed3 is not working
	static float4x4 matrix;
	matrix = camera_frustum.ViewMatrix();
	matrix.Transpose();

	return matrix.ptr();
}

void ComponentCamera::SetAsMainCamera()
{
	App->renderer3D->game_camera = this;
}

void ComponentCamera::SetFOV(float fov)
{
	camera_frustum.SetHorizontalFovAndAspectRatio(fov * DEGTORAD, aspect_ratio);
	current_fov = fov;
	camera_frustum.ComputeProjectionMatrix();
}

float ComponentCamera::GetFOV() const
{
	return current_fov;
}

Color ComponentCamera::GetBackgroundColor() const
{
	return background_color;
}

void ComponentCamera::SetNearPlaneDistance(float distance)
{
	camera_frustum.SetViewPlaneDistances(distance, GetFarPlanceDistance());
}

float ComponentCamera::GetNearPlaneDistance() const
{
	return camera_frustum.NearPlaneDistance();
}

void ComponentCamera::SetFarPlaneDistance(float distance)
{
	camera_frustum.SetViewPlaneDistances(GetNearPlaneDistance(), distance);
}

float ComponentCamera::GetFarPlanceDistance() const
{
	return camera_frustum.FarPlaneDistance();
}

void ComponentCamera::SetAspectRatio(float ratio)
{
	//float r = DEGTORAD * ratio;

	//if (r > 0.0f)
	//{
		camera_frustum.SetHorizontalFovAndAspectRatio(current_fov * DEGTORAD, ratio);
		aspect_ratio = ratio;
	//}
}

float ComponentCamera::GetAspectRatio() const
{
	return camera_frustum.AspectRatio();
}

void ComponentCamera::SetViewport(Rect viewport)
{
	camera_viewport = viewport;
}

Rect ComponentCamera::GetViewport() const
{
	return camera_viewport;
}

int ComponentCamera::GetRenderOrder() const
{
	return render_order;
}

void ComponentCamera::SetTargetTexture(RenderTextureMSAA * texture)
{
	camera_target_texture = texture;
}

void ComponentCamera::SetViewPortTexture(RenderTextureMSAA * texture)
{
	camera_viewport_texture = texture;
}

RenderTextureMSAA * ComponentCamera::GetTargetTexture() const
{
	return camera_target_texture;
}

RenderTextureMSAA * ComponentCamera::GetViewportTexture() const
{
	return camera_viewport_texture;
}

void ComponentCamera::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddString("RenderTexture_Target", camera_viewport_texture->GetAssetsPath());
	float4 color_to_float;
	color_to_float.x = background_color.r;
	color_to_float.y = background_color.g;
	color_to_float.z = background_color.b;
	color_to_float.w = background_color.a;
	data.AddVector4("background_color", color_to_float);
	float4 rect_to_float;
	rect_to_float.x = camera_viewport.left;
	rect_to_float.y = camera_viewport.top;
	rect_to_float.z = camera_viewport.right;
	rect_to_float.w = camera_viewport.bottom;
	data.AddVector4("viewport_rect", rect_to_float);
	data.AddInt("render_order", render_order);
	data.AddFloat("far_plane", GetFarPlanceDistance());
	data.AddFloat("near_plane", GetNearPlaneDistance());
	data.AddFloat("fov", GetFOV());
}

void ComponentCamera::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	//camera_viewport_texture = LoadRenderTexture
	float4 float_to_color = data.GetVector4("background_color");
	background_color.r = float_to_color.x;
	background_color.g = float_to_color.y;
	background_color.b = float_to_color.z;
	background_color.a = float_to_color.w;
	float4 float_to_rect = data.GetVector4("viewport_rect");
	camera_viewport.left = float_to_rect.x;
	camera_viewport.top = float_to_rect.y;
	camera_viewport.right = float_to_rect.z;
	camera_viewport.bottom = float_to_rect.w;
	render_order = data.GetInt("render_order");
	SetFarPlaneDistance(data.GetFloat("far_plane"));
	SetNearPlaneDistance(data.GetFloat("near_plane"));
	SetFOV(data.GetFloat("fov"));
	
	//UpdateProjection();
}

void ComponentCamera::SetRenderOrder(int position)
{
	render_order = position;
}

void ComponentCamera::AddLayerToDraw(std::string layer)
{
	layers_to_draw.push_back(layer);
}

std::string ComponentCamera::GetLayerToDraw(int index) const
{
	return layers_to_draw[index];
}

std::vector<std::string> ComponentCamera::GetAllLayersToDraw() const
{
	return layers_to_draw;
}

void ComponentCamera::RemoveLayerToDraw(std::string layer)
{
	std::vector<std::string>::iterator it = std::find(layers_to_draw.begin(), layers_to_draw.end(), layer);
	if (it != layers_to_draw.end())
	{
		layers_to_draw.erase(it);
	}
}

void ComponentCamera::SetBackgroundColor(Color color)
{
	background_color = color;
}
