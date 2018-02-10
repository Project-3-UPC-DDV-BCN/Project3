#pragma once

#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "Timer.h"
#include "Color.h"

#include <vector>
#include <string>

using namespace std; 

class ComponentParticleEmmiter; 
class ComponentBillboard; 

struct ParticleAnimation
{
	ParticleAnimation();
	~ParticleAnimation();

	void Start();

	int GetNumFrames();
	void PaintStackUI();

	void AddToFrameStack(Texture* new_texture); 
	vector<Texture*>::iterator DeleteFromFrameStack(int to_del);

	Texture* GetCurrentTexture();
	
	string name;
	vector<Texture*> frames_stack;
	int rendering_frame;
	float timeStep;

	Timer switcher_timer; 

};

struct ParticleComponents
{
	Mesh* particle_mesh;
	Texture* texture = nullptr;

	ComponentTransform* particle_transform;
	ComponentBillboard* billboard; 

	ParticleAnimation particle_animation;

	void SetToNull()
	{
		particle_mesh = nullptr;
		particle_transform = nullptr;
		billboard = nullptr;
	}
};

class Particle
{
public:

	Particle(ComponentParticleEmmiter* parent);

	void Update();

	bool CheckIfDelete();
	void DeleteNow();

	//Drawing
	void Draw(ComponentCamera* active_camera);

	//Setters & Getters
	ParticleComponents GetAtributes();

	////Billboarding
	//void SetBillboardReference(ComponentCamera* new_reference);
	//ComponentCamera* GetBillboardReference();

	////Animation
	//void UpdateAnimation();

	//Interpolation
	///Color
	void UpdateColor();
	bool IsInterpolatingColor() const;
	void SetInterpolatingColor(bool interpolate, Color initial_color, Color final_color);

	//Scale
	void UpdateSize();
	void SetInterpolationSize(bool interpolate, float3 initial_scale, float3 final_scale);

	//Rotation
	//void UpdateRotation();
	//void SetInterpolationRotation(bool active, float initial_v, float final_v);

	/////Force calculations
	//void SetMovement(float3 mov);
	void SetGravity(float3 grav);

	//Velocity
	void SetVelocity(const float& new_velocity);
	float GetVelocity() const;

	//Rotation
	void SetAngular(const float& new_velocity);
	float GetAngular() const;
	void ApplyAngularVelocity(); 

	//Lifetime
	void SetMaxLifetime(const float& new_lifetime);
	float GetMaxLifeTime() const;

	//Texture
	void SetParticleTexture(Texture* new_texture); 

	//Color
	void SetColor(Color new_color);
	Color GetColor() const;

	Color GetInitialColor() const;
	Color GetFinalColor() const;

	void SetInitialColor(Color color);
	void SetFinalColor(Color color);

	//Movement
	void SetMovementFromStats(); 
	void SetMovement(); 

	float GetDistanceToCamera();

	void SetBillboarding(float new_dist);
	float IsBillboarding() const;

	bool IsDead();

	void SetWorldSpace(bool is_space); 
	bool IsWorldSpace(); 
	void ApplyWorldSpace(); 

	//Animation
	ParticleAnimation* GetAnimationController(); 

	~Particle();

public:

	ParticleComponents components;			//Components of the particle
	bool animated_particle;

private:

	//Timers
	Timer particle_timer;
	Timer twister;
	Timer interpolation_timer;
	Timer animation_timer;

	//Color
	Color particle_color;

	//Interpolations
	bool interpolate_colors;
	Color initial_particle_color;
	Color final_particle_color;

	bool interpolate_size;
	float3 initial_particle_size;
	float3 final_particle_size;

	bool is_relative; 

	bool interpolate_rotation;
	float initial_particle_angular_v;
	float final_particle_angular_v;
	float curr_rot;
	int color_difference[4];

	//Managing movement
	float particle_velocity;
	float3 particle_gravity;
	float particle_angular_v;
	float max_particle_lifetime;
	float3 movement;						//This vector will be added to the position every frame

	bool kill_me;
	float distance_to_camera;
	bool billboarding;

	ComponentParticleEmmiter* emmiter; 

};
