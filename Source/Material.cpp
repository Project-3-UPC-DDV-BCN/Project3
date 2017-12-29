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
#include "ShaderProgram.h"
#include "Shader.h"

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

	SetDefaultShaders();
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

	if(diffuse_texture != nullptr)
		data.AddString("diffuse_texture", diffuse_texture->GetLibraryPath());

	math::float4 f_diffuse_color;
	f_diffuse_color.x = diffuse_color.r;
	f_diffuse_color.y = diffuse_color.g;
	f_diffuse_color.z = diffuse_color.b;
	f_diffuse_color.w = diffuse_color.a;
	data.AddVector4("diffuse_color", f_diffuse_color);

	if (specular_texture != nullptr)
		data.AddString("specular_texture", specular_texture->GetLibraryPath());

	if (ambient_texture != nullptr)
		data.AddString("ambient_texture", ambient_texture->GetLibraryPath());
	
	if (emissive_texture != nullptr) 
		data.AddString("emissive_texture", emissive_texture->GetLibraryPath());
	
	if (heightmap_texture != nullptr)
		data.AddString("height_texture", heightmap_texture->GetLibraryPath());
	
	if (normalmap_texture != nullptr)
		data.AddString("normalmap_texture", normalmap_texture->GetLibraryPath());

	if (shininess_texture != nullptr)
		data.AddString("shininess_texture", shininess_texture->GetLibraryPath());
	
	if (opacity_texture != nullptr)
		data.AddString("opacity_texture", opacity_texture->GetLibraryPath());
	
	if (displacement_texture != nullptr)
		data.AddString("displacement_texture", displacement_texture->GetLibraryPath());
	
	if (lightmap_texture != nullptr)
		data.AddString("lightmap_texture", lightmap_texture->GetLibraryPath());
	
	if (reflection_texture != nullptr)
		data.AddString("reflection_texture", reflection_texture->GetLibraryPath());

	data.AddString("vertex_shader", shader_program->GetVertexShader()->GetName());
	data.AddString("fragment_shader", shader_program->GetFragmentShader()->GetName());
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
	
	std::string library_path = data.GetString("diffuse_texture");
	Texture* diffuse = (Texture*)App->resources->CreateResourceFromLibrary(library_path);
	SetDiffuseTexture(diffuse);
	
	math::float4 f_diffuse_color = data.GetVector4("diffuse_color");
	diffuse_color.r = f_diffuse_color.x;
	diffuse_color.g = f_diffuse_color.y;
	diffuse_color.b = f_diffuse_color.z;
	diffuse_color.a = f_diffuse_color.w;

	library_path = data.GetString("specular_texture");
	Texture* specular = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetSpecularTexture(specular);
	
	library_path = data.GetString("ambient_texture");
	Texture* ambient = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetAmbientTexture(ambient);
	
	library_path = data.GetString("emissive_texture");
	Texture* emissive = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetEmissiveTexture(emissive);
	
	library_path = data.GetString("heightmap_texture");
	Texture* heightmap = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetHeightMapTexture(heightmap);
	
	library_path = data.GetString("normalmap_texture");
	Texture* normalmap = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetNormalMapTexture(normalmap);
	
	library_path = data.GetString("shininess_texture");
	Texture* shininess = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetShininessTexture(shininess);
	
	library_path = data.GetString("opacity_texture");
	Texture* opacity = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetOpacityTexture(opacity);

	library_path = data.GetString("displacement_texture");
	Texture* displacement = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetDisplacementTexture(displacement);
	
	library_path = data.GetString("lightmap_texture");
	Texture* lightmap = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetLightMapTexture(lightmap);

	library_path = data.GetString("reflection_texture");
	Texture* reflection = App->texture_importer->LoadTextureFromLibrary(library_path);
	SetReflectionTexture(reflection);

	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("material_name"));

	Shader* vert = App->resources->GetShader(data.GetString("vertex_shader"));
	Shader* frag = App->resources->GetShader(data.GetString("fragment_shader"));

	if(vert != nullptr && frag != nullptr)
		SetShaders(vert, frag);

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
	if (diffuse_texture->GetID() != 0)
	{
		glBindTexture(GL_TEXTURE_2D, diffuse_texture->GetID());
	}
}

void Material::UnloadFromMemory()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::SetDiffuseTexture(Texture * diffuse)
{
	diffuse_texture = diffuse;
}

void Material::SetSpecularTexture(Texture * specular)
{
	specular_texture = specular;
}

void Material::SetAmbientTexture(Texture * ambient)
{
	ambient_texture = ambient;
}

void Material::SetEmissiveTexture(Texture * emissive)
{
	emissive_texture = emissive;
}

void Material::SetHeightMapTexture(Texture * heightmap)
{
	heightmap_texture = heightmap;
}

void Material::SetNormalMapTexture(Texture * normalmap)
{
	normalmap_texture = normalmap;
}

void Material::SetShininessTexture(Texture * shininess)
{
	shininess_texture = shininess;
}

void Material::SetOpacityTexture(Texture * opacity)
{
	opacity_texture = opacity;
}

void Material::SetDisplacementTexture(Texture * displacement)
{
	displacement_texture = displacement;
}

void Material::SetLightMapTexture(Texture * ligthmap)
{
	lightmap_texture = ligthmap;
}

void Material::SetReflectionTexture(Texture * refection)
{
	reflection_texture = refection;
}

Texture * Material::GetDiffuseTexture() const
{
	return diffuse_texture;
}

Texture * Material::GetSpecularTexture() const
{
	return specular_texture;
}

Texture * Material::GetAmbientTexture() const
{
	return ambient_texture;
}

Texture * Material::GetEmissiveTexture() const
{
	return emissive_texture;
}

Texture * Material::GetHeightMapTexture() const
{
	return heightmap_texture;
}

Texture * Material::GetNormalMapTexture() const
{
	return normalmap_texture;
}

Texture * Material::GetShininessTexture() const
{
	return shininess_texture;
}

Texture * Material::GetOpacityTexture() const
{
	return opacity_texture;
}

Texture * Material::GetDisplacementTexture() const
{
	return displacement_texture;
}

Texture * Material::GetLightMapTexture() const
{
	return lightmap_texture;
}

Texture * Material::GetReflectionTexture() const
{
	return reflection_texture;
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
	if (diffuse_texture != nullptr)
		diffuse_texture->IncreaseUsedCount();
	
	if (specular_texture != nullptr)
		specular_texture->IncreaseUsedCount();
	
	if (ambient_texture != nullptr)
		ambient_texture->IncreaseUsedCount();
	
	if (emissive_texture != nullptr)
		emissive_texture->IncreaseUsedCount();
	
	if (heightmap_texture != nullptr)
		heightmap_texture->IncreaseUsedCount();
	
	if (normalmap_texture != nullptr)
		normalmap_texture->IncreaseUsedCount();
	
	if (shininess_texture != nullptr)
		shininess_texture->IncreaseUsedCount();
	
	if (opacity_texture != nullptr)
		opacity_texture->IncreaseUsedCount();
	
	if (displacement_texture != nullptr)
		displacement_texture->IncreaseUsedCount();
	
	if(lightmap_texture != nullptr)
		lightmap_texture->IncreaseUsedCount();
	
	if(reflection_texture != nullptr)
		reflection_texture->IncreaseUsedCount();
	
}

void Material::DecreaseUsedTexturesCount()
{
	if (diffuse_texture != nullptr)
		diffuse_texture->DecreaseUsedCount();

	if (specular_texture != nullptr)
		specular_texture->DecreaseUsedCount();

	if (ambient_texture != nullptr)
		ambient_texture->DecreaseUsedCount();

	if (emissive_texture != nullptr)
		emissive_texture->DecreaseUsedCount();

	if (heightmap_texture != nullptr)
		heightmap_texture->DecreaseUsedCount();

	if (normalmap_texture != nullptr)
		normalmap_texture->DecreaseUsedCount();

	if (shininess_texture != nullptr)
		shininess_texture->DecreaseUsedCount();

	if (opacity_texture != nullptr)
		opacity_texture->DecreaseUsedCount();

	if (displacement_texture != nullptr)
		displacement_texture->DecreaseUsedCount();

	if (lightmap_texture != nullptr)
		lightmap_texture->DecreaseUsedCount();

	if (reflection_texture != nullptr)
		reflection_texture->DecreaseUsedCount();
}

ShaderProgram * Material::GetShaderProgram() const
{
	return shader_program;
}

void Material::SetVertexShader(Shader * vertex)
{
	if (vertex != shader_program->GetVertexShader())
	{
		shader_program->DecreaseUsedCount();

		ShaderProgram* prog = App->resources->GetShaderProgram(vertex, shader_program->GetFragmentShader());

		if (prog != nullptr)
		{
			shader_program = prog;
		}
		else
		{
			prog = new ShaderProgram();
			prog->SetFragmentShader(shader_program->GetFragmentShader());
			prog->SetVertexShader(vertex);
			prog->LinkShaderProgram();

			App->resources->AddResource(prog);

			shader_program = prog;
		}

		shader_program->IncreaseUsedCount();
	}
}

void Material::SetFragmentShader(Shader * fragment)
{
	if (fragment != shader_program->GetFragmentShader())
	{
		shader_program->DecreaseUsedCount();

		ShaderProgram* prog = App->resources->GetShaderProgram(shader_program->GetVertexShader(), fragment);

		if (prog != nullptr)
		{
			shader_program = prog;
		}
		else
		{
			prog = new ShaderProgram();
			prog->SetFragmentShader(fragment);
			prog->SetVertexShader(shader_program->GetVertexShader());
			prog->LinkShaderProgram();

			App->resources->AddResource(prog);

			shader_program = prog;
		}

		shader_program->IncreaseUsedCount();
	}
}

void Material::SetShaders(Shader * vertex, Shader * fragment)
{
	ShaderProgram* prog = App->resources->GetShaderProgram(vertex, fragment);

	if (prog != nullptr)
	{
		shader_program = prog;
	}
	else
	{
		prog = new ShaderProgram();
		prog->SetFragmentShader(fragment);
		prog->SetVertexShader(vertex);
		prog->LinkShaderProgram();

		App->resources->AddResource(prog);

		shader_program = prog;
	}

	shader_program->IncreaseUsedCount();
	
}

uint Material::GetShaderProgramID() const
{
	return shader_program->GetProgramID();
}

void Material::SetDefaultShaders()
{
	Shader* vert = App->resources->GetShader("default_vertex");
	Shader* frag = App->resources->GetShader("default_fragment");

	if (vert != nullptr && frag != nullptr)
	{
		ShaderProgram* prog = App->resources->GetShaderProgram(vert, frag);

		if (prog != nullptr)
		{
			shader_program = prog;
			prog->IncreaseUsedCount();
		}
		else
			CONSOLE_ERROR("Default Shader Program missing!");
	}
	else
		CONSOLE_ERROR("Default Shaders missing!");
}

