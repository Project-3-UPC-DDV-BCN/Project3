#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec4 color;

out vec4 ourColor;
out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 view;
uniform mat4 projection;

void main()
{ 
	gl_Position = projection * view * Model * vec4(position, 1.0f);
	FragPos = vec3(Model * vec4(position, 1.0));	Normal = mat3(transpose(inverse(Model))) * normals;
	ourColor = color;
	TexCoord = texCoord.xy;
}