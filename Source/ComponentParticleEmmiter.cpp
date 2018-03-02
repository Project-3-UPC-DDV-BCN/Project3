#include "ComponentParticleEmmiter.h"
#include "ComponentMeshRenderer.h"
#include "ComponentBillboard.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Shader.h"
#include "ComponentTransform.h"
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
	float3 particle_pos = emmit_area.RandomPointInside(random);
//	emmit_area.LocalToWorld().TransformPos(emmit_area_obb.CenterPoint());

	Particle* new_particle = new Particle(this);

	//We create its transform
	new_particle->components.particle_transform = new ComponentTransform(nullptr, true);

	if(data->emmit_style == EMMIT_FROM_CENTER)
		new_particle->components.particle_transform->SetPosition(emmit_area.CenterPoint());

	else if (data->emmit_style == EMMIT_FROM_RANDOM)
		new_particle->components.particle_transform->SetPosition(particle_pos);

	new_particle->components.particle_transform->SetScale({ data->global_scale,  data->global_scale , 0});

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
	if (data->change_color_interpolation)
	{
		new_particle->particle_data->initial_color = data->initial_color; 
		new_particle->particle_data->final_color = data->final_color;

		//CONSOLE_LOG("Init: %f %f %f", data->initial_color.r, data->initial_color.g, data->initial_color.b);
		//CONSOLE_LOG("Final: %f %f %f", data->final_color.r, data->final_color.g, data->final_color.b);
	}
	else
		new_particle->particle_data->change_color_interpolation = false;
	
	///Size
	if (data->change_size_interpolation)
	{
		new_particle->particle_data->initial_scale = data->initial_scale;
		new_particle->particle_data->final_scale = data->final_scale; 
	}
	else
		new_particle->particle_data->change_size_interpolation = false;

	///Rotation
	if (data->change_rotation_interpolation)
	{
		new_particle->particle_data->initial_angular_v = data->initial_angular_v;
		new_particle->particle_data->final_angular_v = data->final_angular_v;
	}
	else
		new_particle->particle_data->change_rotation_interpolation = false;

	///Alpha 
	if (data->change_alpha_interpolation)
	{
		new_particle->particle_data->init_alpha_interpolation_time = data->init_alpha_interpolation_time; 
		new_particle->particle_data->alpha_interpolation_delayed = data->alpha_interpolation_delayed; 
	}
	else
		new_particle->particle_data->change_alpha_interpolation = false;
		
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
	runtime_behaviour = "null"; 

	show_shockwave = false; 
	wave_launched = false; 
	show_emit_area = true; 

	//Make the aabb enclose a primitive cube
	emmit_area.minPoint = { -0.5f,-0.5f,-0.5f };
	emmit_area.maxPoint = { 0.5f,0.5f,0.5f };
	emmit_area.Scale({ 0,0,0 }, { 1,1,1 });



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
	if (data == nullptr)
		return false;

	if (data->emmision_type == EMMISION_CONTINUOUS && system_state == PARTICLE_STATE_PLAY)
		GenerateParticles();

	else if (data->emmision_type == EMMISION_SIMULTANEOUS && system_state == PARTICLE_STATE_PLAY)
	{
		if (data->time_step_sim == 0 && wave_launched == false)
		{
			LaunchParticlesWave(); 
			wave_launched = true; 
		}
		else if(data->time_step_sim != 0)
		{
			if (LaunchingAllowed())
			{
				LaunchParticlesWave();
				spawn_timer.Start(); 
			}
				
		}
	}

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
			if (data->autopause)
			{
				if (global_timer.Read() > data->time_to_stop * 1000)
				{
					system_state = PARTICLE_STATE_PAUSE; 
				}
			}
				
		}
	}

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

	if (parent_transform->dirty)
	{

		//Position increment
		float3 pos_increment = parent_transform->GetGlobalPosition() - emmit_area.CenterPoint();

		float4x4 transform_to_apply = float4x4::FromTRS(pos_increment, Quat::identity, {data->width_increment + 1, data->height_increment + 1, data->depth_increment + 1});

		CONSOLE_LOG("%f %f %f", data->width_increment, data->height_increment, data->depth_increment);

		emmit_area.TransformAsAABB(transform_to_apply);

		parent_transform->dirty = false; 
	}

}

void ComponentParticleEmmiter::UpdateCurrentData()
{
	
}

ComponentParticleEmmiter::~ComponentParticleEmmiter()
{
}

void ComponentParticleEmmiter::Save(Data & data) const
{
	data.AddInt("Type", (int)GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	//Save Transform 
	ComponentTransform* go_transform = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform); 
	
	data.AddVector3("Position", go_transform->GetGlobalPosition()); 
	data.AddVector3("Rotation", go_transform->GetGlobalRotation());
	data.AddString("Template", this->data->GetName()); 
}

void ComponentParticleEmmiter::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));

	uint particle_uid = data.GetUInt("UUID");

	float3 pos = data.GetVector3("Position"); 
	float3 rot = data.GetVector3("Rotation");
	float3 scale = { 1,1,1 }; 

	//Load Template 
	this->data = App->resources->GetParticleTemplate(data.GetString("Template")); 
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

void Particle::ApplyRelativePos()
{
	float3 particle_desplacament = emmiter_transform->GetGlobalPosition() - prev_emmiter_pos;
	components.particle_transform->SetPosition(components.particle_transform->GetGlobalPosition() + particle_desplacament);
}


Particle * ComponentParticleEmmiter::GetRootParticle() const
{
	return nullptr;
}



void ComponentParticleEmmiter::CreateShockWave(Texture* texture, float duration, float final_scale)
{
	data->shock_wave.object = new GameObject(nullptr); 

	data->shock_wave.active = true; 
	data->shock_wave.done = false; 

	data->shock_wave.wave_mesh = App->resources->GetMesh("PrimitivePlane");
	data->shock_wave.wave_transform = new ComponentTransform(data->shock_wave.object);
	data->shock_wave.wave_texture = texture; 

	data->shock_wave.duration = duration;
	data->shock_wave.final_scale = final_scale;
	data->shock_wave.wave_timer.Start(); 
}

void ComponentParticleEmmiter::UpdateShockWave()
{
	////We get the number that we have to increment 
	//float time_ex = data->shock_wave.wave_timer.Read() / 1000;
	//float time_dec = data->shock_wave.wave_timer.Read() % 1000;
	//float time = time_ex + time_dec / 1000;

	//float percentage = (time / (data->shock_wave.duration));

	////We get the current scale 
	//float current_scale = data->shock_wave.final_scale * percentage; 

	//data->shock_wave.wave_transform->SetScale({ 1, current_scale, current_scale });

	//if (data->shock_wave.wave_timer.Read() > data->shock_wave.duration * 1000)
	//{
	//	data->shock_wave.active = false;
	//	data->shock_wave.done = true; 
	//	show_shockwave = false; 
	//}
		
}

void ComponentParticleEmmiter::PlayEmmiter()
{
	SetSystemState(PARTICLE_STATE_PLAY);

	if (show_shockwave)
		CreateShockWave(data->shock_wave.wave_texture, data->shock_wave.duration, data->shock_wave.final_scale);

	Start();
}

void ComponentParticleEmmiter::StopEmmiter()
{
	SetSystemState(PARTICLE_STATE_PAUSE);
	wave_launched = false; 
}

bool ComponentParticleEmmiter::LaunchingAllowed()
{
	if (spawn_timer.Read() > data->time_step_sim * 1000)
		return true;  

	return false;
}

void ComponentParticleEmmiter::LaunchParticlesWave()
{
	for (int i = 0; i < data->amount_to_emmit; i++)
	{
		Particle* new_particle = CreateParticle();
		new_particle->components.particle_transform->SetPosition(emmit_area.CenterPoint());
		active_particles.insert(pair<float, Particle* >(new_particle->GetDistanceToCamera(), new_particle));
	}
}

void ComponentParticleEmmiter::DrawShockWave(ComponentCamera* active_camera)
{
	//uint id = App->resources->GetShaderProgram("default_shader_program")->GetProgramID();
	//App->renderer3D->UseShaderProgram(id);

	//App->renderer3D->SetUniformMatrix(id, "Model", data->shock_wave.wave_transform->GetMatrix().Transposed().ptr());
	//App->renderer3D->SetUniformMatrix(id, "view", active_camera->GetViewMatrix());
	//App->renderer3D->SetUniformMatrix(id, "projection", active_camera->GetProjectionMatrix());

	//if (data->shock_wave.wave_texture != nullptr)
	//{	
	//	if (data->shock_wave.wave_texture->GetID() == 0)
	//		data->shock_wave.wave_texture->LoadToMemory();

	//	App->renderer3D->SetUniformBool(id, "has_texture", true);
	//	App->renderer3D->SetUniformBool(id, "has_material_color", false);

	//	App->renderer3D->SetUniformVector4(id, "material_color", float4(1.0f, 0.1f, 0.0f, 1.0f));

	//	glBindTexture(GL_TEXTURE_2D, data->shock_wave.wave_texture->GetID());
	//}

	//if (data->shock_wave.wave_mesh->id_indices == 0)
	//	data->shock_wave.wave_mesh->LoadToMemory();

	//App->renderer3D->BindVertexArrayObject(data->shock_wave.wave_mesh->id_vao);

	//glDrawElements(GL_TRIANGLES, data->shock_wave.wave_mesh->num_indices, GL_UNSIGNED_INT, NULL);

	//App->renderer3D->UnbindVertexArrayObject();
}






