#include "ParticleData.h"
#include "Application.h"
#include "ModuleTextureImporter.h"
#include "ModuleResources.h"
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
	billboard_type = BILLBOARD_ALL; 
	emmision_type = EMMISION_CONTINUOUS; 
	runtime_behaviour_ = EMMIT_ALWAYS; 
	emmit_style = EMMIT_FROM_RANDOM;
	billboarding = false;
	gravity = { 0,0,0 };
	angular_v = 0;
	emision_angle = 0;
	time_step = 0.2;
	global_scale = 1; 

	autopause = false; 
	time_to_stop = 0; 

	shock_wave.ToNull(); 

	emmit_width = 1;
	emmit_height = 1;
	emmit_depth = 1;

	relative_pos = false;
	billboarding = false;

	change_rotation_interpolation = false;
	change_velocity_interpolation = false; 
	change_size_interpolation = false;
	change_color_interpolation = false;
	change_alpha_interpolation = false; 

	init_alpha_interpolation_time = 0; 

	v_interpolation_start_time = 0.0f; 
	v_interpolation_end_time = 0.0f;
	v_interpolation_final_v = 0.0f; 

	initial_scale = { 1,1,1 };
	final_scale = { 1,1,1 };

	initial_angular_v = 0;
	final_angular_v = 0;

	color = { 1,1,1,1 }; 
	initial_color = { 0,0,0,0 }; 
	final_color = { 0,0,0,0 };

	amount_to_emmit = 0;
	time_step_sim = 0;

	animation_system.Init(); 
	
}

void ParticleData::Save(Data & data) const
{

	data.CreateSection("Particle");

	data.AddInt("Emmision_Type", (int)emmision_type);

	if (emmision_type == EMMISION_SIMULTANEOUS)
	{
		data.AddFloat("TimeStepSim", time_step_sim); 
		data.AddInt("SimAmount", amount_to_emmit); 
	}

	//Runtime behaviour (MANUAL/AUTO)

	data.AddInt("Runtime_Behaviour", (int)runtime_behaviour_);

	// Emmit area -----

	data.AddString("Name", GetName()); 

	data.AddFloat("Emit_Width", emmit_width);
	data.AddFloat("Emit_Height", emmit_height);
	data.AddFloat("Emit_Depth", emmit_depth);

	data.AddInt("Emmision_Style", (int)emmit_style);

	//billboarding 
	data.AddBool("Billboard", billboarding); 

	if (billboard_type == BILLBOARD_X)
		data.AddString("Billboard_Axis", "X"); 

	else if (billboard_type == BILLBOARD_Y)
		data.AddString("Billboard_Axis", "Y");

	else if (billboard_type == BILLBOARD_ALL)
		data.AddString("Billboard_Axis", "ALL");

	//AutoPause
	data.AddBool("AutoPause", autopause); 

	if(autopause)
		data.AddFloat("Time_To_Stop", time_to_stop); 

	// Colors -----

	float4 new_col = {color.r, color.g, color.b, color.a};
	data.AddVector4("Color", new_col);

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

	data.AddFloat("Global_Scale", global_scale); 

	if (change_velocity_interpolation)
	{
		data.AddBool("Velocity_Interpolation", true);

		data.AddFloat("Initial_V_Time", v_interpolation_start_time);
		data.AddFloat("Final_V_Time", v_interpolation_end_time);
		data.AddFloat("Final_V_Speed", v_interpolation_final_v);
	}
	else
		data.AddBool("Velocity_Interpolation", false);

	if (change_color_interpolation)
	{
		data.AddBool("Color_Interpolation", true);

		float3 init_color = { initial_color .r, initial_color .g, initial_color .b};
		float3 fin_color = { final_color.r, final_color.g, final_color.b };

		data.AddVector3("Initial_Color", init_color);
		data.AddVector3("Final_Color", fin_color);
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

	if (change_alpha_interpolation)
	{
		data.AddBool("Alpha_Interpolation", true);	
	}
	else
		data.AddBool("Alpha_Interpolation", false);

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

	data.CreateSection("Particle_Anim");

	//Texture
	if (animation_system.GetNumFrames() > 0)
	{
		int counter = 0; 
		for (vector<Texture*>::iterator it = animation_system.frames_stack.begin(); it != animation_system.frames_stack.end(); it++)
		{
			string tex_name("Frame_");
			tex_name += to_string(counter + 1);
			counter++; 

			data.AddString(tex_name, (*it)->GetLibraryPath()); 
		}		
	}

	data.AddFloat("TimeStep", animation_system.timeStep);
	data.AddInt("Frames_Num", animation_system.GetNumFrames()); 

	data.CloseSection(); 
	
}

void ParticleData::Copy(ParticleData * other)
{
	animation_system = other->animation_system;

	emmision_type = other->emmision_type;
	runtime_behaviour_ = other->runtime_behaviour_; 
	emmit_style = other->emmit_style;
	time_step_sim = other->time_step_sim; 
	amount_to_emmit = other->amount_to_emmit; 

	color = other->color; 
									
	max_lifetime = other->max_lifetime;
	emmision_rate = other->emmision_rate;
	velocity = other->velocity;

	global_scale = other->global_scale; 
									
	gravity	= other->gravity;
	emision_angle = other->emision_angle;
									
	color = other->color;
	angular_v	= other->angular_v;
	billboard_type	= other->billboard_type;
									
	emmit_width	= other->emmit_width;
	emmit_height = other->emmit_height;
	emmit_depth	= other->emmit_depth;

	autopause = other->autopause; 
	time_to_stop = other->time_to_stop; 

	change_alpha_interpolation = other->change_alpha_interpolation;
									
	change_size_interpolation = other->change_size_interpolation;
	initial_scale = other->initial_scale;
	final_scale = other->final_scale;

	change_velocity_interpolation = other->change_velocity_interpolation;
	v_interpolation_start_time = other->v_interpolation_start_time;
	v_interpolation_end_time = other->v_interpolation_end_time;
	v_interpolation_final_v = other->v_interpolation_final_v;

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
	if (_data.EnterSection("Particle"))
	{
		emmision_type = static_cast<emmision_behaviour>(_data.GetInt("Emmision_Type"));
		emmit_style = static_cast<emmision_style>(_data.GetInt("Emmision_Style"));

		if (emmision_type == EMMISION_SIMULTANEOUS)
		{
			time_step_sim = _data.GetFloat("TimeStepSim");
			amount_to_emmit = _data.GetInt("SimAmount");
		}

		runtime_behaviour_ = static_cast<runtime_behaviour>(_data.GetInt("Runtime_Behaviour")); 

		string name = _data.GetString("Name");
		SetName(name.c_str());

		emmit_width = _data.GetFloat("Emit_Width");
		emmit_height = _data.GetFloat("Emit_Height");
		emmit_depth = _data.GetFloat("Emit_Depth");

		// Textures ----

		if (_data.EnterSection("Particle_Anim"))
		{
			int frames_num = _data.GetInt("Frames_Num");

			for (int i = 0; i < frames_num; i++)
			{
				string tex_name("Frame_");
				tex_name += to_string(i + 1);

				string path = _data.GetString(tex_name);

				Texture* texture = App->texture_importer->LoadTextureFromLibrary(path);
				animation_system.AddToFrameStack(texture);
			}

			animation_system.timeStep = _data.GetFloat("TimeStep");

			animation_system.rendering_frame = 0;

			_data.LeaveSection();
		}

		//AutoPause
		autopause = _data.GetBool("AutoPause");

		if (autopause)
			time_to_stop = _data.GetFloat("Time_To_Stop");

		// Colors -----
		float4 col = _data.GetVector4("Color");

		color.r = col.x;
		color.g = col.y;
		color.b = col.z;
		color.a = col.w;

		//Billboard
		billboarding = _data.GetBool("Billboard");

		string billboard_axis = _data.GetString("Billboard_Axis");

		if (billboard_axis == "X")
			billboard_type = BILLBOARD_X;

		else if (billboard_axis == "Y")
			billboard_type = BILLBOARD_Y;

		else if (billboard_axis == "ALL")
			billboard_type = BILLBOARD_ALL;

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
		change_rotation_interpolation = _data.GetBool("Rotation_Interpolation");
		change_alpha_interpolation = _data.GetBool("Alpha_Interpolation");
		change_velocity_interpolation = _data.GetBool("Velocity_Interpolation");

		global_scale = _data.GetFloat("Global_Scale");

		if (change_velocity_interpolation)
		{
			v_interpolation_start_time = _data.GetFloat("Initial_V_Time");
			v_interpolation_end_time = _data.GetFloat("Final_V_Time");
			v_interpolation_final_v = _data.GetFloat("Final_V_Speed");
		}

		if (change_size_interpolation)
		{
			initial_scale = _data.GetVector3("Initial_Size");
			final_scale = _data.GetVector3("Final_Size");
		}

		if (change_rotation_interpolation)
		{
			initial_angular_v = _data.GetFloat("Initial_Rotation");
			final_angular_v = _data.GetFloat("Final_Rotation");
		}

		if (change_color_interpolation)
		{
			float3 initcolor = _data.GetVector3("Initial_Color");
			float3 fin_color = _data.GetVector3("Final_Color");

			initial_color.Set(initcolor.x, initcolor.y, initcolor.z);
			final_color.Set(fin_color.x, fin_color.y, fin_color.z);
			// ------
		}

		_data.LeaveSection();
	}

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
		if ((*it) != nullptr)
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
