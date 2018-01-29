#pragma once

#include "imgui.h"
#include "../ResourcesWindow.h"

class Texture;
class Mesh;
class Prefab;
class GameObject;
class Material;
class Script;
class PhysicsMaterial;
class BlastMesh;

namespace ImGui
{
#define IMGUI_DEFINE_MATH_OPERATORS
	bool InputResourceTexture(const char* label, Texture** texture);
	bool InputResourceMesh(const char* label, Mesh** mesh);
	bool InputResourcePrefab(const char* label, Prefab** prefab);
	bool InputResourceGameObject(const char* label, GameObject** gameobject, ResourcesWindow::GameObjectFilter filter = ResourcesWindow::GameObjectFilter::GoFilterNone);
	bool InputResourceMaterial(const char* label, Material** mat);
	bool InputResourceScript(const char* label, Script** script);
	bool InputResourcePhysMaterial(const char* label, PhysicsMaterial** phys_mat);
	bool InputResourceBlastMesh(const char* label, BlastMesh** mesh);
}

