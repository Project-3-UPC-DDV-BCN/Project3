#pragma once

#include "Window.h"
#include <map>

class ComponentMeshRenderer;
class ComponentTransform;
class ComponentCamera;
class Component;
class ComponentScript;
class ComponentFactory;
class ComponentListener;
class ComponentAudioSource;
class ComponentDistorsionZone;
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
	void DrawAudioListener(ComponentListener* listener);
	void DrawAudioSource(ComponentAudioSource* audio_source);
	void DrawAudioDistZone(ComponentDistorsionZone* dist_zone);
private:
	int scripts_count;
	int factories_count;
};

