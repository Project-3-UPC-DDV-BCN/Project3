#pragma once

#include "Component.h"
#include "ComponentBillboard.h"
#include "MathGeoLib\MathGeoLib.h"
#include "Particles.h"
#include <map>

class ComponentCamera; 
class ParticleData; 
using namespace std; 

enum particle_system_state
{
	PARTICLE_STATE_PLAY,
	PARTICLE_STATE_PAUSE,
};


class ComponentParticleEmmiter : public Component
{

public:
	//Common methods
	ComponentParticleEmmiter(GameObject* parent);
	~ComponentParticleEmmiter();

	bool Start();
	bool Update();

	void Save(Data& data) const;
	void Load(Data& data);

	void SaveSystemToBinary(); 

	//Root particle
	void UpdateCurrentData();

	//Draws
	void DrawParticles(ComponentCamera* active_camera); 

	//Particle Management
	void GenerateParticles();
	Particle* CreateParticle();
	int GetParticlesNum();
	void DeleteLastParticle();
	void ReoderParticles(ComponentCamera* rendering_camera);
	void AddaptEmmitAreaAABB();

	//Getters & Setters ----
	///Emmision Rate
	int GetEmmisionRate() const;
	void SetEmmisionRate(float new_emision_rate);
	void SetFrequencyFromRate(int rate);

	///Emmision area
	bool ShowEmmisionArea() const;
	void SetShowEmmisionArea(bool new_show);

	///System state
	particle_system_state GetSystemState() const;
	void SetSystemState(particle_system_state new_state);

	///Root particle
	Particle* GetRootParticle() const;

	//ShockWave
	void CreateShockWave(Texture*, float duration, float final_scale); 
	void UpdateShockWave(); 
	void DrawShockWave(ComponentCamera* active_camera);

	void PlayEmmiter();
	void StopEmmiter(); 

	//Simultaneous launching 
	bool LaunchingAllowed(); 
	void LaunchParticlesWave(); 


public:

	ParticleData* data; 
	Timer global_timer;									//Global emmiter timer
		
	//Emmit area AABB
	AABB emmit_area; 

	float prev_emmiter_width;
	float prev_emmiter_height;
	float prev_emmiter_depth; 

	string runtime_behaviour; 

	bool show_shockwave;

private:

private:

	//Lists
	multimap<float, Particle*> active_particles;		//Particles that are currently beeing rendered

	//Timers
	Timer spawn_timer;									//Timer to control the emmision rate 
	
	//Data
	particle_system_state system_state;					//Inner play & pause 
	
	//Spawn Management
	float emmision_frequency;							//Difference between spawn
	bool show_emit_area; 
	bool wave_launched; 
};