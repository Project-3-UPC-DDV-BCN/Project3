#pragma once

#include "Window.h"
#include <map>

class ComponentMeshRenderer;
class ComponentTransform;
class ComponentCamera;
class Component;
class ComponentScript;
class ComponentFactory;
class ComponentRectTransform;
class ComponentCanvas;
class ComponentImage;
class ComponentText;
class ComponentLight;
class GameObject;
class Texture;

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
	void DrawRectTransformPanel(ComponentRectTransform* rect_transform);
	void DrawCanvasPanel(ComponentCanvas* canvas);
	void DrawImagePanel(ComponentImage* image);
	void DrawTextPanel(ComponentText* text);
	void DrawLightPanel(ComponentLight* light);

private:
	int scripts_count;
	int factories_count;
	int lights_count;

	Texture* texture1_ = nullptr;
	Texture* texture2_ = nullptr;
};

