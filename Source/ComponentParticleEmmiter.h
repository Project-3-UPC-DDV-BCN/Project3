#pragma once

#include "Component.h"
#include "ComponentBillboard.h"
#include "Particles.h"
#include <map>

class ComponentCamera; 
using namespace std; 

enum particle_system_state
{
	PARTICLE_STATE_PLAY,
	PARTICLE_STATE_PAUSE,
};

class ComponentParticleEmmiter : public Component
{

public:
	ComponentParticleEmmiter(GameObject* parent);

	bool Start();
	void CreateRootParticle();

	bool Update();
	void UpdateRootParticle();
	void DrawParticles(ComponentCamera* active_camera); 
	void AddaptEmmitAreaAABB(); 

	~ComponentParticleEmmiter();

	void Save(Data& data) const;
	void Load(Data& data);

	void ReoderParticles(ComponentCamera* rendering_camera);

	//Spawning
	void GenerateParticles();
	Particle* CreateParticle();
	int GetParticlesNum();
	void DeleteLastParticle(); 

	//Getters & Setters
	int GetEmmisionRate() const;
	void SetEmmisionRate(float new_emision_rate);
	void SetFrequencyFromRate(int rate);

	bool ShowEmmisionArea() const;
	void SetShowEmmisionArea(bool new_show);

	float GetLifetime() const;
	void SetLifeTime(float new_show);

	particle_system_state GetSystemState() const;
	void SetSystemState(particle_system_state new_state);

	int GetTextureID(int pos);
	int GetTextureIDAmount();

	void SetCurrentTextureID(uint texture_id);
	uint GetCurrentTextureID() const;

	vector<ParticleAnimation> GetAllParticleAnimations();
	Particle* GetRootParticle() const;

public:

	//UI data ------

	//Emiter

	//Shape
	uint curr_texture_id;					//Hold the texture that will be drawn into the particles that we are cloning
											
	Color color;							//Color
	float angular_v;
	BillboardingType billboard_type; 

	float emmit_width; 
	float emmit_height; 
	float emmit_depth; 

	float width_increment; 
	float height_increment; 
	float depth_increment; 


	//Interpolations
	int initial_color[4];
	int final_color[4];

	float3 initial_scale;
	float3 final_scale;

	float initial_angular_v;
	float final_angular_v;

	bool change_color_interpolation;			//If true, the particles instanciated will be given 2 colors and they will interpolate between them
	bool change_size_interpolation;
	bool change_rotation_interpolation;

	//Animated particle 
	bool is_animated;
	float time_step;

	//Motion
	bool billboarding;
	bool lock_billboarding_y;
	bool lock_billboarding_x;
	bool relative_pos;

	float max_lifetime;						//Time that particules will be rendering
	int emmision_rate;						//How many spawn in a second
	float velocity;							//Velocity at what the particles are emmited

	float3 gravity;							//Gravity
	float emision_angle;					//This is the max angle thet will be given to the particle
											
	OBB emit_area_obb; 

private:

private:

	//General Management
	Particle* root_particle;				 //This will be the particle that will be cloned over time
	multimap<float, Particle*> active_particles;		 //Particles that are currently beeing rendered

	vector<ParticleAnimation> particle_animations;

	float particles_lifetime;				 //Lifetime of the particules spawned
	particle_system_state system_state;		 //Inner play & pause 

	Timer reorder_time;

	//Spawn Management
	float emmision_frequency;				//Difference between spawn
	Timer spawn_timer;

	//Debug
	bool show_emit_area;					//boolean for showing the emmiter area
	vector<uint> shapes_ids;				//This list will hold the id's of the textures that can give shape to the particles
	uint shapes_amount;
};