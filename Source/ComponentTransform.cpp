#include "ComponentTransform.h"
#include "GameObject.h"
#include "componentRigidBody.h"
#include "ComponentLight.h"
#include "Application.h"
#include "ComponentBlast.h"
#include "PerfTimer.h"

ComponentTransform::ComponentTransform(GameObject* attached_gameobject, bool is_particle)
{
	SetActive(true);
	SetName("Transform");
	SetType(ComponentType::CompTransform);
	SetGameObject(attached_gameobject);
	dirty = false; 

	position = float3(0.f, 0.f, 0.f);
	shown_rotation = float3(0.f, 0.f, 0.f);
	rotation = Quat(0.f, 0.f, 0.f, 1.f);
	scale = float3(1.f, 1.f, 1.f);
	global_pos = float3(0.f, 0.f, 0.f);
	global_rot = float3(0.f, 0.f, 0.f);
	global_scale = float3(1.f, 1.f, 1.f);
	transform_matrix.SetIdentity();

	this->is_particle = is_particle;
	rb_transforms_go = false;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::SetPosition(float3 position)
{
	if (!rb_transforms_go || rb_transforms_go && !App->IsPlaying())
	{
		this->position = position;
		UpdateGlobalMatrix();
		dirty = true;
	}
	else
	{
		CONSOLE_ERROR("SetPosition: RigidBody is in charge of transforms");
	}
}

void ComponentTransform::SetPositionFromRB(float3 position)
{
	this->position = position;
	UpdateGlobalMatrix();
	dirty = true;
}

float3 ComponentTransform::GetGlobalPosition() const
{
	return global_pos;
}

float3 ComponentTransform::GetLocalPosition() const
{
	return position; //If it's the parent. local position = global position
}

void ComponentTransform::SetRotation(float3 rotation)
{
	if (!rb_transforms_go || rb_transforms_go && !App->IsPlaying())
	{
		this->rotation = Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);

		shown_rotation = rotation;
		UpdateGlobalMatrix();
		dirty = true; 
	}
	else
	{
		CONSOLE_ERROR("SetRotation: RigidBody is in charge of transforms");
	}
}

void ComponentTransform::SetRotationFromRB(float3 rotation)
{
	this->rotation = Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);

	shown_rotation = rotation;
	UpdateGlobalMatrix();
	dirty = true;
}

void ComponentTransform::SetIncrementalRotation(float3 rotation)
{
	if (!rb_transforms_go || rb_transforms_go && !App->IsPlaying())
	{
		float3 diff = rotation - shown_rotation;
		Quat q = Quat::FromEulerXYZ(diff.x * DEGTORAD, diff.y * DEGTORAD, diff.z * DEGTORAD);
		this->rotation = this->rotation * q;

		shown_rotation = rotation;
		UpdateGlobalMatrix();
		dirty = true;
	}
	else
	{
		CONSOLE_ERROR("SetIncrementalRotation: RigidBody is in charge of transforms");
	}
}

float3 ComponentTransform::GetGlobalRotation() const
{
	return global_rot;
}

float3 ComponentTransform::GetLocalRotation() const
{
	return shown_rotation; //If it's the parent. local rotation = global rotation
}

Quat ComponentTransform::GetQuatRotation() const
{
	return rotation;
}

void ComponentTransform::SetScale(float3 scale)
{
	if (!rb_transforms_go || rb_transforms_go && !App->IsPlaying())
	{
		this->scale = scale;
		UpdateGlobalMatrix();
		dirty = true;
	}
	else
	{
		CONSOLE_ERROR("SetScale: RigidBody is in charge of transforms");
	}
}

void ComponentTransform::SetScaleFromRB(float3 scale)
{
	this->scale = scale;
	UpdateGlobalMatrix();
	dirty = true;
}

float3 ComponentTransform::GetGlobalScale() const
{
	return global_scale;
}

float3 ComponentTransform::GetLocalScale() const
{
	return scale; //If it's the parent. local scale = global scale
}

void ComponentTransform::UpdateGlobalMatrix()
{
	if (!is_particle && !(this->GetGameObject()->IsRoot() || this->GetGameObject()->GetParent() == nullptr))
	{
		ComponentTransform* parent_transform = (ComponentTransform*)this->GetGameObject()->GetParent()->GetComponent(Component::CompTransform);

		transform_matrix = float4x4::FromTRS(position, rotation, scale);
		transform_matrix = parent_transform->transform_matrix * transform_matrix;

		for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
		{
			ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
			child_transform->UpdateGlobalMatrix();
		}

		global_pos = parent_transform->GetGlobalPosition() + position;
		global_rot = (parent_transform->GetQuatRotation() * rotation).ToEulerXYZ() * RADTODEG;
		global_scale = parent_transform->GetGlobalScale().Mul(scale);
	}
	else
	{
		transform_matrix = float4x4::FromTRS(position, rotation, scale);

		if (!is_particle)
		{
			for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
			{
				ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
				child_transform->UpdateGlobalMatrix();
			}
		}

		global_pos = position;
		global_rot = shown_rotation;
		global_scale = scale;
	}

	if (!is_particle)
	{
		GetGameObject()->UpdateBoundingBox();
		//If gameobject has a camera component
		GetGameObject()->UpdateCamera();

		if (!rb_transforms_go)
		{
			ComponentRigidBody* rb = (ComponentRigidBody*)GetGameObject()->GetComponent(Component::CompRigidBody);
			if (rb)
			{
				//rb->SetTransform(transform_matrix.Transposed().ptr());
				rb->SetPosition(global_pos);
				rb->SetRotation(global_rot);
			}
			else
			{
				ComponentBlast* blast = (ComponentBlast*)GetGameObject()->GetComponent(Component::CompBlast);
				if (blast)
				{
					blast->SetTransform(transform_matrix.Transposed().ptr());
					//blast->SetPosition(global_pos);
					//blast->SetRotation(global_rot);
				}
			}
		}
		
	}
}

void ComponentTransform::UpdateLocals()
{
	if(!this->GetGameObject()->IsRoot())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)this->GetGameObject()->GetParent()->GetComponent(Component::CompTransform);
		transform_matrix = transform_matrix * parent_transform->transform_matrix;
	}

	//local_transform.Decompose(position, rotation, scale);

	float3 _pos, _scale;
	Quat _rot;
	transform_matrix.Decompose(_pos, _rot, _scale);
	global_pos = _pos;
	global_rot = _rot.ToEulerXYZ() * RADTODEG;
	global_scale = _scale;
	
}

const float4x4 ComponentTransform::GetMatrix() const
{
	return transform_matrix;
}

const float4x4 ComponentTransform::GetOpenGLMatrix() const
{
	return transform_matrix.Transposed();
}

void ComponentTransform::SetMatrix(const float4x4 & matrix)
{
	transform_matrix = matrix;

	UpdateLocals();

	if (GetGameObject() == nullptr)
	{
		transform_matrix = matrix;
		return; 
	}
		
	if (this->GetGameObject()->IsRoot())
	{
		for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
		{
			ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
			child_transform->UpdateGlobalMatrix();
		}
	}
}

void ComponentTransform::SetTransformedFromRB(bool transformed)
{
	rb_transforms_go = transformed;
}

bool ComponentTransform::GetTransformedFromRB() const
{
	return rb_transforms_go;
}

void ComponentTransform::RotateAroundAxis(float3 axis, float angle)
{
	this->rotation = this->rotation * Quat::RotateAxisAngle(axis, angle * DEGTORAD);
	shown_rotation = this->rotation.ToEulerXYZ() * RADTODEG;
	UpdateGlobalMatrix();
	dirty = true;
}

float3 ComponentTransform::GetForward() const
{
	return transform_matrix.WorldZ();
}

float3 ComponentTransform::GetUp() const
{
	return transform_matrix.WorldY();
}

float3 ComponentTransform::GetRight() const
{
	return transform_matrix.WorldX();
}

bool ComponentTransform::IsParticle()
{
	return is_particle;
}

void ComponentTransform::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddVector3("position", position);
	float4 quat_to_float;
	quat_to_float.x = rotation.x;
	quat_to_float.y = rotation.y;
	quat_to_float.z = rotation.z;
	quat_to_float.w = rotation.w;
	data.AddVector4("rotation", quat_to_float);
	data.AddVector3("scale", scale);
}

void ComponentTransform::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	position = data.GetVector3("position");
	SetPosition(position);
	float4 float_to_quat = data.GetVector4("rotation");
	rotation.x = float_to_quat.x;
	rotation.y = float_to_quat.y;
	rotation.z = float_to_quat.z;
	rotation.w = float_to_quat.w;
	shown_rotation = rotation.ToEulerXYZ();
	shown_rotation *= RADTODEG;
	SetRotation(shown_rotation);
	scale = data.GetVector3("scale");
	SetScale(scale);
}