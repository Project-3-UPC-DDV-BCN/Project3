#include "Particles.h"
#include "Component.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include "Mesh.h"

Particle::Particle()
{
	kill_me = false;
	interpolation_timer.Start();
	particle_color = initial_particle_color;
	particle_angular_v = 0;
	curr_rot = 0;
	animated_particle = false;
	interpolate_size = false;
	interpolate_rotation = false;
	animation_timer.Start();
	twister.Start();
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

void Particle::SetTextureByID(uint texture_ID)
{
	particle_texture_id = texture_ID;
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

void Particle::SetDistanceToCamera(float new_dist)
{
	distance_to_camera = new_dist;
}

float Particle::GetDistanceToCamera()
{
	//float distance = (App->renderer3D->rendering_cam->frustum.pos - components.particle_transform->GetLocalPosition()).Length();
	//distance_to_camera = distance;

	return distance_to_camera;
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

	float increment_r = color_difference[0] * percentage;
	float increment_g = color_difference[1] * percentage;
	float increment_b = color_difference[2] * percentage;
	float increment_a = color_difference[3] * percentage;

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

	float increment_x = total_increment.x * percentage;
	float increment_y = total_increment.y * percentage;
	float increment_z = 1;

	float3 new_scale = { initial_particle_size.x + increment_x, initial_particle_size.y + increment_y, 1 };

	components.particle_transform->SetScale(new_scale);

}

void Particle::SetInterpolationSize(bool interpolate, float3 initial_scale, float3 final_scale)
{
	interpolate_size = interpolate;

	initial_particle_size = initial_scale;
	final_particle_size = final_scale;
}

void Particle::Update()
{

	//Translate the particles in the necessary direction
	movement += particle_gravity*0.01f;
	components.particle_transform->SetPosition(components.particle_transform->GetLocalPosition() + movement);

	//Update the particle color in case of interpolation
	if (interpolate_colors)
		UpdateColor();

	//Update scale
	if (interpolate_size)
		UpdateSize();

	//Update Rotation	
	//UpdateRotation();

	////Update Billboarding
	////if (IsBillboarding() == true)
	////	components.particle_billboarding->Update();

	////Animations
	//if (animated_particle)
	//	UpdateAnimation();

	//Check if they have to be deleted
	if (particle_timer.Read() > max_particle_lifetime * 1000)
		kill_me = true;
}

void Particle::Delete()
{
	components.particle_mesh->CleanUp(); 
//	components.particle_billboarding->Delete();

	components.SetToNull();
}

void Particle::Draw()
{
	App->renderer3D->AddMeshToDraw(components.particle_mesh); 
	//glEnableClientState(GL_VERTEX_ARRAY);

	//glColor3f(particle_color.r, particle_color.g, particle_color.b);

	//glPushMatrix();
	////glMultMatrixf(components.particle_transform->GetOpenGLMatrix().Transposed().ptr());

	//glBindBuffer(GL_ARRAY_BUFFER, components.particle_mesh->GetMesh()->id_vertices_data);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	//if (particle_texture_id != -1)
	//{
	//	glEnable(GL_BLEND);
	//	glEnable(GL_ALPHA_TEST);

	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glBindTexture(GL_TEXTURE_2D, particle_texture_id);

	//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//	glBindBuffer(GL_ARRAY_BUFFER, components.particle_mesh->id_uvs);
	//	glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	//}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, components.particle_mesh->GetMesh()->id_indices);

	//glDrawElements(GL_TRIANGLES, components.particle_mesh->num_indices, GL_UNSIGNED_INT, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glColor3f(255, 255, 255);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//glPopMatrix();
}

Particle::~Particle()
{
}