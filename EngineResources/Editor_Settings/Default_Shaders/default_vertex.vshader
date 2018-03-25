#version 400 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec4 color;

layout(location = 4) in vec3 tangents;
layout(location = 5) in vec3 bitangents;
out vec4 ourColor;
out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

out vec3 TangentFragPos;
out mat3 TBN;
uniform mat4 Model;
uniform mat4 view;
uniform mat4 projection;

void main()
{ 
	gl_Position = projection * view * Model * vec4(position, 1.0f);
	FragPos = vec3(Model * vec4(position, 1.0));	Normal = normals;
	mat3 normalMatrix = transpose(inverse(mat3(Model)));
	vec3 T = normalize(normalMatrix * tangents);
	vec3 N = normalize(normalMatrix * normals);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	TBN = transpose(mat3(T, B, N));
	TangentFragPos = TBN * FragPos;
	ourColor = color;
	TexCoord = texCoord.xy;
}