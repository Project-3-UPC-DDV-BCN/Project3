#include "ParticleData.h"
#include "Texture.h"

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
	change_alpha_interpolation = false; 

	init_alpha_interpolation_time = 0; 

	initial_scale = { 1,1,1 };
	final_scale = { 1,1,1 };

	initial_angular_v = 0;
	final_angular_v = 0;

	initial_color = { 0,0,0,0 }; 
	final_color = { 0,0,0,0 };

	animation_system.Init(); 
	
}

void ParticleData::Save(Data & data) const
{

	//data.AddInt("Type", GetType());
	//data.AddBool("Active", IsActive());
	//data.AddUInt("UUID", GetUID());
	data.CreateSection("Particle");
	//data.AddBool("Show_Emit_Area", show_emit_area);

	// Emmit area -----

	data.AddString("Name", GetName()); 

	data.AddFloat("Emit_Width",emmit_width);
	data.AddFloat("Emit_Height", emmit_height);
	data.AddFloat("Emit_Depth", emmit_depth);

	// Textures ----
	

	//------

	// Colors -----

	// ------

	// Motion -----

	data.AddBool("Relative_Pos", relative_pos);
	data.AddInt("Emmision_Rate", emmision_rate);
	data.AddFloat("Lifetime", max_lifetime);
	data.AddFloat("Initial_Velocity", velocity);
	data.AddVector3("Gravity", gravity);
	data.AddFloat("Angular_Velocity", angular_v);
	data.AddFloat("Emmision_Angle", emision_angle);

	// ------

	// Interpolation -----

	if (change_color_interpolation)
	{
		data.AddBool("Color_Interpolation", true);
	}
	else
		data.AddBool("Color_Interpolation", false);

	if (change_size_interpolation)
	{
		data.AddBool("Size_Interpolation", true);

		data.AddVector3("Initial_Size", initial_scale);
		data.AddVector3("Final_Size",final_scale);
	}
	else
		data.AddBool("Size_Interpolation", false);

	if (change_rotation_interpolation)
	{
		data.AddBool("Rotation_Interpolation", true);

		data.AddFloat("Initial_Rotation", initial_angular_v);
		data.AddFloat("Final_Rotation", final_angular_v);
	}
	else
		data.AddBool("Rotation_Interpolation", false);

	// ------

	data.CloseSection();
}

void ParticleData::SaveTextures(Data& data)
{
	if (animation_system.GetNumFrames() == 0)
		data.AddBool("Has_Texture", false);
	else
	{
		data.AddBool("Has_Texture", true);

		int frame_num = 1;

		for (vector<Texture*>::iterator it = animation_system.frames_stack.begin(); it != animation_system.frames_stack.end(); it++)
		{
			string tex_name("Frame_");
			tex_name += to_string(frame_num);
			frame_num++;

			data.AddInt(tex_name, (*it)->GetUID());
		}
	}
}

void ParticleData::Copy(ParticleData * other)
{
	animation_system = other->animation_system;
									
	max_lifetime = other->max_lifetime;
	emmision_rate = other->emmision_rate;
	velocity = other->velocity;
									
	gravity	= other->gravity;
	emision_angle = other->emision_angle;
									
	color = other->color;
	angular_v	= other->angular_v;
	billboard_type	= other->billboard_type;
									
	emmit_width	= other->emmit_width;
	emmit_height = other->emmit_height;
	emmit_depth	= other->emmit_depth;
									
	width_increment	= other->width_increment;
	height_increment = other->height_increment;
	depth_increment = other->depth_increment;
									
	change_size_interpolation = other->change_size_interpolation;
	initial_scale = other->initial_scale;
	final_scale = other->final_scale;
									
	change_rotation_interpolation = other->change_rotation_interpolation;
	initial_angular_v = other->initial_angular_v;
	final_angular_v = other->final_angular_v;
																	
	change_color_interpolation = other->change_color_interpolation;
	initial_color = other->initial_color;
	final_color = other->final_color;
									
	time_step = other->time_step;
							
	billboarding = other->billboarding;
	relative_pos = other->relative_pos;
}

bool ParticleData::Load(Data & _data)
{
	// Emmit area -----
	_data.EnterSection("Particle");

	string name = _data.GetString("Name");
	SetName(name.c_str());

	emmit_width = _data.GetFloat("Emit_Width");
	emmit_height = _data.GetFloat("Emit_Height");
	emmit_depth = _data.GetFloat("Emit_Depth");
	
	// Textures ----
	if (animation_system.GetNumFrames() == 0)
		_data.AddBool("Has_Texture", false);
	else
	{
		_data.AddBool("Has_Texture", true);

		int frame_num = 1;
		for (vector<Texture*>::iterator it = animation_system.frames_stack.begin(); it != animation_system.frames_stack.end(); it++)
		{
			string tex_name("Frame_");
			tex_name += to_string(frame_num);
			frame_num++;

			_data.AddInt(tex_name, (*it)->GetUID());
		}
	}

	// Colors -----


	// Motion -----

	relative_pos = _data.GetBool("Relative_Pos");
	emmision_rate = _data.GetFloat("Emmision_Rate");
	max_lifetime = _data.GetFloat("Lifetime");
	velocity = _data.GetFloat("Initial_Velocity");
	gravity = _data.GetVector3("Gravity");
	angular_v = _data.GetFloat("Angular_Velocity");
	emision_angle = _data.GetFloat("Emmision_Angle");

	// ------

	// Interpolation -----

	change_color_interpolation = _data.GetBool("Color_Interpolation");

	change_size_interpolation = _data.GetBool("Size_Interpolation");

	if (change_size_interpolation)
	{
		initial_scale = _data.GetVector3("Initial_Size");
		final_scale = _data.GetVector3("Final_Size");
	}

	change_rotation_interpolation = _data.GetBool("Rotation_Interpolation");

	if (change_rotation_interpolation)
	{
		initial_angular_v = _data.GetFloat("Initial_Rotation");
		final_angular_v = _data.GetFloat("Final_Rotation");
	}

	// ------

	//Function calling ----
	width_increment = emmit_width;
	height_increment = emmit_height;
	depth_increment =emmit_depth;

	//	AddaptEmmitAreaAABB(); 

	_data.LeaveSection();

	return true;
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

int ParticleAnimation::GetNumFrames()
{
	return frames_stack.size();
}

void ParticleAnimation::PaintStackUI()
{
	int number = 1;
	for (vector<Texture*>::iterator it = frames_stack.begin(); it != frames_stack.end();)
	{
		ImGui::Text(to_string(number).c_str()); ImGui::SameLine();

		ImGui::Text(". "); ImGui::SameLine();
		ImGui::Text((*it)->GetName().c_str()); ImGui::SameLine();

		string button_name("X##");
		button_name += to_string(number);

		if (ImGui::Button(button_name.c_str()))
		{
			it = DeleteFromFrameStack(number);

			if (frames_stack.empty())
				break;
		}
		else
			it++;

		number++;
	}
}

void ParticleAnimation::AddToFrameStack(Texture * new_texture)
{
	frames_stack.push_back(new_texture);
}

vector<Texture*>::iterator ParticleAnimation::DeleteFromFrameStack(int to_del)
{
	int counter = 0;
	vector<Texture*>::iterator it;

	for (it = frames_stack.begin(); it != frames_stack.end(); it++)
	{
		if (counter++ == to_del - 1)
		{
			it = frames_stack.erase(it);
			return it;
		}

	}
	return it;
}

Texture * ParticleAnimation::GetCurrentTexture()
{
	if (GetNumFrames() == 0)
		return nullptr;

	else if (GetNumFrames() == 1)
		rendering_frame = 0;

	if (switcher_timer.Read() > timeStep * 1000 && timeStep != 0)
	{
		if (rendering_frame >= frames_stack.size() - 1)
			rendering_frame = 0;
		else
			rendering_frame++;

		switcher_timer.Start();
	}

	return frames_stack[rendering_frame];
}


void ParticleAnimation::Init()
{
	frames_stack.clear(); 
	rendering_frame = 0; 
	timeStep = 0; 
}

void ParticleAnimation::Start()
{
	switcher_timer.Start();
}
