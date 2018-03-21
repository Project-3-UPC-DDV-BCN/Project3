#pragma once
#include "Resource.h"
#include "Color.h"
#include <vector>

class Texture;
class ShaderProgram;
class Shader;

class Material :
	public Resource
{
public:
	Material();
	~Material();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	void SetDiffuseTexture(Texture* diffuse);
	void SetSpecularTexture(Texture* specular);
	void SetAmbientTexture(Texture* ambient);
	void SetEmissiveTexture(Texture* emissive);
	void SetHeightMapTexture(Texture* heightmap);
	void SetNormalMapTexture(Texture* normalmap);
	void SetShininessTexture(Texture* shininess);
	void SetOpacityTexture(Texture* opacity);
	void SetDisplacementTexture(Texture* displacement);
	void SetLightMapTexture(Texture* ligthmap);
	void SetReflectionTexture(Texture* refection);

	Texture* GetDiffuseTexture() const;
	Texture* GetSpecularTexture() const;
	Texture* GetAmbientTexture() const;
	Texture* GetEmissiveTexture() const;
	Texture* GetHeightMapTexture() const;
	Texture* GetNormalMapTexture() const;
	Texture* GetShininessTexture() const;
	Texture* GetOpacityTexture() const;
	Texture* GetDisplacementTexture() const;
	Texture* GetLightMapTexture() const;
	Texture* GetReflectionTexture() const;

	void SetDiffuseColor(float r, float g, float b);
	void SetSpecularColor(float r, float g, float b);
	void SetAmbientColor(float r, float g, float b);
	void SetEmissiveColor(float r, float g, float b);
	void SetTransparentColor(float r, float g, float b);
	void SetReflectiveColor(float r, float g, float b);

	Color GetDiffuseColor() const;
	Color GetSpecularColor() const;
	Color GetAmbientColor() const;
	Color GetEmissiveColor() const;
	Color GetTransparentColor() const;
	Color GetReflectiveColor() const;

	void SetWireframe(bool wireframe);
	bool IsWireFrame() const;

	void SetTwoSided(bool two_sided);
	bool IsTwoSided() const;
	
	void SetShadingModel(int shading_model);
	int GetShadingModel() const;

	void SetBlendMode(int blend_mode);
	int GetBlendMode() const;

	void SetOpacity(float opacity);
	float GetOpacity() const;

	void SetShininess(float shininess);
	float GetShininess() const;

	void SetShininessStrength(float shininess_strength);
	float GetShininessStrength() const;

	void SetRefraction(float refraction);
	float GetRefraction() const;

	void SetReflectivity(float reflectivity);
	float GetReflectivity() const;

	void SetBumpScaling(float bump_scaling);
	float GetBumpScaling() const;

	void IncreaseUsedTexturesCount();
	void DecreaseUsedTexturesCount();

	ShaderProgram* GetShaderProgram()const;
	void SetVertexShader(Shader* vertex);
	void SetFragmentShader(Shader* fragment);
	

	uint GetShaderProgramID() const;

private:
	void SetDefaultShaders();
	void SetShaders(Shader* vertex, Shader* fragment);

private:

	Texture* diffuse_texture = nullptr;
	Texture* specular_texture = nullptr;
	Texture* ambient_texture = nullptr;
	Texture* emissive_texture = nullptr;
	Texture* heightmap_texture = nullptr;
	Texture* normalmap_texture = nullptr;
	Texture* shininess_texture = nullptr;
	Texture* opacity_texture = nullptr;
	Texture* displacement_texture = nullptr;
	Texture* lightmap_texture = nullptr;
	Texture* reflection_texture = nullptr;

	Color diffuse_color;
	Color specular_color;
	Color ambient_color;
	Color emissive_color;
	Color transparent_color;
	Color reflective_color;

	//Specifies whether wireframe rendering must be turned on for the material.
	bool wireframe;
	//Specifies whether meshes using this material must be rendered without backface culling.
	bool two_sided;
	//Defines the library shading model to use for (real time) rendering to approximate the original look of the material as closely as possible.
	//i.e: Blinn, Toon, Flat, Phong... 
	int shading_model;
	//Defines how the final color value in the screen buffer is computed from the given color at that position and the newly computed color from the material.
	//Simply said, alpha blending settings.
	int blend_mode;
	//Defines the opacity of the material in a range between 0..1.	
	float opacity;
	//Defines the shininess of a phong-shaded material. This is actually the exponent of the phong specular equation	
	float shininess;
	//Scales the specular color of the material.
	float shininess_strength;
	//Defines the Index Of Refraction for the material. That's not supported by most file formats.
	float refraction;
	float reflectivity;
	float bump_scaling;

	ShaderProgram* shader_program = nullptr;

	public:
	//Objects with this material will receive light ot not
};

