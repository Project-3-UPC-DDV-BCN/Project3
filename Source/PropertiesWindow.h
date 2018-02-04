#pragma once

#include "Window.h"
#include <map>

class ComponentMeshRenderer;
class ComponentTransform;
class ComponentCamera;
class ComponentParticleEmmiter; 
class ComponentBillboard;
class Component;
class ComponentScript;
class ComponentFactory;
class GameObject;

class PropertiesWindow :
	public Window
{
public:
	PropertiesWindow();
	virtual ~PropertiesWindow();

	void DrawWindow();
	void DrawComponent(Component* component);
	void DrawTransformPanel(ComponentTransform* transform);
	void DrawMeshRendererPanel(ComponentMeshRenderer* spriteRenderer);
	void DrawCameraPanel(ComponentCamera* camera);
	void DrawScriptPanel(ComponentScript* script);
	void DrawFactoryPanel(ComponentFactory* factory);
	void DrawParticleEmmiterPanel(ComponentParticleEmmiter* camera);
	void DrawBillboardPanel(ComponentBillboard* camera);

private:
	int scripts_count;
	int factories_count;
};

