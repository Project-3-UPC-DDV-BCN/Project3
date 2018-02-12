#include "ComponentParticleEmmiter.h"
#include "ComponentMeshRenderer.h"
#include "ComponentBillboard.h"
#include "GameObject.h"
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
	float3 particle_pos = emit_area_obb.RandomPointInside(random);
	emit_area_obb.LocalToWorld().TransformPos(particle_pos); 

	Particle* new_particle = new Particle(this);

	//We create its transform
	new_particle->components.particle_transform = new ComponentTransform(nullptr, true);
	new_particle->components.particle_transform->SetPosition(particle_pos);
	new_particle->components.particle_transform->SetRotation({ -90,0,0 }); 

	//We generate the always squared surface for the particle 
	new_particle->components.particle_mesh = App->resources->GetMesh("PrimitivePlane");

	//Billboard the squad for always be looking at the camera, at the beggining it will be deactivated 
	if (billboarding)
	{
		new_particle->SetBillboarding(true);
		new_particle->components.billboard = new ComponentBillboard(new_particle);
	}
	else
		new_particle->SetBillboarding(false);

	//Copy Stats
	new_particle->SetMaxLifetime(max_lifetime);
	new_particle->SetVelocity(velocity);
	new_particle->SetAngular(angular_v); 
	new_particle->SetParticleTexture(root_particle->components.texture);
	new_particle->SetColor(color);
	new_particle->SetGravity(gravity);
	new_particle->SetEmmisionAngle(root_particle->GetEmmisionAngle());
	new_particle->SetMovement();

	new_particle->SetWorldSpace(relative_pos);
	
	//Copy Interpolations
	new_particle->SetInterpolatingColor(apply_color_interpolation, root_particle->GetInitialColor(), root_particle->GetFinalColor());

	if (apply_size_interpolation)
		new_particle->SetInterpolationSize(apply_size_interpolation, initial_scale, final_scale);
	else
		new_particle->SetInterpolationSize(apply_size_interpolation, { 1,1,1 }, { 1,1,1 });

	//Copy Animation
	new_particle->components.particle_animation = root_particle->components.particle_animation; 
	new_particle->components.texture = new_particle->GetAnimationController()->GetCurrentTexture();
	new_particle->GetAnimationController()->Start(); 

	//if (apply_rotation_interpolation)
	//	new_particle->SetInterpolationRotation(true, initial_angular_v, final_angular_v);
	//else
	//{
	//	new_particle->SetInterpolationRotation(false, 0, 0);
	//	new_particle->SetAngular(angular_v);
	//}

	return new_particle;
}

ComponentParticleEmmiter::ComponentParticleEmmiter(GameObject* parent)
{
	//Component properties
	SetGameObject(parent);
	SetActive(true); 
	SetType(Component::CompParticleSystem); 

	//Emmiter properties -------
	emmision_frequency = 1000;
	emmision_rate = 1;

	particles_lifetime = 0.0f;
	system_state = PARTICLE_STATE_PAUSE;

	shapes_amount = 0;
	emmision_rate = 1;
	max_lifetime = 1;
	velocity = 5.0f;
	curr_texture_id = -1;
	color = Color(255, 255, 255, 0);
	billboarding = false;
	gravity = { 0,0,0 };
	angular_v = 0;
	emision_angle = 0;
	reorder_time.Start();
	is_animated = false;
	time_step = 0.2;

	relative_pos = false; 
	billboarding = false; 

	apply_rotation_interpolation = false;
	apply_size_interpolation = false;
	apply_color_interpolation = false;

	initial_scale = { 1,1,1 };
	final_scale = { 1,1,1 };

	initial_angular_v = 0;
	final_angular_v = 0;

	initial_color[0] = initial_color[1] = initial_color[2] = initial_color[3] = 0;
	final_color[0] = final_color[1] = final_color[2] = final_color[3] = 0;

	// ------

	//Make the aabb enclose a primitive cube
	AABB emit_area; 
	emit_area.minPoint = { -0.5f,-0.5f,-0.5f };
	emit_area.maxPoint = { 0.5f,0.5f,0.5f };
	emit_area.Scale({ 0,0,0 }, { 1,1,1 });

	emit_area_obb.SetFrom(emit_area); 

	CreateRootParticle();

}

bool ComponentParticleEmmiter::Start()
{
	spawn_timer.Start();

	return true;
}

void ComponentParticleEmmiter::CreateRootParticle()
{
	root_particle = new Particle(this);
	root_particle->components.SetToNull();

	root_particle->SetMaxLifetime(max_lifetime);
	root_particle->SetVelocity(velocity);
	root_particle->SetParticleTexture(nullptr);
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
			(*it).second->Update();

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
				
				
		}
	}

	GenerateParticles();

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

void ComponentParticleEmmiter::MoveEmmitArea()
{
	ComponentTransform* parent_transform = (ComponentTransform*) GetGameObject()->GetComponent(CompTransform);

	//Position increment
	float3 pos_increment = parent_transform->GetGlobalPosition() - emit_area_obb.CenterPoint(); 

	//Rotation increment
	float3 parent_eule_rot = parent_transform->GetMatrix().RotatePart().ToEulerXYZ();
	float3 obb_rot = emit_area_obb.LocalToWorld().RotatePart().ToEulerXYZ(); 

	float3 inc_angle = parent_eule_rot - obb_rot; 

	//Apply
	float4x4 rot_mat = float4x4::FromEulerXYZ(inc_angle.x, inc_angle.y, inc_angle.z);
	float4x4 transform_to_apply = float4x4::FromTRS(pos_increment, rot_mat, parent_transform->GetGlobalScale());

	emit_area_obb.Transform(transform_to_apply);	
}

void ComponentParticleEmmiter::UpdateRootParticle()
{
	SetEmmisionRate(emmision_rate);

	root_particle->SetMaxLifetime(max_lifetime);
	root_particle->SetVelocity(velocity);
	root_particle->SetAngular(angular_v); 
	root_particle->SetColor(color);
	root_particle->SetGravity(gravity); 
	root_particle->SetEmmisionAngle(emision_angle); 

	root_particle->SetInterpolatingColor(apply_color_interpolation, Color(initial_color[0], initial_color[1], initial_color[2], initial_color[3]), Color(final_color[0], final_color[1], final_color[2], final_color[3]));
	root_particle->SetInterpolationSize(apply_size_interpolation, initial_scale, final_scale);

	root_particle->SetBillboarding(billboarding);
	root_particle->SetWorldSpace(relative_pos);

	//if (apply_rotation_interpolation) root_particle->SetInterpolationRotation(true, initial_angular_v, final_angular_v);
	//else root_particle->SetAngular(angular_v);

	if (apply_size_interpolation)
		root_particle->SetInterpolationSize(true, initial_scale, final_scale);
}

ComponentParticleEmmiter::~ComponentParticleEmmiter()
{
}

void ComponentParticleEmmiter::Save(Data & data) const
{
}

void ComponentParticleEmmiter::Load(Data & data)
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
	return emmision_rate;
}

void ComponentParticleEmmiter::SetEmmisionRate(float new_emision_rate)
{
	emmision_rate = new_emision_rate;
	SetFrequencyFromRate(emmision_rate);
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

float ComponentParticleEmmiter::GetLifetime() const
{
	return particles_lifetime;
}

void ComponentParticleEmmiter::SetLifeTime(float new_life)
{
	particles_lifetime = new_life;
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

int ComponentParticleEmmiter::GetTextureID(int pos)
{
	return shapes_ids[pos];
}

int ComponentParticleEmmiter::GetTextureIDAmount()
{
	return shapes_amount;
}

void ComponentParticleEmmiter::SetCurrentTextureID(uint texture_id)
{
	curr_texture_id = texture_id;
}

uint ComponentParticleEmmiter::GetCurrentTextureID() const
{
	return curr_texture_id;
}

vector<ParticleAnimation> ComponentParticleEmmiter::GetAllParticleAnimations()
{
	return particle_animations;
}

Particle * ComponentParticleEmmiter::GetRootParticle() const
{
	return root_particle;
}