#version 330 core

in vec4 ourColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
out vec4 color;

uniform bool has_material_color;
uniform vec4 material_color;
uniform bool has_texture;
uniform sampler2D ourTexture;
uniform bool is_ui;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec4 color;
	bool active;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec4 color;
	bool active;
};
struct SpotLight {
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 color;
	bool active;
};

#define NR_POINT_LIGHTS 8
#define NR_DIREC_LIGHTS 2
#define NR_SPOT_LIGHTS 8

uniform vec3 viewPos;
uniform DirLight dirLights[NR_DIREC_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
if (has_texture)
{
	color = texture(ourTexture, TexCoord);
}
else if (has_material_color)
	color = material_color;
else
	color = ourColor;

	if(is_ui == false)
	{
		vec3 norm = normalize(Normal);
		vec3 viewDir = normalize(viewPos - FragPos);
		
		vec3 result = vec3(0.0, 0.0, 0.0);for (int i = 0; i < NR_DIREC_LIGHTS; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir);
		for (int k = 0; k < NR_POINT_LIGHTS; k++)
		result += CalcPointLight(pointLights[k], norm, FragPos, viewDir);
		for (int j = 0; j < NR_SPOT_LIGHTS; j++)
		result += CalcSpotLight(spotLights[j], norm, FragPos, viewDir);
		color = vec4(result, 1.0);
	}
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	if (light.active == true)
	{
		vec3 lightDir = normalize(-light.direction);

		float diff = max(dot(normal, lightDir), 0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

		vec3 ambient = light.ambient * vec3(color);
		vec3 diffuse = light.diffuse * diff * vec3(color);
		vec3 specular = light.specular * spec;
		return (ambient + diffuse + specular);
	}
	else
		return vec3(0.0, 0.0, 0.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	if (light.active == true)
	{
		vec3 lightDir = normalize(light.position - fragPos);

		float diff = max(dot(normal, lightDir), 0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		vec3 ambient = light.ambient * vec3(color);
		vec3 diffuse = light.diffuse * diff * vec3(color);
		vec3 specular = light.specular * spec;
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		return (ambient + diffuse + specular) * vec3(light.color);
	}
	else
		return vec3(0.0, 0.0, 0.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	if (light.active == true)
	{
		vec3 lightDir = normalize(light.position - fragPos);

		float diff = max(dot(normal, lightDir), 0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		float theta = dot(lightDir, normalize(-light.direction));
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambient = light.ambient * vec3(color);
		vec3 diffuse = light.diffuse * diff * vec3(color);
		vec3 specular = light.specular * spec;
		ambient *= attenuation * intensity;
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;
		return (ambient + diffuse + specular) * vec3(light.color);
	}
	else
		return vec3(0.0, 0.0, 0.0);
}