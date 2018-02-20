#include "ParticleData.h"

ParticleData::ParticleData()
{
}

ParticleData::~ParticleData()
{
}

void ParticleData::LoadDefaultData()
{
	SetName("Default");

	emmision_rate = 15;
	max_lifetime = 1;
	velocity = 5.0f;
	color = Color(255, 255, 255, 0);
	billboarding = false;
	gravity = { 0,0,0 };
	angular_v = 0;
	emision_angle = 0;
	is_animated = false;
	time_step = 0.2;

	emmit_width = 1;
	emmit_height = 1;
	emmit_depth = 1;

	width_increment = 0;
	height_increment = 0;
	depth_increment = 0;

	relative_pos = false;
	billboarding = false;

	change_rotation_interpolation = false;
	change_size_interpolation = false;
	change_color_interpolation = false;

	initial_scale = { 1,1,1 };
	final_scale = { 1,1,1 };

	initial_angular_v = 0;
	final_angular_v = 0;

	initial_color[0] = initial_color[1] = initial_color[2] = initial_color[3] = 0;
	final_color[0] = final_color[1] = final_color[2] = final_color[3] = 0;
}

void ParticleData::Save(Data & data) const
{
}

bool ParticleData::Load(Data & data)
{
	return false;
}

void ParticleData::CreateMeta() const
{
}

void ParticleData::LoadToMemory()
{
}

void ParticleData::UnloadFromMemory()
{
}
