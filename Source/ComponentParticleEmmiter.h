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

struct ParticleData
{
	//UI Data -------	
	string name; 

	float max_lifetime;						//Time that particules will be rendering
	int emmision_rate;						//How many spawn in a second
	float velocity;							//Velocity at what the particles are emmited

	float3 gravity;							//Gravity
	float emision_angle;					//This is the max angle thet will be given to the particle

	Color color;
	float angular_v;
	BillboardingType billboard_type;

	float emmit_width;
	float emmit_height;
	float emmit_depth;

	float width_increment;
	float height_increment;
	float depth_increment;

	//Interpolations
	///Size
	bool change_size_interpolation;
	float3 initial_scale;
	float3 final_scale;

	///Rotation
	bool change_rotation_interpolation;
	float initial_angular_v;
	float final_angular_v;

	///Alpha

	///Color
	bool change_color_interpolation;
	int initial_color[4];
	int final_color[4];

	//Animation system 
	bool is_animated;
	float time_step;

	//Booleans
	bool billboarding;
	bool relative_pos;

	//Loading Functions
	void LoadDefaultData(); 
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
	void CreateRootParticle();
	void UpdateRootParticle();

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

	///Lifetime
	float GetLifetime() const;
	void SetLifeTime(float new_show);

	///System state
	particle_system_state GetSystemState() const;
	void SetSystemState(particle_system_state new_state);

	///Root particle
	Particle* GetRootParticle() const;

	//Particle Data Management
	void SaveCurrentDataAsTemplate(const char* new_template_name);
	void AddNewTemplate(ParticleData new_data);
	void DeleteTemplate(int index); 
	vector<string> GetTemplatesVector();


public:

	ParticleData data; 
		
	//Emmit area AABB
	OBB emmit_area_obb; 

private:

private:

	//Lists
	Particle* root_particle;							//This will be the particle that will be cloned over time
	multimap<float, Particle*> active_particles;		//Particles that are currently beeing rendered
	list<ParticleData> template_list; 

	//Timers
	Timer spawn_timer;									//Timer to control the emmision rate 

	//Data
	float particles_lifetime;							//Lifetime of the particules spawned
	particle_system_state system_state;					//Inner play & pause 
	
	//Spawn Management
	float emmision_frequency;							//Difference between spawn

	//Debug
	bool show_emit_area;								//boolean for showing the emmiter area
	
};