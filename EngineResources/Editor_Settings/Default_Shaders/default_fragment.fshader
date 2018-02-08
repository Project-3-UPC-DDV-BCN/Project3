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

uniform vec3 LightPos;

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
		
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(light_color);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(light_color);
	
	float specularStrength = 0.8;
	vec3 view_direction = normalize(camera_pos - FragPos);
	vec3 reflect_direction = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 64);
	vec3 specular = specularStrength * spec * vec3(light_color);
	
	vec3 result = (ambient + diffuse + specular);
	color = vec4(result, 1.0) * color;
}