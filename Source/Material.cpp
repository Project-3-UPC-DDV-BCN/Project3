#include "Material.h"
#include "Texture.h"
#include "Data.h"
#include "Application.h"
#include "ModuleMaterialImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleTextureImporter.h"
#include <ctime>
#include "OpenGL.h"

Material::Material()
{
	SetType(Resource::MaterialResource);
	wireframe = false;
	two_sided = false;
	shading_model = 0;
	blend_mode = 0;
	opacity = 1;
	shininess = 0;
	shininess_strength = 1;
	refraction = 1;
	reflectivity = 0;
	bump_scaling = 1;

	diffuse_color = { 0.6f,0.6f,0.6f };
}

Material::~Material()
{

}

void Material::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("material_name", GetName());
	data.AddUInt("UUID", GetUID());
	data.AddBool("wireframe", wireframe);
	data.AddBool("two_sided", two_sided);
	data.AddInt("shading", shading_model);
	data.AddInt("blend", blend_mode);
	data.AddFloat("opacity", opacity);
	data.AddFloat("shininess", shininess);
	data.AddFloat("shininess_strength", shininess_strength);
	data.AddFloat("refraction", refraction);
	data.AddFloat("reflectivity", reflectivity);
	data.AddFloat("bump_scaling", bump_scaling);
	data.CreateSection("Diffuse_textures");
	data.AddInt("diffuse_count", diffuse_texture_list.size());
	for (int i = 0; i < diffuse_texture_list.size(); i++)
	{
		if (diffuse_texture_list[i] == nullptr) continue;
		data.AddString("diffuse_texture" + std::to_string(i), diffuse_texture_list[i]->GetLibraryPath());
	}
	math::float4 f_diffuse_color;
	f_diffuse_color.x = diffuse_color.r;
	f_diffuse_color.y = diffuse_color.g;
	f_diffuse_color.z = diffuse_color.b;
	f_diffuse_color.w = diffuse_color.a;
	data.AddVector4("diffuse_color", f_diffuse_color);
	data.CloseSection();
	data.CreateSection("Specular_textures");
	data.AddInt("specular_count", specular_texture_list.size());
	for (int i = 0; i < specular_texture_list.size(); i++)
	{
		if (specular_texture_list[i] == nullptr) continue;
		data.AddString("specular_texture" + std::to_string(i), specular_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Ambient_textures");
	data.AddInt("ambient_count", ambient_texture_list.size());
	for (int i = 0; i < ambient_texture_list.size(); i++)
	{
		if (ambient_texture_list[i] == nullptr) continue;
		data.AddString("ambient_texture" + std::to_string(i), ambient_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Emissive_textures");
	data.AddInt("emissive_count", emissive_texture_list.size());
	for (int i = 0; i < emissive_texture_list.size(); i++)
	{
		if (emissive_texture_list[i] == nullptr) continue;
		data.AddString("emissive_texture" + std::to_string(i), emissive_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Heightmap_textures");
	data.AddInt("height_count", heightmap_texture_list.size());
	for (int i = 0; i < heightmap_texture_list.size(); i++)
	{
		if (heightmap_texture_list[i] == nullptr) continue;
		data.AddString("height_texture" + std::to_string(i), heightmap_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Normalmap_textures");
	data.AddInt("normal_count", normalmap_texture_list.size());
	for (int i = 0; i < normalmap_texture_list.size(); i++)
	{
		if (normalmap_texture_list[i] == nullptr) continue;
		data.AddString("normalmap_texture" + std::to_string(i), normalmap_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Shininess_textures");
	data.AddInt("shininess_count", shininess_texture_list.size());
	for (int i = 0; i < shininess_texture_list.size(); i++)
	{
		if (shininess_texture_list[i] == nullptr) continue;
		data.AddString("shininess_texture" + std::to_string(i), shininess_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Opacity_textures");
	data.AddInt("opacity_count", opacity_texture_list.size());
	for (int i = 0; i < opacity_texture_list.size(); i++)
	{
		if (opacity_texture_list[i] == nullptr) continue;
		data.AddString("opacity_texture" + std::to_string(i), opacity_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Displacement_textures");
	data.AddInt("displacement_count", displacement_texture_list.size());
	for (int i = 0; i < displacement_texture_list.size(); i++)
	{
		if (displacement_texture_list[i] == nullptr) continue;
		data.AddString("displacement_texture" + std::to_string(i), displacement_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Lightmap_textures");
	data.AddInt("lightmap_count", lightmap_texture_list.size());
	for (int i = 0; i < lightmap_texture_list.size(); i++)
	{
		if (lightmap_texture_list[i] == nullptr) continue;
		data.AddString("lightmap_texture" + std::to_string(i), lightmap_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
	data.CreateSection("Reflection_textures");
	data.AddInt("reflection_count", reflection_texture_list.size());
	for (int i = 0; i < reflection_texture_list.size(); i++)
	{
		if (reflection_texture_list[i] == nullptr) continue;
		data.AddString("reflection_texture" + std::to_string(i), reflection_texture_list[i]->GetLibraryPath());
	}
	data.CloseSection();
}

bool Material::Load(Data & data)
{
	bool ret = true;

	wireframe = data.GetBool("wireframe");
	two_sided = data.GetBool("two_sided");
	shading_model = data.GetInt("shading");
	blend_mode = data.GetInt("blend");
	opacity = data.GetFloat("opacity");
	shininess = data.GetFloat("shininess");
	shininess_strength = data.GetFloat("shininess_strength");
	refraction = data.GetFloat("refraction");
	reflectivity = data.GetFloat("reflectivity");
	bump_scaling = data.GetFloat("bump_scaling");
	data.EnterSection("Diffuse_textures");
	int diffuse_count = data.GetInt("diffuse_count");
	for (int i = 0; i < diffuse_count; i++)
	{
		std::string library_path = data.GetString("diffuse_texture" + std::to_string(i));
		Texture* diffuse = (Texture*)App->resources->CreateResourceFromLibrary(library_path);
		SetDiffuseTexture(diffuse);
	}
	math::float4 f_diffuse_color = data.GetVector4("diffuse_color");
	diffuse_color.r = f_diffuse_color.x;
	diffuse_color.g = f_diffuse_color.y;
	diffuse_color.b = f_diffuse_color.z;
	diffuse_color.a = f_diffuse_color.w;
	data.LeaveSection();
	data.EnterSection("Specular_textures");
	int specular_count = data.GetInt("specular_count");
	for (int i = 0; i < specular_count; i++)
	{
		std::string library_path = data.GetString("specular_texture" + std::to_string(i));
		Texture* specular = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetSpecularTexture(specular);
	}
	data.LeaveSection();
	data.EnterSection("Ambient_textures");
	int ambient_count = data.GetInt("ambient_count");
	for (int i = 0; i < ambient_count; i++)
	{
		std::string library_path = data.GetString("ambient_texture" + std::to_string(i));
		Texture* ambient = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetAmbientTexture(ambient);
	}
	data.LeaveSection();
	data.EnterSection("Emissive_textures");
	int emissive_count = data.GetInt("emissive_count");
	for (int i = 0; i < emissive_count; i++)
	{
		std::string library_path = data.GetString("emissive_texture" + std::to_string(i));
		Texture* emissive = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetEmissiveTexture(emissive);
	}
	data.LeaveSection();
	data.EnterSection("Heightmap_textures");
	int heightmap_count = data.GetInt("heightmap_count");
	for (int i = 0; i < heightmap_count; i++)
	{
		std::string library_path = data.GetString("heightmap_texture" + std::to_string(i));
		Texture* heightmap = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetHeightMapTexture(heightmap);
	}
	data.LeaveSection();
	data.EnterSection("Normalmap_textures");
	int normalmap_count = data.GetInt("normalmap_count");
	for (int i = 0; i < normalmap_count; i++)
	{
		std::string library_path = data.GetString("normalmap_texture" + std::to_string(i));
		Texture* normalmap = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetNormalMapTexture(normalmap);
	}
	data.LeaveSection();
	data.EnterSection("Shininess_textures");
	int shininess_count = data.GetInt("shininess_count");
	for (int i = 0; i < shininess_count; i++)
	{
		std::string library_path = data.GetString("shininess_texture" + std::to_string(i));
		Texture* shininess = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetShininessTexture(shininess);
	}
	data.LeaveSection();
	data.EnterSection("Opacity_textures");
	int opacity_count = data.GetInt("opacity_count");
	for (int i = 0; i < opacity_count; i++)
	{
		std::string library_path = data.GetString("opacity_texture" + std::to_string(i));
		Texture* opacity = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetOpacityTexture(opacity);
	}
	data.LeaveSection();
	data.EnterSection("Displacement_textures");
	int displacement_count = data.GetInt("displacement_count");
	for (int i = 0; i < displacement_count; i++)
	{
		std::string library_path = data.GetString("displacement_texture" + std::to_string(i));
		Texture* displacement = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetDisplacementTexture(displacement);
	}
	data.LeaveSection();
	data.EnterSection("Lightmap_textures");
	int lightmap_count = data.GetInt("lightmap_count");
	for (int i = 0; i < lightmap_count; i++)
	{
		std::string library_path = data.GetString("lightmap_texture" + std::to_string(i));
		Texture* lightmap = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetLightMapTexture(lightmap);
	}
	data.LeaveSection();
	data.EnterSection("Reflection_textures");
	int reflection_count = data.GetInt("reflection_count");
	for (int i = 0; i < reflection_count; i++)
	{
		std::string library_path = data.GetString("reflection_texture" + std::to_string(i));
		Texture* reflection = App->texture_importer->LoadTextureFromLibrary(library_path);
		SetReflectionTexture(reflection);
	}
	data.LeaveSection();

	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("material_name"));

	return ret;
}

void Material::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.SaveAsMeta(GetAssetsPath());
}

void Material::LoadToMemory()
{
	for (std::vector<Texture*>::iterator it = diffuse_texture_list.begin(); it != diffuse_texture_list.end(); it++)
	{
		if (*it != nullptr)
		{
			if ((*it)->GetID() > 0)
			{
				glBindTexture(GL_TEXTURE_2D, (*it)->GetID());
			}
		}
	}
}

void Material::UnloadFromMemory()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::SetDiffuseTexture(Texture * diffuse)
{
	diffuse_texture_list.push_back(diffuse);
}

void Material::SetSpecularTexture(Texture * specular)
{
	specular_texture_list.push_back(specular);
}

void Material::SetAmbientTexture(Texture * ambient)
{
	ambient_texture_list.push_back(ambient);
}

void Material::SetEmissiveTexture(Texture * emissive)
{
	emissive_texture_list.push_back(emissive);
}

void Material::SetHeightMapTexture(Texture * heightmap)
{
	heightmap_texture_list.push_back(heightmap);
}

void Material::SetNormalMapTexture(Texture * normalmap)
{
	normalmap_texture_list.push_back(normalmap);
}

void Material::SetShininessTexture(Texture * shininess)
{
	shininess_texture_list.push_back(shininess);
}

void Material::SetOpacityTexture(Texture * opacity)
{
	opacity_texture_list.push_back(opacity);
}

void Material::SetDisplacementTexture(Texture * displacement)
{
	displacement_texture_list.push_back(displacement);
}

void Material::SetLightMapTexture(Texture * ligthmap)
{
	lightmap_texture_list.push_back(ligthmap);
}

void Material::SetReflectionTexture(Texture * refection)
{
	reflection_texture_list.push_back(refection);
}

Texture * Material::GetDiffuseTexture(int index) const
{
	return diffuse_texture_list[index];
}

Texture * Material::GetSpecularTexture(int index) const
{
	return specular_texture_list[index];
}

Texture * Material::GetAmbientTexture(int index) const
{
	return ambient_texture_list[index];
}

Texture * Material::GetEmissiveTexture(int index) const
{
	return emissive_texture_list[index];
}

Texture * Material::GetHeightMapTexture(int index) const
{
	return heightmap_texture_list[index];
}

Texture * Material::GetNormalMapTexture(int index) const
{
	return normalmap_texture_list[index];
}

Texture * Material::GetShininessTexture(int index) const
{
	return shininess_texture_list[index];
}

Texture * Material::GetOpacityTexture(int index) const
{
	return opacity_texture_list[index];
}

Texture * Material::GetDisplacementTexture(int index) const
{
	return displacement_texture_list[index];
}

Texture * Material::GetLightMapTexture(int index) const
{
	return lightmap_texture_list[index];
}

Texture * Material::GetReflectionTexture(int index) const
{
	return reflection_texture_list[index];
}

std::vector<Texture*> Material::GetDiffuseTextureList() const
{
	return diffuse_texture_list;
}

std::vector<Texture*> Material::GetSpecularTextureList() const
{
	return specular_texture_list;
}

std::vector<Texture*> Material::GetAmbientTextureList() const
{
	return ambient_texture_list;
}

std::vector<Texture*> Material::GetEmissiveTextureList() const
{
	return emissive_texture_list;
}

std::vector<Texture*> Material::GetHeightMapTextureList() const
{
	return heightmap_texture_list;
}

std::vector<Texture*> Material::GetNormalMapTextureList() const
{
	return normalmap_texture_list;
}

std::vector<Texture*> Material::GetShininessTextureList() const
{
	return shininess_texture_list;
}

std::vector<Texture*> Material::GetOpacityTextureList() const
{
	return opacity_texture_list;
}

std::vector<Texture*> Material::GetDisplacementTextureList() const
{
	return displacement_texture_list;
}

std::vector<Texture*> Material::GetLightMapTextureList() const
{
	return lightmap_texture_list;
}

std::vector<Texture*> Material::GetReflectionTextureList() const
{
	return reflection_texture_list;
}

void Material::SetDiffuseColor(float r, float g, float b)
{
	diffuse_color.r = r;
	diffuse_color.g = g;
	diffuse_color.b = b;
}

void Material::SetSpecularColor(float r, float g, float b)
{
	specular_color.r = r;
	specular_color.g = g;
	specular_color.b = b;
}

void Material::SetAmbientColor(float r, float g, float b)
{
	ambient_color.r = r;
	ambient_color.g = g;
	ambient_color.b = b;
}

void Material::SetEmissiveColor(float r, float g, float b)
{
	emissive_color.r = r;
	emissive_color.g = g;
	emissive_color.b = b;
}

void Material::SetTransparentColor(float r, float g, float b)
{
	transparent_color.r = r;
	transparent_color.g = g;
	transparent_color.b = b;
}

void Material::SetReflectiveColor(float r, float g, float b)
{
	reflective_color.r = r;
	reflective_color.g = g;
	reflective_color.b = b;
}

Color Material::GetDiffuseColor() const
{
	return diffuse_color;
}

Color Material::GetSpecularColor() const
{
	return specular_color;
}

Color Material::GetAmbientColor() const
{
	return ambient_color;
}

Color Material::GetEmissiveColor() const
{
	return emissive_color;
}

Color Material::GetTransparentColor() const
{
	return transparent_color;
}

Color Material::GetReflectiveColor() const
{
	return reflective_color;
}

void Material::SetWireframe(bool wireframe)
{
	this->wireframe = wireframe;
}

bool Material::IsWireFrame() const
{
	return wireframe;
}

void Material::SetTwoSided(bool two_sided)
{
	this->two_sided = two_sided;
}

bool Material::IsTwoSided() const
{
	return two_sided;
}

void Material::SetShadingModel(int shading_model)
{
	this->shading_model = shading_model;
}

int Material::GetShadingModel() const
{
	return shading_model;
}

void Material::SetBlendMode(int blend_mode)
{
	this->blend_mode = blend_mode;
}

int Material::GetBlendMode() const
{
	return blend_mode;
}

void Material::SetOpacity(float opacity)
{
	this->opacity = opacity;
}

float Material::GetOpacity() const
{
	return opacity;
}

void Material::SetShininess(float shininess)
{
	this->shininess = shininess;
}

float Material::GetShininess() const
{
	return shininess;
}

void Material::SetShininessStrength(float shininess_strength)
{
	this->shininess_strength = shininess_strength;
}

float Material::GetShininessStrength() const
{
	return shininess_strength;
}

void Material::SetRefraction(float refraction)
{
	this->refraction = refraction;
}

float Material::GetRefraction() const
{
	return refraction;
}

void Material::SetReflectivity(float reflectivity)
{
	this->reflectivity = reflectivity;
}

float Material::GetReflectivity() const
{
	return reflectivity;
}

void Material::SetBumpScaling(float bump_scaling)
{
	this->bump_scaling = bump_scaling;
}

float Material::GetBumpScaling() const
{
	return bump_scaling;
}

void Material::IncreaseUsedTexturesCount()
{
	for (std::vector<Texture*>::iterator it = diffuse_texture_list.begin(); it != diffuse_texture_list.end(); it++)
	{
		if((*it) != nullptr) (*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = specular_texture_list.begin(); it != specular_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = ambient_texture_list.begin(); it != ambient_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = emissive_texture_list.begin(); it != emissive_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = heightmap_texture_list.begin(); it != heightmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = normalmap_texture_list.begin(); it != normalmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = shininess_texture_list.begin(); it != shininess_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = opacity_texture_list.begin(); it != opacity_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = displacement_texture_list.begin(); it != displacement_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = lightmap_texture_list.begin(); it != lightmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
	for(std::vector<Texture*>::iterator it = reflection_texture_list.begin(); it != reflection_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->IncreaseUsedCount();
	}
}

void Material::DecreaseUsedTexturesCount()
{
	for (std::vector<Texture*>::iterator it = diffuse_texture_list.begin(); it != diffuse_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = specular_texture_list.begin(); it != specular_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = ambient_texture_list.begin(); it != ambient_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = emissive_texture_list.begin(); it != emissive_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = heightmap_texture_list.begin(); it != heightmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = normalmap_texture_list.begin(); it != normalmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = shininess_texture_list.begin(); it != shininess_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = opacity_texture_list.begin(); it != opacity_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = displacement_texture_list.begin(); it != displacement_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = lightmap_texture_list.begin(); it != lightmap_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
	for (std::vector<Texture*>::iterator it = reflection_texture_list.begin(); it != reflection_texture_list.end(); it++)
	{
		if ((*it) != nullptr)(*it)->DecreaseUsedCount();
	}
}



