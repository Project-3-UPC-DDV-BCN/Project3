#version 330 core
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
out vec4 color;

uniform bool has_material_color;
uniform vec4 material_color;
uniform bool has_texture;
uniform vec4 light_color;
uniform sampler2D ourTexture;
uniform vec3 camera_pos;

struct Light 
{
vec3 position;
vec3 direction;
float cutOff;

vec3 ambient;
vec3 diffuse;
vec3 specular;

float constant;
float linear;
float quadratic;
};

uniform Light light;

void main()
{
	if(has_texture)
	{
		color = texture(ourTexture, TexCoord);
		}
	else if(has_material_color)
		color = material_color;
	else
		color = ourColor;
		color = color * light_color;
    vec3 ambient = light.ambient * vec3(light_color);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(light_color);
	
	vec3 view_direction = normalize(camera_pos - FragPos);
	vec3 reflect_direction = reflect(-lightDir, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 64);
	vec3 specular = light.specular * spec * vec3(light_color);
	
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 result = (ambient + diffuse + specular);
	color = vec4(result, 1.0) * color;
}