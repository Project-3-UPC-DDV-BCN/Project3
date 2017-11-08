#include "ComponentCamera.h"
#include "MathGeoLib\Geometry\Plane.h"
#include "RenderTexture.h"
#include "Application.h"
#include "ModuleScene.h"
#include "TagsAndLayers.h"
#include <algorithm>
#include "GameObject.h"
#include "ModuleWindow.h"
#include "RenderTextureMSAA.h"

ComponentCamera::ComponentCamera(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Camera");
	SetType(ComponentType::Camera);
	SetGameObject(attached_gameobject);

	(attached_gameobject) ? camera_frustum.pos = attached_gameobject->GetGlobalTransfomMatrix().TranslatePart() : camera_frustum.pos = float3::zero;
	(attached_gameobject) ? camera_frustum.front = attached_gameobject->GetGlobalTransfomMatrix().WorldZ() : camera_frustum.front = float3::unitZ;
	(attached_gameobject) ? camera_frustum.up = attached_gameobject->GetGlobalTransfomMatrix().WorldY() : camera_frustum.up = float3::unitY;

	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.nearPlaneDistance = 0.3f;
	camera_frustum.farPlaneDistance = 200;
	camera_frustum.horizontalFov = (float)App->window->GetWidth() / (float)App->window->GetHeight(); //<-- Needed for the first time correct aspect ratio
	SetFOV(60);

	background_color = Black;

	camera_viewport_texture = new RenderTextureMSAA();
	camera_viewport_texture->Create((uint)camera_frustum.NearPlaneWidth(), (uint)camera_frustum.NearPlaneHeight(), 2);
	//camera_viewport_texture = nullptr;
	camera_target_texture = nullptr;

	if (attached_gameobject)
	{
		for (int i = 0; i < App->tags_and_layers->layers_list.size(); i++)
		{
			layers_to_draw.push_back(App->tags_and_layers->layers_list[i]);
		}

		render_order = App->scene->GetNumCameras();
		App->scene->scene_cameras.push_back(this);
	}
}

ComponentCamera::~ComponentCamera()
{
	RELEASE(camera_viewport_texture);
	RELEASE(camera_target_texture);
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
	camera_frustum.pos = GetGameObject()->GetGlobalTransfomMatrix().TranslatePart();
	camera_frustum.front = GetGameObject()->GetGlobalTransfomMatrix().WorldZ();
	camera_frustum.up = GetGameObject()->GetGlobalTransfomMatrix().WorldY();
}

void ComponentCamera::UpdateProjection()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf((GLfloat*)camera_frustum.ProjectionMatrix().Transposed().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float * ComponentCamera::GetProjectionMatrix() const
{
	return (float*)camera_frustum.ProjectionMatrix().Transposed().v;
}

float * ComponentCamera::GetViewMatrix()
{
	//ViewMatrix is 3x4 and Transposed3 is not working
	float4x4 matrix = camera_frustum.ViewMatrix();
	return (float*)matrix.Transposed().v;
}

void ComponentCamera::SetFOV(float fov)
{
	camera_frustum.verticalFov = fov * DEGTORAD;
	camera_frustum.horizontalFov = 2 * atanf(tanf(camera_frustum.verticalFov / 2) * camera_frustum.AspectRatio());
	UpdateProjection();
}

float ComponentCamera::GetFOV() const
{
	return camera_frustum.verticalFov * RADTODEG;
}

Color ComponentCamera::GetBackgroundColor() const
{
	return background_color;
}

void ComponentCamera::SetNearPlaneDistance(float distance)
{
	camera_frustum.nearPlaneDistance = distance;
	UpdateProjection();
}

float ComponentCamera::GetNearPlaneDistance() const
{
	return camera_frustum.nearPlaneDistance;
}

void ComponentCamera::SetFarPlaneDistance(float distance)
{
	camera_frustum.farPlaneDistance = distance;
	UpdateProjection();
}

float ComponentCamera::GetFarPlanceDistance() const
{
	return camera_frustum.farPlaneDistance;
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