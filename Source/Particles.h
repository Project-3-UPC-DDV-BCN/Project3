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
class ParticleData; 

struct ParticleComponents
{
	Mesh* particle_mesh;
	Texture* texture = nullptr;

	ComponentTransform* particle_transform;
	ComponentBillboard* billboard; 

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
	~Particle();

	void Update();

	bool CheckIfDelete();
	void DeleteNow();

	//Drawing
	void Draw(ComponentCamera* active_camera);

	///Color
	void UpdateColor();
	bool IsInterpolatingColor() const;
	void SetInterpolatingColor(bool interpolate, Color initial_color, Color final_color);

	//Scale
	void UpdateSize();
	void SetInterpolationSize(bool interpolate, float3 initial_scale, float3 final_scale);

	//Rotation
	void UpdateRotation();
	void SetInterpolationRotation(float initial_v, float final_v);

	/////Force calculations
	//void SetMovement(float3 mov);
	void SetGravity(float3 grav);

	//Velocity
	void SetVelocity(const float& new_velocity);
	float GetVelocity() const;

	//Alpha
	float GetAlphaInterpolationPercentage();

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

	//Emission angle
	float GetEmmisionAngle();
	void SetEmmisionAngle(float new_angle); 
	float3 GetEmmisionVector();

	//Movement
	void SetMovementFromStats(); 
	void SetMovement(); 

	float GetDistanceToCamera();

	void SetBillboarding(float new_dist);
	float IsBillboarding() const;

	void SetWorldSpace(bool is_space); 
	bool IsWorldSpace(); 
	void ApplyWorldSpace(); 

	//Setters & Getters ----
	ParticleComponents GetAtributes();
	ParticleData* GetData(); 

	// ----



public:

	ParticleComponents components;			//Components of the particle
	ParticleData* particle_data;

private:

	//Timers
	Timer particle_timer;
	Timer twister;
	Timer interpolation_timer;
	Timer animation_timer;

	//Extra data
	float distance_to_camera;
	float3 movement;
	int color_difference[4];

	//Alpha
	bool alpha_delayed; 

	//Parent emmiter
	ComponentParticleEmmiter* emmiter; 

};
