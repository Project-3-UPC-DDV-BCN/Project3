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
class ComponentRectTransform;
class ComponentCanvas;
class ComponentImage;
class ComponentText;
class ComponentProgressBar;
class ComponentListener;
class ComponentAudioSource;
class ComponentDistorsionZone;
class ComponentLight;
class GameObject;
class ComponentRigidBody;
class ComponentCollider;
class ComponentJointDistance;
class ComponentGOAPAgent;


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
	void DrawRectTransformPanel(ComponentRectTransform* rect_transform);
	void DrawCanvasPanel(ComponentCanvas* canvas);
	void DrawImagePanel(ComponentImage* image);
	void DrawTextPanel(ComponentText* text);
	void DrawProgressBarPanel(ComponentProgressBar* text);
	void DrawRigidBodyPanel(ComponentRigidBody* rigidbody);
	void DrawColliderPanel(ComponentCollider* collider);
	void DrawJointDistancePanel(ComponentJointDistance* joint);
	void DrawParticleEmmiterPanel(ComponentParticleEmmiter* camera);
	void DrawBillboardPanel(ComponentBillboard* camera);
	void DrawAudioListener(ComponentListener* listener);
	void DrawAudioSource(ComponentAudioSource* audio_source);
	void DrawAudioDistZone(ComponentDistorsionZone* dist_zone);
	void DrawLightPanel(ComponentLight* light);
	void DrawGOAPAgent(ComponentGOAPAgent* goap_agent);

private:
	int scripts_count;
	int factories_count;
	int colliders_count;
	int distance_joints_count;
	bool rename_template; 
	int lights_count;
};

