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
	void DrawRectTransformPanel(ComponentRectTransform* rect_transform);
	void DrawCanvasPanel(ComponentCanvas* canvas);
	void DrawImagePanel(ComponentImage* canvas);

private:
	int scripts_count;
	int factories_count;
	int current_render_mode = 0;
	int current_scale_mode = 0;
};

