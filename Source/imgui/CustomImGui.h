#pragma once

#include "imgui.h"
#include "../Shader.h"

class Texture;
class Mesh;
class Prefab;
class GameObject;
class Material;
class Script;

namespace ImGui
{
#define IMGUI_DEFINE_MATH_OPERATORS
	bool InputResourceTexture(const char* label, Texture** texture);
	bool InputResourceMesh(const char* label, Mesh** texture);
	bool InputResourcePrefab(const char* label, Prefab** texture);
	bool InputResourceGameObject(const char* label, GameObject** texture);
	bool InputResourceMaterial(const char* label, Material** texture);
	bool InputResourceScript(const char* label, Script** script);
	bool InputResourceShader(const char* label, Shader** shader, Shader::ShaderType type);
}

