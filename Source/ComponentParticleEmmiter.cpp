#include "ComponentParticleEmmiter.h"
#include "ComponentMeshRenderer.h"
#include "ComponentBillboard.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ParticleData.h"
#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "Mesh.h"

#include <map>

#include "OpenGL.h"

void ComponentParticleEmmiter::GenerateParticles()
{
	if (system_state == PARTICLE_STATE_PAUSE)
		return;

	if (spawn_timer.Read() > emmision_frequency)
	{
		Particle* new_particle = CreateParticle();
		active_particles.insert(pair<float, Particle* > (new_particle->GetDistanceToCamera(),new_particle));
		spawn_timer.Start();
	}

}

Particle * ComponentParticleEmmiter::CreateParticle()
{

	//First we get the point were the particle is gonna be instanciated
	LCG random;
	float3 particle_pos = emmit_area_obb.RandomPointInside(random);
	emmit_area_obb.LocalToWorld().TransformPos(particle_pos);

	Particle* new_particle = new Particle(this);

	//We create its transform
	new_particle->components.particle_transform = new ComponentTransform(nullptr, true);
	new_particle->components.particle_transform->SetPosition(particle_pos);

	//We generate the always squared surface for the particle 
	new_particle->components.particle_mesh = App->resources->GetMesh("PrimitiveParticlePlane");

	//Billboard the squad for always be looking at the camera, at the beggining it will be deactivated 
	if (data->billboarding && data->billboard_type != BILLBOARD_NONE)
	{
		new_particle->SetBillboarding(true);
		new_particle->components.billboard = new ComponentBillboard(new_particle);
		new_particle->components.billboard->SetBillboardType(data->billboard_type);
	}
	else
	{
		new_particle->SetBillboarding(false);
	}
		
	//Copy Stats
	new_particle->SetMaxLifetime(data->max_lifetime);
	new_particle->SetVelocity(data->velocity);
	new_particle->SetAngular(data->angular_v);
	new_particle->SetParticleTexture(data->animation_system.GetCurrentTexture());
	new_particle->SetColor(data->color);
	new_particle->SetGravity(data->gravity);
	new_particle->SetEmmisionAngle(data->emision_angle);
	new_particle->SetMovement();

	new_particle->SetWorldSpace(data->relative_pos);
	
	//Copy Interpolations
	///Color
	//new_particle->SetInterpolatingColor(data->change_color_interpolation, root_particle->GetInitialColor(), root_particle->GetFinalColor());

	///Size
	if (data->change_size_interpolation)
	{
		new_particle->SetInterpolationSize(data->change_size_interpolation, data->initial_scale, data->final_scale);
	}
	else
		new_particle->SetInterpolationSize(data->change_size_interpolation, { 1,1,1 }, { 1,1,1 });

	///Rotation
	if (data->change_rotation_interpolation)
		new_particle->SetInterpolationRotation(data->initial_angular_v, data->final_angular_v);
	else
	{
		new_particle->SetInterpolationRotation(0, 0);
		new_particle->SetAngular(data->angular_v);
	}

	//Copy Animation
	new_particle->GetData()->animation_system = data->animation_system;
	new_particle->GetData()->animation_system.Start();

	return new_particle;
}

ComponentParticleEmmiter::ComponentParticleEmmiter(GameObject* parent)
{
	//Component properties
	SetGameObject(parent);
	SetActive(true); 
	SetType(Component::CompParticleSystem); 

	//Filling data with Default Emmiter
	data = App->resources->GetParticleTemplate("Default"); 

	//Emmiter properties -------
	emmision_frequency = 1000;
	system_state = PARTICLE_STATE_PAUSE;

	//Make the aabb enclose a primitive cube
	AABB emit_area; 
	emit_area.minPoint = { -0.5f,-0.5f,-0.5f };
	emit_area.maxPoint = { 0.5f,0.5f,0.5f };
	emit_area.Scale({ 0,0,0 }, { 1,1,1 });

	emmit_area_obb.SetFrom(emit_area); 

	//Automatic turnoff
	automatic_turnoff = false; 
	seconds_to_turn_off = 0; 


	//Add the emmiter to the scene list
	App->scene->scene_emmiters.push_back(this);
}

bool ComponentParticleEmmiter::Start()
{
	spawn_timer.Start();
	global_timer.Start();

	return true;
}

int ComponentParticleEmmiter::GetParticlesNum()
{
	return active_particles.size();
}

void ComponentParticleEmmiter::DeleteLastParticle()
{
	multimap<float, Particle*>::iterator it = active_particles.end();

	active_particles.erase(it); 
}

bool ComponentParticleEmmiter::Update()
{
	if (active_particles.empty() == false)
	{
		for (multimap<float, Particle*>::iterator it = active_particles.begin(); it != active_particles.end();)
		{
			//Update
			(*it).second->Update();

			//Delete ----
			if ((*it).second->CheckIfDelete())
			{
				(*it).second->DeleteNow();
				
				it = active_particles.erase(it);

				if (active_particles.empty())
				{
					break;
				}
			}
			else
				it++; 

			//Automatic turnoff
			if (automatic_turnoff)
			{
				if (global_timer.Read() > seconds_to_turn_off * 1000)
				{
					system_state = PARTICLE_STATE_PAUSE; 
					automatic_turnoff = false; 
				}
			}

			// Shock Wave

				
				
		}
	}

	GenerateParticles();

	if (data->shock_wave.active && data->shock_wave.done == false && system_state == PARTICLE_STATE_PLAY)
	{
		UpdateShockWave();
		DrawShockWave(App->renderer3D->editor_camera);
	}

	return true;
}

void ComponentParticleEmmiter::DrawParticles(ComponentCamera* active_camera)
{
	if (active_particles.empty() == false)
	{
		for (multimap<float, Particle*>::reverse_iterator it = active_particles.rbegin(); it != active_particles.rend(); it++)
		{
			(*it).second->Draw(active_camera);
		}
	}
}

void ComponentParticleEmmiter::AddaptEmmitAreaAABB()
{
	ComponentTransform* parent_transform = (ComponentTransform*) GetGameObject()->GetComponent(CompTransform);

	//Position increment
	float3 pos_increment = parent_transform->GetGlobalPosition() - emmit_area_obb.CenterPoint();

	//Rotation increment
	float3 parent_eule_rot = parent_transform->GetMatrix().RotatePart().ToEulerXYZ();
	float3 obb_rot = emmit_area_obb.LocalToWorld().RotatePart().ToEulerXYZ();

	float3 inc_angle = parent_eule_rot - obb_rot; 

	//Apply
	float4x4 rot_mat = float4x4::FromEulerXYZ(inc_angle.x, inc_angle.y, inc_angle.z);

	float4x4 transform_to_apply = float4x4::FromTRS(pos_increment, rot_mat, { data->width_increment + 1,data->height_increment + 1,data->depth_increment + 1 });

	emmit_area_obb.Transform(transform_to_apply);
}

void ComponentParticleEmmiter::UpdateCurrentData()
{
	//SetEmmisionRate(data->emmision_rate);

	////Stats
	//data->emision_angle 
	//root_particle->SetMaxLifetime(data->max_lifetime);
	//root_particle->SetVelocity(data->velocity);
	//root_particle->SetAngular(data->angular_v);
	//root_particle->SetColor(data->color);
	//root_particle->SetGravity(data->gravity);
	//root_particle->SetEmmisionAngle(data->emision_angle);

	////Interpolations
	//if (data->change_color_interpolation) root_particle->SetInterpolatingColor(data->change_color_interpolation, data->initial_color, data->final_color);
	//if (data->change_size_interpolation) root_particle->SetInterpolationSize(data->change_size_interpolation, data->initial_scale, data->final_scale);
	//if (data->change_rotation_interpolation) root_particle->SetInterpolationRotation(data->initial_angular_v, data->final_angular_v);

	////Billboard & relative pos
	//root_particle->SetBillboarding(data->billboarding);
	//root_particle->SetWorldSpace(data->relative_pos);

}

ComponentParticleEmmiter::~ComponentParticleEmmiter()
{
}

void ComponentParticleEmmiter::Save(Data & _data) const
{
	_data.AddInt("Type", GetType());
	_data.AddBool("Active", IsActive());
	_data.AddUInt("UUID", GetUID());
	_data.CreateSection("Particle");

	// Emmit area -----
	_data.AddBool("Show_Emit_Area", show_emit_area);

	_data.AddFloat("Emit_Width", data->emmit_width); 
	_data.AddFloat("Emit_Height", data->emmit_height);
	_data.AddFloat("Emit_Depth", data->emmit_depth);

	// -----

	// Textures ----

	if(data->animation_system.GetNumFrames() == 0)
		_data.AddBool("Has_Texture", false);
	else
	{
		_data.AddBool("Has_Texture", true);

		int frame_num = 1; 
		for (vector<Texture*>::iterator it = data->animation_system.frames_stack.begin(); it != data->animation_system.frames_stack.end(); it++)
		{
			string tex_name("Frame_");
			tex_name += to_string(frame_num); 
			frame_num++; 

			_data.AddInt(tex_name, (*it)->GetUID());
		}
	}

	//------

	// Colors -----

	// ------

	// Motion -----

	_data.AddBool("Relative_Pos", data->relative_pos);
	_data.AddInt("Emmision_Rate", data->emmision_rate);
	_data.AddFloat("Lifetime", data->max_lifetime);
	_data.AddFloat("Initial_Velocity", data->velocity);
	_data.AddVector3("Gravity", data->gravity);
	_data.AddFloat("Angular_Velocity", data->angular_v);
	_data.AddFloat("Emmision_Angle", data->emision_angle);

	// ------

	// Interpolation -----

	if (data->change_color_interpolation)
	{
		_data.AddBool("Color_Interpolation", true);
	}
	else
		_data.AddBool("Color_Interpolation", false);
	
	if (data->change_size_interpolation)
	{
		_data.AddBool("Size_Interpolation", true);

		_data.AddVector3("Initial_Size", data->initial_scale);
		_data.AddVector3("Final_Size", data->final_scale);
	}
	else
		_data.AddBool("Size_Interpolation", false);

	if (data->change_rotation_interpolation)
	{
		_data.AddBool("Rotation_Interpolation", true);

		_data.AddFloat("Initial_Rotation", data->initial_angular_v);
		_data.AddFloat("Final_Rotation", data->final_angular_v);
	}
	else
		_data.AddBool("Rotation_Interpolation", false);

	// ------

	_data.CloseSection();
}

void ComponentParticleEmmiter::Load(Data & _data)
{
	SetType((Component::ComponentType)_data.GetInt("Type"));
	SetActive(_data.GetBool("Active"));
	SetUID(_data.GetUInt("UUID"));
	_data.EnterSection("Particle");

	
}

void ComponentParticleEmmiter::SaveSystemToBinary()
{
	
	
}

void ComponentParticleEmmiter::ReoderParticles(ComponentCamera * rendering_camera)
{
	//We get the position of the camera
	ComponentTransform* camera_transform = (ComponentTransform*)rendering_camera->GetGameObject()->GetComponent(CompTransform); 
	float3 camera_position = camera_transform->GetGlobalPosition(); 

	//Now we iterate and set a distance

}

int ComponentParticleEmmiter::GetEmmisionRate() const
{
	return data->emmision_rate;
}

void ComponentParticleEmmiter::SetEmmisionRate(float new_emision_rate)
{
	data->emmision_rate = new_emision_rate;
	SetFrequencyFromRate(data->emmision_rate);
}

void ComponentParticleEmmiter::SetFrequencyFromRate(int rate)
{

	if (rate != 0)
		emmision_frequency = (1000 / rate);

}

bool ComponentParticleEmmiter::ShowEmmisionArea() const
{
	return show_emit_area;
}

void ComponentParticleEmmiter::SetShowEmmisionArea(bool show)
{
	show_emit_area = show;
}


particle_system_state ComponentParticleEmmiter::GetSystemState() const
{
	return system_state;
}

void ComponentParticleEmmiter::SetSystemState(particle_system_state new_state)
{
	system_state = new_state;
}

void Particle::ApplyWorldSpace()
{
	ComponentTransform* emmiter_transform = (ComponentTransform*)emmiter->GetGameObject()->GetComponent(Component::CompTransform);
	
	float4x4 emmiter_rot = emmiter_transform->GetMatrix(); 
	float4x4 particle_rot = components.particle_transform->GetMatrix(); 

	particle_rot = emmiter_rot * particle_rot; 
	components.particle_transform->SetMatrix(particle_rot);
}


Particle * ComponentParticleEmmiter::GetRootParticle() const
{
	return nullptr;
}

void ComponentParticleEmmiter::SetTurnOffLimit()
{
	if (seconds_to_turn_off == 0)
	{
		automatic_turnoff = false; 
		return; 
	}
	else
	{
		automatic_turnoff = true; 
	}
}

void ComponentParticleEmmiter::CreateShockWave(Texture* texture, float duration, float final_scale)
{
	data->shock_wave.active = true; 
	data->shock_wave.wave_mesh = App->resources->GetMesh("PrimitivePlane"); 
	data->shock_wave.wave_transform = new ComponentTransform(nullptr, true); 
	data->shock_wave.wave_texture = texture; 
	data->shock_wave.duration = duration; 
	data->shock_wave.final_scale = final_scale; 
	data->shock_wave.wave_timer.Start(); 
}

void ComponentParticleEmmiter::UpdateShockWave()
{
	//We get the number that we have to increment 
	float time_ex = data->shock_wave.wave_timer.Read() / 1000;
	float time_dec = data->shock_wave.wave_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (data->shock_wave.duration));

	//We get the current scale 
	float current_scale = data->shock_wave.final_scale * percentage; 

	data->shock_wave.wave_transform->SetScale({ 1, current_scale, current_scale });
	data->shock_wave.wave_transform->SetScale({ 1, 1,1 });

	if (data->shock_wave.wave_timer.Read() > data->shock_wave.duration)
	{
		data->shock_wave.active = false;
		data->shock_wave.done = true; 
	}
		
}

void ComponentParticleEmmiter::DrawShockWave(ComponentCamera* active_camera)
{
	//Activate shader program
	uint id = App->resources->GetShaderProgram("default_shader_program")->GetProgramID();
	App->renderer3D->UseShaderProgram(id);

	App->renderer3D->SetUniformMatrix(id, "Model", data->shock_wave.wave_transform->GetMatrix().Transposed().ptr());
	App->renderer3D->SetUniformMatrix(id, "view", active_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(id, "projection", active_camera->GetProjectionMatrix());

	if (data->shock_wave.wave_texture != nullptr)
	{	
		if (data->shock_wave.wave_texture->GetID() == 0)
			data->shock_wave.wave_texture->LoadToMemory();

		App->renderer3D->SetUniformBool(id, "has_texture", true);
		App->renderer3D->SetUniformBool(id, "has_material_color", false);

		App->renderer3D->SetUniformVector4(id, "material_color", float4(1.0f, 0.1f, 0.0f, 1.0f));

		glBindTexture(GL_TEXTURE_2D, data->shock_wave.wave_texture->GetID());
	}

	if (data->shock_wave.wave_mesh->id_indices == 0)
		data->shock_wave.wave_mesh->LoadToMemory();

	App->renderer3D->BindVertexArrayObject(data->shock_wave.wave_mesh->id_vao);

	glDrawElements(GL_TRIANGLES, data->shock_wave.wave_mesh->num_indices, GL_UNSIGNED_INT, NULL);

	App->renderer3D->UnbindVertexArrayObject();
}






