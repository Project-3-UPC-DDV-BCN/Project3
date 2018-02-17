#include "Particles.h"
#include "Component.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "ComponentParticleEmmiter.h"
#include "ComponentBillboard.h"
#include "ModuleTime.h"
#include "ModuleResources.h"
#include "imgui/CustomImGui.h"
#include "OpenGL.h"
#include "Mesh.h"

Particle::Particle(ComponentParticleEmmiter * parent)
{
	//Interpolations
	///Size
	interpolate_size = false;

	///Color
	interpolate_colors = false;
	particle_color = initial_particle_color;

	///Rotation
	interpolate_rotation = false;
	curr_rot = 0;

	//Stats
	particle_angular_v = 0;
	
	particle_gravity = { 0,0,0 }; 
	emmiter = parent;
	emmision_angle = 0; 
	
	//Timers
	particle_timer.Start();
	animation_timer.Start();
	interpolation_timer.Start();
	twister.Start();
}


void Particle::SetWorldSpace(bool is_space)
{
	is_relative = is_space;
}

bool Particle::IsWorldSpace()
{
	return is_relative;
}

ParticleComponents Particle::GetAtributes()
{
	return components;
}

void Particle::SetMaxLifetime(const float& new_lifetime)
{
	max_particle_lifetime = new_lifetime;
}

float Particle::GetMaxLifeTime() const
{
	return max_particle_lifetime;
}

void Particle::SetParticleTexture(Texture* new_texture)
{
	components.texture = new_texture; 
}

void Particle::SetGravity(float3 grav)
{
	particle_gravity = grav;
}

void Particle::SetVelocity(const float & new_velocity)
{
	particle_velocity = new_velocity;
}

float Particle::GetVelocity() const
{
	return particle_velocity;
}

void Particle::SetAngular(const float & new_velocity)
{
	particle_angular_v = new_velocity;
}

float Particle::GetAngular() const
{
	return particle_angular_v;
}

void Particle::ApplyAngularVelocity()
{
	float rads_to_spin = particle_angular_v * (2 * pi) / 360; 
	GetAtributes().particle_transform->SetRotation({ GetAtributes().particle_transform->GetGlobalRotation().x, GetAtributes().particle_transform->GetGlobalRotation().y, GetAtributes().particle_transform->GetGlobalRotation().z + rads_to_spin });
}

void Particle::SetColor(Color new_color)
{
	particle_color = new_color;
}

Color Particle::GetColor() const
{
	return particle_color;
}

Color Particle::GetInitialColor() const
{
	return initial_particle_color;
}

Color Particle::GetFinalColor() const
{
	return final_particle_color;
}

void Particle::SetInitialColor(Color color)
{
	initial_particle_color = color;
}

void Particle::SetFinalColor(Color color)
{
	final_particle_color = color;
}

float Particle::GetEmmisionAngle()
{
	return emmision_angle;
}

void Particle::SetEmmisionAngle(float new_angle)
{
	emmision_angle = new_angle; 
}

float3 Particle::GetEmmisionVector()
{
	float3 direction;
	ComponentTransform* emmiter_transform = (ComponentTransform*)emmiter->GetGameObject()->GetComponent(Component::CompTransform);

	//First we create a vector matching the Y axis
	direction = emmiter_transform->GetMatrix().WorldY();

	//We apply the rotation angle to the vector 
	LCG random_angle_z;
	int angle_z = random_angle_z.Int(0, emmision_angle);

	float3x3 z_rotation;
	z_rotation = z_rotation.FromEulerXYZ(0, 0, angle_z * DEGTORAD);
	direction = z_rotation.Transform(direction);

	//We apply a rotation on X for randomization
	LCG random_angle_y;
	int angle_y = random_angle_y.Int(0, 360);

	float3x3 y_rotation;
	y_rotation = y_rotation.FromEulerXYZ(0, angle_y * DEGTORAD, 0);
	direction = y_rotation.Transform(direction);

	//Dir is the maximum angle direction so we hace to add some randomnes 
	LCG random_percentage;
	int amount = random_percentage.Int(0, 100);
	amount /= 10; 

	float from_point_to_origin = direction.x;
	from_point_to_origin = from_point_to_origin - (from_point_to_origin*amount);

	return direction;
}


float Particle::GetDistanceToCamera()
{
	//We get the current camera
	float3 camera_position = App->renderer3D->editor_camera->camera_frustum.pos;

	//Compute the distance & return
	float distance = (camera_position - GetAtributes().particle_transform->GetGlobalPosition()).Length();
	return distance;
}

void Particle::SetBillboarding(float new_dist)
{
	billboarding = new_dist;
}

float Particle::IsBillboarding() const
{
	return billboarding;
}

void Particle::UpdateColor()
{
	if (!interpolate_colors)
		return;

	//We get the number that we have to increment 
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (max_particle_lifetime));

	//Setting the increment in each component
	float increment_r = color_difference[0] * percentage;
	float increment_g = color_difference[1] * percentage;
	float increment_b = color_difference[2] * percentage;
	float increment_a = color_difference[3] * percentage;

	//Applying the increment
	particle_color.r = (initial_particle_color.r + increment_r) / 255;
	particle_color.g = (initial_particle_color.g + increment_g) / 255;
	particle_color.b = (initial_particle_color.b + increment_b) / 255;
	particle_color.a = (initial_particle_color.a + increment_a) / 255;

}

bool Particle::IsInterpolatingColor() const
{
	return interpolate_colors;
}

void Particle::SetInterpolatingColor(bool interpolate, Color initial_color, Color final_color)
{
	interpolate_colors = interpolate;

	if (!interpolate)
		return;

	initial_particle_color = initial_color;
	final_particle_color = final_color;

	color_difference[0] = (final_particle_color.r - initial_particle_color.r);
	color_difference[1] = (final_particle_color.g - initial_particle_color.g);
	color_difference[2] = (final_particle_color.b - initial_particle_color.b);
	color_difference[3] = (final_particle_color.a - initial_particle_color.a);
}

void Particle::UpdateSize()
{
	if (!interpolate_size)
		return;

	//We get the number that we have to increment 
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (max_particle_lifetime));
	float3 total_increment((final_particle_size.x - initial_particle_size.x), (final_particle_size.y - initial_particle_size.y), 1);

	//Get the increment in each component
	float increment_x = total_increment.x * percentage;
	float increment_y = total_increment.y * percentage;
	float increment_z = 1;

	//Apply the increment
	float3 new_scale = { initial_particle_size.x + increment_x, 0,  initial_particle_size.x + increment_x };
	GetAtributes().particle_transform->SetScale(new_scale);

}

void Particle::SetInterpolationSize(bool interpolate, float3 initial_scale, float3 final_scale)
{
	interpolate_size = interpolate;

	initial_particle_size = initial_scale;
	final_particle_size = final_scale;
}

void Particle::UpdateRotation()
{
	//Get the percentatge that has passed
	float time_ex = interpolation_timer.Read() / 1000;
	float time_dec = interpolation_timer.Read() % 1000;
	float time = time_ex + time_dec / 1000;

	float percentage = (time / (max_particle_lifetime));

	//Get the increment
	float velocity_increment = final_particle_angular_v - initial_particle_angular_v; 
	float rotation_increment = velocity_increment * percentage;

	float3 curr_rot = GetAtributes().particle_transform->GetGlobalRotation(); 

	//Apply the increment to the particle
	if (curr_rot.x > 360)
		curr_rot.x = 0;

	GetAtributes().particle_transform->SetRotation({ curr_rot.x, curr_rot.y, curr_rot.z + rotation_increment });

}

void Particle::SetInterpolationRotation(float initial_v, float final_v)
{
	interpolate_rotation = true;

	initial_particle_angular_v = initial_v; 
	final_particle_angular_v = final_v; 
}

void Particle::SetMovementFromStats()
{
	movement += particle_gravity * App->time->GetGameDt();

}
void Particle::SetMovement()
{
	//Getting the angle in which the particle have to be launched
	if (GetEmmisionAngle() == 0)
		movement = emmiter->emmit_area_obb.axis[1] * particle_velocity;
	else	
		movement = GetEmmisionVector(); 
	
	//Setting the movement vector
	movement.Normalize(); 
	movement *= particle_velocity;
	movement *= App->time->GetGameDt();
}

void Particle::Update()
{
	//Translate the particles in the necessary direction
	SetMovementFromStats(); 
	GetAtributes().particle_transform->SetPosition(GetAtributes().particle_transform->GetLocalPosition() + movement);

	if (IsWorldSpace())
		ApplyWorldSpace(); 

	//Update the particle color in case of interpolation
	if (interpolate_colors)
		UpdateColor();

	//Update scale
	if (interpolate_size)
		UpdateSize();

	//Update rotation
	if (interpolate_rotation)
		UpdateRotation(); 

	//Apply angular velocity
	ApplyAngularVelocity();

	//Animations
	if(GetAnimationController()->GetNumFrames() != 0)
		components.texture = GetAnimationController()->GetCurrentTexture(); 
	
}

void Particle::DeleteNow()
{
	//GetAtributes().particle_billboarding->Delete();
	GetAtributes().SetToNull();
}

bool Particle::CheckIfDelete()
{
	bool ret = false; 
	
	if (particle_timer.Read() > max_particle_lifetime * 1000)
		ret = true; 

	return ret; 
}

void Particle::Draw(ComponentCamera* active_camera)
{
	//Billboard to the active camera 
	if (billboarding)
	{		
		GetAtributes().billboard->RotateObject();		
	}
		
	//Activate shader program
	uint id = App->resources->GetShaderProgram("default_shader_program")->GetProgramID();
	App->renderer3D->UseShaderProgram(id); 

	App->renderer3D->SetUniformMatrix(id, "Model", GetAtributes().particle_transform->GetMatrix().Transposed().ptr());
	App->renderer3D->SetUniformMatrix(id, "view", active_camera->GetViewMatrix());
	App->renderer3D->SetUniformMatrix(id, "projection", active_camera->GetProjectionMatrix());

	if (GetAtributes().texture == nullptr)
	{
		App->renderer3D->SetUniformBool(id, "has_texture", false);
		App->renderer3D->SetUniformBool(id, "has_material_color", true);

		App->renderer3D->SetUniformVector4(id, "material_color", float4(1.0f, 0.1f, 0.0f, 1.0f));
	}
	else
	{
		if (GetAtributes().texture->GetID() == 0)
			GetAtributes().texture->LoadToMemory();

		App->renderer3D->SetUniformBool(id, "has_texture", true);
		App->renderer3D->SetUniformBool(id, "has_material_color", false);

		//glEnable(GL_BLEND); 
		//glEnable(GL_ALPHA_TEST);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		App->renderer3D->SetUniformVector4(id, "material_color", float4(1.0f, 0.1f, 0.0f, 1.0f));

		glBindTexture(GL_TEXTURE_2D, GetAtributes().texture->GetID());
	}

	if (GetAtributes().particle_mesh->id_indices == 0) GetAtributes().particle_mesh->LoadToMemory(); 

	App->renderer3D->BindVertexArrayObject(GetAtributes().particle_mesh->id_vao); 
 
	glDrawElements(GL_TRIANGLES, GetAtributes().particle_mesh->num_indices, GL_UNSIGNED_INT, NULL);
		
	App->renderer3D->UnbindVertexArrayObject(); 
}

ParticleAnimation * Particle::GetAnimationController()
{
	return &components.particle_animation;
}

Particle::~Particle()
{
}

//Animation Controller -----------

ParticleAnimation::ParticleAnimation()
{
	name = "";
	timeStep = 0;
	rendering_frame = 0;
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

ParticleAnimation::~ParticleAnimation()
{

}

void ParticleAnimation::Start()
{
	switcher_timer.Start(); 
}

// -------------------------------