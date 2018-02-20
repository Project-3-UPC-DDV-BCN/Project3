#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"

ComponentLight::ComponentLight(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Light");
	SetType(ComponentType::CompLight);
	SetGameObject(attached_gameobject);
	color = Color(1.0, 1.0, 1.0, 1.0);
	light_offset_pos = float3::zero;
	light_offset_direction = float3::zero;
	position = float3::zero;
	SetTypeToSpot();
	/*SetDirectionOffset({ 0,0,0 });*/
	//SetPositionOffset({ 0,0,0 });
}

ComponentLight::~ComponentLight()
{
}

bool ComponentLight::CleanUp()
{
	App->renderer3D->RemoveLight(this);
	return true;
}

void ComponentLight::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());

	ImVec4 color(GetColor().r, GetColor().g, GetColor().b, GetColor().a);
	data.AddImVector4("color", color);

	data.AddUInt("lightType", (uint)GetLightType());
	data.AddFloat("diffuse", (float)GetDiffuse());
	data.AddFloat("ambient", (float)GetAmbient());
	data.AddFloat("specular", (float)GetSpecular());
	data.AddFloat("cutOff", (float)GetCutOff());
	data.AddFloat("outercutOff", (float)GetOuterCutOff());
}

void ComponentLight::Load(Data & data)
{
	App->renderer3D->RemoveLight(this);
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));

	SetColor(Color(data.GetImVector4("color").x, data.GetImVector4("color").y, data.GetImVector4("color").z, data.GetImVector4("color").w));
	SetLightType((LightType)data.GetUInt("lightType"));
	SetDiffuse(data.GetFloat("diffuse"));
	SetAmbient(data.GetFloat("ambient"));
	SetSpecular(data.GetFloat("specular"));
	SetCutOff(data.GetFloat("cutOff"));
	SetOuterCutOff(data.GetFloat("outercutOff"));
	App->renderer3D->AddLight(this);
}

void ComponentLight::SetColor(Color color)
{
	this->color = color;
}

Color ComponentLight::GetColor() const
{
	return color;
}

float* ComponentLight::GetColorToEdit() const
{
	return (float*)&color;
}

float4 ComponentLight::GetColorAsFloat4() const
{
	return float4(color.r, color.g, color.b, 1.0f);
}

LightType ComponentLight::GetLightType() const
{
	return type;
}

void ComponentLight::SetLightType(LightType type)
{
	this->type = type;
}

std::string ComponentLight::GetTypeString() const
{
	switch (type) {
	case DIRECTIONAL_LIGHT:
		return "Directional";
		break;
	case POINT_LIGHT:
		return "Point";
		break;
	case SPOT_LIGHT:
		return "Spot";
		break;
	default:
		return "Error";
		break;
	}
}

void ComponentLight::SetTypeToDirectional()
{
	App->renderer3D->RemoveLight(this);
	ambient = 0.0;
	specular = 0.0;
	diffuse = 1.5;
	cutOff = 0;
	outercutOff = 0;
	type = DIRECTIONAL_LIGHT;

	SetDirectionOffset({ -100, -150, 0 });

	App->renderer3D->AddLight(this);
}

void ComponentLight::SetTypeToPoint()
{
	App->renderer3D->RemoveLight(this);
	ambient = 2.5;
	specular = 2.5;
	cutOff = 0;
	outercutOff = 0;
	type = POINT_LIGHT;
	//SetDirectionOffset({ 0,0,0 });

	App->renderer3D->AddLight(this);
}

void ComponentLight::SetTypeToSpot()
{
	App->renderer3D->RemoveLight(this);
	ambient = 10.0;
	specular = 2.5;
	cutOff = 12.0f;
	outercutOff = 12.0f;
	type = SPOT_LIGHT;
	//SetDirectionOffset({ 0,0,0 });

	App->renderer3D->AddLight(this);
}

void ComponentLight::SetDiffuse(float diffuse)
{
	this->diffuse = diffuse;
}

void ComponentLight::SetAmbient(float ambient)
{
	this->ambient = ambient;
}

void ComponentLight::SetSpecular(float specular)
{
	this->specular = specular;
}

void ComponentLight::SetCutOff(float cutoff)
{
	this->cutOff = cutoff;
}

void ComponentLight::SetOuterCutOff(float outercutoff)
{
	this->outercutOff = outercutoff;
}

float ComponentLight::GetDiffuse() const
{
	return diffuse;
}

float ComponentLight::GetAmbient() const
{
	return ambient;
}

float ComponentLight::GetSpecular() const
{
	return specular;
}

float ComponentLight::GetCutOff() const
{
	return cutOff;
}

float ComponentLight::GetOuterCutOff() const
{
	return outercutOff;
}

float * ComponentLight::GetDiffuseToEdit() const
{
	return (float*) &diffuse;
}

float * ComponentLight::GetAmbientToEdit() const
{
	return (float*)&ambient;
}

float * ComponentLight::GetSpecularToEdit() const
{
	return (float*)&specular;
}

float * ComponentLight::GetCutOffToEdit() const
{
	return (float*)&cutOff;
}

float * ComponentLight::GetOuterCutOffToEdit() const
{
	return (float*)&outercutOff;
}

void ComponentLight::SetPositionOffset(float3 pos)
{
	light_offset_pos = pos;
	ComponentTransform* transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
	position = transform->GetGlobalPosition() + pos;
}

float3 ComponentLight::GetPositionOffset() const
{
	return light_offset_pos;
}

void ComponentLight::SetDirectionOffset(float3 offset_direction)
{
	ComponentTransform* transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
	float3 rot = transform->GetGlobalRotation() + offset_direction;
	float4x4 mat;
	mat = float4x4::FromEulerXYZ(DegToRad(rot.x), DegToRad(rot.y), DegToRad(rot.z));
	direction = mat.WorldZ();
}

float3 ComponentLight::GetDirectionOffset() const
{
	return light_offset_direction;
}

float3 ComponentLight::GetLightPosition() const
{
	//CONSOLE_LOG("%.3f, %.3f, %.3f", position.x, position.y, position.z);
	return position;
}

float3 ComponentLight::GetLightDirection() const
{
	return direction;
}

void ComponentLight::SetDirectionFromGO(float3 pre_direction)
{
	ComponentTransform* transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
	float3 rot;
	rot = pre_direction + light_offset_direction;
	Quat q = Quat::FromEulerXYZ(DegToRad(rot.x), DegToRad(rot.y), DegToRad(rot.z));
	float4x4 mat = float4x4::FromTRS(light_offset_pos, q, { 1,1,1 });
	float4x4 parent_mat = transform->GetMatrix();
	mat = parent_mat * mat;
	float3 pos_, scale_;
	Quat q2;
	mat.Decompose(pos_, q2, scale_);
	position = pos_ + light_offset_pos;
	direction = mat.WorldZ();
}

void ComponentLight::SetPositionFromGO(float3 pre_position)
{
	position = pre_position + light_offset_pos;
}