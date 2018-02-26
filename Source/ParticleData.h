#pragma once

#include <string>
#include <vector>
#include "ComponentBillboard.h"
#include "Texture.h"
#include "Resource.h"
#include "Mesh.h"
#include "Color.h"
#include "Timer.h"
#include "MathGeoLib\MathGeoLib.h"

using namespace std; 

enum emmision_behaviour
{
	EMMISION_CONTINUOUS,
	EMMISION_SIMULTANEOUS,
};

struct ShockWave
{
	float duration;
	float final_scale;

	Timer wave_timer;

	GameObject* object;

	Mesh* wave_mesh;
	ComponentTransform* wave_transform;
	Texture* wave_texture; 

	bool active;
	bool done; 

	void ToNull()
	{
		object = nullptr;
		wave_mesh = nullptr; 
		duration = 0; 
		final_scale = 0;
		active = false; 
		done = false; 
		wave_transform = nullptr; 
		wave_texture = nullptr; 
	}
};

struct ParticleAnimation
{
public: 

	void Init(); 
	void Start();

	int GetNumFrames();
	void PaintStackUI();

	void AddToFrameStack(Texture* new_texture);
	vector<Texture*>::iterator DeleteFromFrameStack(int to_del);

	Texture* GetCurrentTexture();

public: 

	vector<Texture*> frames_stack;
	int rendering_frame;
	float timeStep;

	Timer switcher_timer;
};

class ParticleData : public Resource
{

public: 

	ParticleData(); 
	~ParticleData();

	//Loading Functions
	void LoadDefaultData();

	void Save(Data& data) const;
	void SaveTextures(Data& data);

	void Copy(ParticleData* other); 

	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

public: 

	//Animations 
	ParticleAnimation animation_system;
	ShockWave shock_wave; 

	//UI Data -------	
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
	float global_scale; 
	float3 initial_scale;
	float3 final_scale;

	///Rotation
	bool change_rotation_interpolation;
	float initial_angular_v;
	float final_angular_v;

	///Alpha
	bool change_alpha_interpolation;
	float init_alpha_interpolation_time;
	bool alpha_interpolation_delayed;

	///Color
	bool change_color_interpolation;
	Color initial_color;
	Color final_color;

	//Animation system 
	float time_step;

	//Booleans
	bool billboarding;
	bool relative_pos;

	//AutoPause
	bool autopause; 
	float time_to_stop;

	//Simultaneous shot 
	emmision_behaviour emmision_type;
	float amount_to_emmit;
	float time_step_sim;

};
