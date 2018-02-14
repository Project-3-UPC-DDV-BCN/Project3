#include "ComponentLight.h"
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
	App->renderer3D->AddLight(this);
	type = SPOT_LIGHT;
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
}

void ComponentLight::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
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

LightType ComponentLight::GetType() const
{
	return type;
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
	ambient = 0.0;
	specular = 0.0;
	cutOff = 0;
	outercutOff = 0;
	type = DIRECTIONAL_LIGHT;
}

void ComponentLight::SetTypeToPoint()
{
	ambient = 2.5;
	specular = 2.5;
	cutOff = 0;
	outercutOff = 0;
	type = POINT_LIGHT;
}

void ComponentLight::SetTypeToSpot()
{
	ambient = 10.0;
	specular = 2.5;
	cutOff = 12.0f;
	outercutOff = 12.0f;
	type = SPOT_LIGHT;
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
