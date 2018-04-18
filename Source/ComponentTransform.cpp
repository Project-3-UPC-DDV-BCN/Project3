#include "ComponentTransform.h"
#include "GameObject.h"
#include "componentRigidBody.h"
#include "ComponentLight.h"
#include "Application.h"
#include "ComponentBlast.h"
#include "PerfTimer.h"
#include "ComponentMeshRenderer.h"

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
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::SetPosition(float3 position)
{
	this->position = position;
	UpdateGlobalMatrix();
	dirty = true;
}

void ComponentTransform::SetPositionFromRB(float3 position)
{
	this->position = position;
	UpdateGlobalMatrix(true);
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

void ComponentTransform::SetRotation(float3 rot)
{
	this->rotation = Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD);
	shown_rotation = rot;
	UpdateGlobalMatrix();
	dirty = true;
}

void ComponentTransform::SetIncrementalRotation(float3 rotation)
{
	float3 diff = rotation - shown_rotation;
	Quat q = Quat::FromEulerXYZ(diff.x * DEGTORAD, diff.y * DEGTORAD, diff.z * DEGTORAD);
	this->rotation = this->rotation * q;

	shown_rotation = rotation;
	UpdateGlobalMatrix();
	dirty = true;
}

float3 ComponentTransform::GetGlobalRotation() const
{
	return global_rot;
}

Quat ComponentTransform::GetGlobalQuatRotation() const
{
	return global_quat_rot;
}

float3 ComponentTransform::GetLocalRotation() const
{
	return shown_rotation; //If it's the parent. local rotation = global rotation
}

Quat ComponentTransform::GetQuatRotation() const
{
	return rotation;
}

void ComponentTransform::SetQuatRotation(Quat q)
{
	this->rotation = q;

	shown_rotation = rotation.ToEulerXYZ() * RADTODEG;
	UpdateGlobalMatrix();
	dirty = true;
}

void ComponentTransform::SetRotationFromRB(Quat q)
{
	this->rotation = q;

	shown_rotation = rotation.ToEulerXYZ() * RADTODEG;
	UpdateGlobalMatrix(true);
	dirty = true;
}

void ComponentTransform::SetScale(float3 scale)
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

void ComponentTransform::UpdateGlobalMatrix(bool from_rigidbody)
{	
	if (is_particle || (this->GetGameObject()->IsRoot() || this->GetGameObject()->GetParent() == nullptr))
	{
		transform_matrix = float4x4::FromTRS(position, rotation, scale);

		global_pos = position;
		global_quat_rot = rotation;
		global_rot = shown_rotation;
		global_scale = scale;

		if (!is_particle)
		{
			for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
			{
				ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
				child_transform->UpdateGlobalMatrix();
			}
		}
	}
	else
	{
		ComponentTransform* parent_transform = (ComponentTransform*)this->GetGameObject()->GetParent()->GetComponent(Component::CompTransform);

		global_pos = parent_transform->GetGlobalPosition() + position;
		global_quat_rot = parent_transform->GetGlobalQuatRotation() * rotation;
		global_rot = global_quat_rot.ToEulerXYZ() * RADTODEG;
		global_scale = parent_transform->GetGlobalScale().Mul(scale);

		transform_matrix = float4x4::FromTRS(position, rotation, scale);

		transform_matrix = parent_transform->transform_matrix * transform_matrix;
		
		for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
		{
			ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
			child_transform->UpdateGlobalMatrix();
		}
	}

	if (!is_particle)
	{
		GetGameObject()->UpdateBoundingBox();
		//If gameobject has a camera component
		GetGameObject()->UpdateCamera();

		/*ComponentMeshRenderer* renderer = (ComponentMeshRenderer*)GetGameObject()->GetComponent(Component::CompMeshRenderer);
		if (renderer)
		{
			global_pos = renderer->bounding_box.CenterPoint();
		}*/

		if (from_rigidbody)
		{
			ComponentRigidBody* rb = (ComponentRigidBody*)GetGameObject()->GetComponent(Component::CompRigidBody);
			if (rb)
			{
				//rb->SetTransform(transform_matrix.Transposed().ptr());
				rb->SetPosition(global_pos);
				rb->SetRotation(rotation);
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

		global_pos = parent_transform->GetGlobalPosition() + position;
		global_rot = (parent_transform->GetQuatRotation() * rotation).ToEulerXYZ() * RADTODEG;
		global_scale = parent_transform->GetGlobalScale().Mul(scale);
	}
	else
	{
		global_pos = position;
		global_rot = shown_rotation;
		global_scale = scale;
	}

	//local_transform.Decompose(position, rotation, scale);

	/*float3 _pos, _scale;
	Quat _rot;
	transform_matrix.Decompose(_pos, _rot, _scale);
	global_pos = _pos;
	global_rot = _rot.ToEulerXYZ() * RADTODEG;
	global_scale = _scale;*/
	
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
	float3 pos;
	Quat rot;
	float3 scale;
	matrix.Decompose(pos, rot, scale);
	position = pos;
	rotation = rot;
	shown_rotation = rot.ToEulerXYZ() * RADTODEG;
	this->scale = scale;
	UpdateGlobalMatrix();
}

void ComponentTransform::LookAt(float3 dir, float3 up)
{
	float3 z(dir);
	float3 x(-z.Normalized().Cross(up));
	float3 y(x.Normalized().Cross(-z.Normalized()));
	float3x3 mat(x, y, z);
	//mat.ToQuat crashes and need to do this workaround...
	float3 eulers = mat.ToEulerXYZ();
	Quat q = Quat::FromEulerXYZ(eulers.x, eulers.y, eulers.z);
	SetQuatRotation(q);
	/*CONSOLE_LOG("%.3f,%.3f,%.3f,%.3f", rotation.x, rotation.y, rotation.z, rotation.w);*/
}

bool ComponentTransform::AnyDirty()
{
	GameObject* current_go= GetGameObject(); 

	while (current_go != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)current_go->GetComponent(CompTransform);

		if (transform->dirty)
			return true;

		current_go = current_go->GetParent(); 
	}

	return false; 
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