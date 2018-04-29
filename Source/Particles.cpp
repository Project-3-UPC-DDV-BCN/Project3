#include "Particles.h"
#include "Component.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "ParticleData.h"
#include "ComponentParticleEmmiter.h"
#include "ComponentBillboard.h"
#include "ModuleTime.h"
#include "ModuleResources.h"
#include "imgui/CustomImGui.h"
#include "OpenGL.h"
#include "Mesh.h"

Particle::Particle(ComponentParticleEmmiter * parent)
{
	//General
	emmiter = parent;
	runtime_behaviour = RUNTIME_NONE; 
	particle_data = new ParticleData(); 

	emmiter_transform = (ComponentTransform*)emmiter->GetGameObject()->GetComponent(Component::CompTransform); 
	prev_emmiter_pos = emmiter_transform->GetGlobalPosition();
	
	//Timers
	particle_timer.Start();
	animation_timer.Start();
	interpolation_timer.Start();
	twister.Start();
}


void Particle::SetWorldSpace(bool is_space)
{
	particle_data->relative_pos = is_space;
}

bool Particle::IsWorldSpace()
{
	return particle_data->relative_pos;
}

ParticleComponents Particle::GetAtributes()
{
	return components;
}

ParticleData * Particle::GetData()
{
	return particle_data;
}


float3 Particle::GetPrevEmmiterPos()
{
	return prev_emmiter_pos;
}

void Particle::SetPrevEmmiterPos(float3 _spawn_pos)
{
	prev_emmiter_pos = _spawn_pos; 
}

void Particle::SetRuntimeBehaviour(const char * string)
{
	if (string == "Auto")
		runtime_behaviour = RUNTIME_ALWAYS_EMMIT;

	else if (string == "Auto")
		runtime_behaviour = RUNTIME_MANUAL;

	else
		runtime_behaviour = RUNTIME_NONE; 
}

void Particle::SetMaxLifetime(const float& new_lifetime)
{
	particle_data->max_lifetime = new_lifetime;
}

float Particle::GetMaxLifeTime() const
{
	return particle_data->max_lifetime;
}

void Particle::SetParticleTexture(Texture* new_texture)
{
	components.texture = new_texture; 
}

void Particle::SetGravity(float3 grav)
{
	particle_data->gravity = grav;
}

void Particle::SetVelocity(const float & new_velocity)
{
	particle_data->velocity = new_velocity;
}

float Particle::GetVelocity() const
{
	return particle_data->velocity;
}

void Particle::SetAngular(const float & new_velocity)
{
	particle_data->angular_v = new_velocity;
}

float Particle::GetAngular() const
{
	return particle_data->angular_v;
}

float Particle::GetAlphaInterpolationPercentage()
{
	float alpha_percentage = 0.0f; 

	alpha_started = false;

	if (particle_data->init_alpha_interpolation_time == 0)
		alpha_percentage = (particle_timer.Read() / (particle_data->max_lifetime * 1000)); 
	else
	{
		float time_to_interpolate = particle_data->max_lifetime - particle_data->init_alpha_interpolation_time; 

		if (particle_timer.Read() > particle_data->init_alpha_interpolation_time * 1000 && !alpha_started)
		{
			interpolation_timer.Start(); 
			alpha_started = true;
		}

		if (alpha_started)
		{
			alpha_percentage = (interpolation_timer.Read() / (time_to_interpolate * 1000)); 
		}

	}

	return (1.0f - alpha_percentage); 
}

void Particle::ApplyAngularVelocity()
{
	float rads_to_spin = particle_data->angular_v * (2 * pi) / 360;
	GetAtributes().particle_transform->SetRotation({ GetAtributes().particle_transform->GetGlobalRotation().x, GetAtributes().particle_transform->GetGlobalRotation().y, GetAtributes().particle_transform->GetGlobalRotation().z + rads_to_spin });
}

void Particle::SetColor(Color new_color)
{
	particle_data->color = new_color;
}

Color Particle::GetColor() const
{
	return particle_data->color;
}

Color Particle::GetInitialColor() const
{
	return particle_data->initial_color; 
}

Color Particle::GetFinalColor() const
{

	return particle_data->final_color;
}

void Particle::SetInitialColor(Color color)
{	
	
	particle_data->initial_color = color;
}

void Particle::SetFinalColor(Color color)
{
	particle_data->final_color = color;
}

float Particle::GetEmmisionAngle()
{
	return particle_data->emision_angle;
}

void Particle::SetEmmisionAngle(float new_angle)
{
	particle_data->emision_angle = new_angle; 
}

float3 Particle::GetEmmisionVector()
{
	LCG random;

	float3 direction;
	ComponentTransform* emmiter_transform = (ComponentTransform*)emmiter->GetGameObject()->GetComponent(Component::CompTransform);

	//First we create a vector matching the Y axis
	direction = emmiter_transform->GetMatrix().WorldY();
	float3 direction_cpy = direction; 

	//We apply the rotation angle to the vector 
	int angle_z = random.Int(0, particle_data->emision_angle);

	float3x3 z_rotation;
	z_rotation = z_rotation.FromEulerXYZ(0, 0, angle_z * DEGTORAD);
	direction = z_rotation.Transform(direction);

	//We apply a rotation on Y for randomization
	int angle_y = random.Int(0, 360);
	float3x3 y_rotation = float3x3::RotateAxisAngle(direction_cpy, angle_y * DEGTORAD);

	direction = y_rotation.Transform(direction);

	//Dir is the maximum angle direction so we hace to add some randomnes 
	int amount = random.Int(0, 100);
	amount /= 10; 

	float from_point_to_origin = direction.x;
	from_point_to_origin = from_point_to_origin - (from_point_to_origin*amount);

	return direction;
}


float Particle::GetDistanceToCamera()
{
	//We get the current camera
	float3 camera_position = App->renderer3D->editor_camera->camera_frustum.Pos();

	//Compute the distance & return
	float distance = (camera_position - GetAtributes().particle_transform->GetGlobalPosition()).Length();
	return distance;
}

void Particle::SetBillboarding(float new_dist)
{
	particle_data->billboarding = new_dist;
}

float Particle::IsBillboarding() const
{
	return particle_data->billboarding;
}

void Particle::UpdateColor()
{
	if (!particle_data->change_color_interpolation)
		return;

	Color difference(particle_data->final_color.r - particle_data->initial_color.r, particle_data->final_color.g - particle_data->initial_color.g, particle_data->final_color.b - particle_data->initial_color.b, particle_data->final_color.a - particle_data->initial_color.a);

	//We get the number that we have to increment 
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (particle_data->max_lifetime));

	//Setting the increment in each component
	float increment_r = difference.r * percentage;
	float increment_g = difference.g * percentage;
	float increment_b = difference.b * percentage;

	//Applying the increment
	particle_data->color.r = (particle_data->initial_color.r + increment_r);
	particle_data->color.g = (particle_data->initial_color.g + increment_g);
	particle_data->color.b = (particle_data->initial_color.b + increment_b);

}

void Particle::UpdateSize()
{
	if (!particle_data->change_size_interpolation)
		return;

	//We get the number that we have to increment 
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (particle_data->max_lifetime));
	float3 total_increment((particle_data->final_scale.x - particle_data->initial_scale.x), (particle_data->final_scale.y - particle_data->initial_scale.y), 1);

	//Get the increment in each component
	float increment_x = total_increment.x * percentage;
	float increment_y = total_increment.y * percentage;
	float increment_z = 1;

	//Apply the increment
	float3 new_scale = { particle_data->initial_scale.x + increment_x, particle_data->initial_scale.x + increment_x,  1 };
	GetAtributes().particle_transform->SetScale(new_scale);

}

void Particle::UpdateRotation()
{
	//Get the percentatge that has passed
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (particle_data->max_lifetime));

	//Get the increment
	float velocity_increment = particle_data->final_angular_v - particle_data->initial_angular_v; 
	float rotation_increment = velocity_increment * percentage;

	float3 curr_rot = GetAtributes().particle_transform->GetGlobalRotation(); 

	//Apply the increment to the particle
	if (curr_rot.x > 360)
		curr_rot.x = 0;

	GetAtributes().particle_transform->SetRotation({ curr_rot.x, curr_rot.y, curr_rot.z + rotation_increment });

}

void Particle::SetMovementFromStats()
{
	movement += (particle_data->gravity);

}

void Particle::SetMovement()
{
	//Getting the angle in which the particle have to be launched
	ComponentTransform* trans = (ComponentTransform*)emmiter->GetGameObject()->GetComponent(Component::CompTransform); 

	if (GetEmmisionAngle() == 0)
		movement = trans->GetMatrix().WorldY();
	else	
		movement = GetEmmisionVector(); 
	
	//Setting the movement vector
	movement.Normalize(); 
	movement *= particle_data->velocity;
}

void Particle::Update()
{
	if (App->time->time_scale <= 0 && particle_timer.getTimerState())
	{
		particle_timer.Stop();
	}

	//Translate the particles in the necessary direction
	SetMovementFromStats(); 

	float3 increment = movement; 

	if (App->GetEngineState() == Application::EngineState::OnPlay)
		increment *= App->time->GetGameDt();
	else
		increment *= App->GetDt();

	GetAtributes().particle_transform->SetPosition(GetAtributes().particle_transform->GetLocalPosition() + increment);

	if (IsWorldSpace())
	{
		if (prev_emmiter_pos.Equals(emmiter_transform->GetGlobalPosition()) == false)
		{
			ApplyRelativePos();
			prev_emmiter_pos = emmiter_transform->GetGlobalPosition(); 
		}
		
	}

	//Update the particle color in case of interpolation
	if (particle_data->change_color_interpolation)
	{
		UpdateColor();
	}
	

	//Update scale
	if (particle_data->change_size_interpolation)
	{
		UpdateSize();
	}
		

	//Update rotation
	if (particle_data->change_rotation_interpolation)
	{
		UpdateRotation();
	}
		
	//Apply angular velocity
	ApplyAngularVelocity();

	//Animations
	if (particle_data->animation_system.GetNumFrames() != 0)
		components.texture = particle_data->animation_system.GetCurrentTexture();
	
}

void Particle::DeleteNow()
{
	GetAtributes().SetToNull();
}

bool Particle::CheckIfDelete()
{
	bool ret = false; 
	
	if (particle_timer.Read() > particle_data->max_lifetime * 1000)
		ret = true; 

	return ret; 
}

void Particle::Draw(ComponentCamera* active_camera)
{
	//Billboard to the active camera 
	BROFILER_CATEGORY("Particle Draw", Profiler::Color::Thistle);
	if (particle_data->billboarding)
	{		
		GetAtributes().billboard->RotateObject();		
	}
		
	//Activate shader program
	uint id = App->resources->GetShaderProgram("default_particle_program")->GetProgramID();
	App->renderer3D->UseShaderProgram(id); 

	App->renderer3D->SetUniformMatrix(id, "Model", GetAtributes().particle_transform->GetMatrix().Transposed().ptr());
	App->renderer3D->SetUniformMatrix(id, "view", active_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(id, "projection", active_camera->GetProjectionMatrix());

	App->renderer3D->SetUniformVector3(id, "camera_position", App->renderer3D->game_camera->GetFrustum().Pos()); 
	App->renderer3D->SetUniformVector3(id, "particle_position", GetAtributes().particle_transform->GetGlobalPosition());

	if (particle_data->change_alpha_interpolation)
	{
		App->renderer3D->SetUniformBool(id, "alpha_interpolation", true); 
		float percentage = GetAlphaInterpolationPercentage(); 

		App->renderer3D->SetUniformFloat(id, "alpha_percentage", percentage);
	}
	else
	{
		App->renderer3D->SetUniformBool(id, "alpha_interpolation", false);
		App->renderer3D->SetUniformFloat(id, "alpha_percentage", 0);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	if (GetAtributes().texture == nullptr)
	{
		App->renderer3D->SetUniformBool(id, "has_texture", false);
		App->renderer3D->SetUniformBool(id, "has_material_color", true);
		App->renderer3D->SetUniformFloat(id, "material_alpha", particle_data->color.a);

		float4 color = float4(particle_data->color.r, particle_data->color.g, particle_data->color.b, particle_data->color.a);

		

		App->renderer3D->SetUniformVector4(id, "material_color", color);
	}
	else
	{
		if (GetAtributes().texture->GetID() == 0)
			GetAtributes().texture->LoadToMemory();

		App->renderer3D->SetUniformBool(id, "has_texture", true);
		App->renderer3D->SetUniformBool(id, "has_material_color", false);

		float4 color = float4(particle_data->color.r, particle_data->color.g, particle_data->color.b, particle_data->color.a);
		App->renderer3D->SetUniformVector4(id, "material_color", color);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetAtributes().texture->GetID());
	}

	if (GetAtributes().particle_mesh->id_indices == 0) GetAtributes().particle_mesh->LoadToMemory(); 

	App->renderer3D->BindVertexArrayObject(GetAtributes().particle_mesh->id_vao); 
 
	glDrawElements(GL_TRIANGLES, GetAtributes().particle_mesh->num_indices, GL_UNSIGNED_INT, NULL);
		
	App->renderer3D->UnbindVertexArrayObject(); 

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Particle::~Particle()
{
}

//Animation Controller -----------



// -------------------------------