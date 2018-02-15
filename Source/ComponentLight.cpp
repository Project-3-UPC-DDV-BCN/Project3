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
	type = SPOT_LIGHT;
	SetGameObject(attached_gameobject);
	App->renderer3D->AddLight(this);
	color = Color(1.0, 1.0, 1.0, 1.0);
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

	ComponentTransform* trans = (ComponentTransform*) GetGameObject()->GetComponent(Component::CompTransform);
	trans->SetRotation(float3 (-250, -35, 0));

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

	ComponentTransform* trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
	trans->SetRotation(float3(0, 0, 0));

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

	ComponentTransform* trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);
	trans->SetRotation(float3(0, 0, 0));

	App->renderer3D->RemoveLight(this);
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
