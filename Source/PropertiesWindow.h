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
class ComponentLight;
class GameObject;
class ComponentRigidBody;
class ComponentCollider;
class ComponentJointDistance;


class PropertiesWindow :
	public Window
{
public:
	PropertiesWindow();
	virtual ~PropertiesWindow();

	void DrawWindow();
	void DrawComponent(Component* component);
	void DrawTransformPanel(ComponentTransform* transform);
	void DrawMeshRendererPanel(ComponentMeshRenderer* mesh_renderer);
	void DrawCameraPanel(ComponentCamera* camera);
	void DrawScriptPanel(ComponentScript* script);
	void DrawFactoryPanel(ComponentFactory* factory);
	void DrawRigidBodyPanel(ComponentRigidBody* rigidbody);
	void DrawColliderPanel(ComponentCollider* collider);
	void DrawJointDistancePanel(ComponentJointDistance* joint);
	void DrawAudioListener(ComponentListener* listener);
	void DrawAudioSource(ComponentAudioSource* audio_source);
	void DrawAudioDistZone(ComponentDistorsionZone* dist_zone);
	void DrawLightPanel(ComponentLight* light);

private:
	int scripts_count;
	int factories_count;
	int colliders_count;
	int distance_joints_count;
	int lights_count;
};

