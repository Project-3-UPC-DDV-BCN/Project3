#pragma once

#include <string>
#include "ComponentBillboard.h"
#include "Resource.h"
#include "Color.h"
#include "MathGeoLib\MathGeoLib.h"

using namespace std; 

class ParticleData : public Resource
{

public: 

	ParticleData(); 
	~ParticleData();

	//Loading Functions
	void LoadDefaultData();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

public: 

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


};
