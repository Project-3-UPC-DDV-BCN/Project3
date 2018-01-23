#include "ComponentTransform.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Transform");
	SetType(ComponentType::CompTransform);
	SetGameObject(attached_gameobject);

	position = float3(0.f, 0.f, 0.f);
	shown_rotation = float3(0.f, 0.f, 0.f);
	rotation = Quat(0.f, 0.f, 0.f, 0.f);
	scale = float3(1.f, 1.f, 1.f);
	global_pos = float3(0.f, 0.f, 0.f);
	global_rot = float3(0.f, 0.f, 0.f);
	global_scale = float3(1.f, 1.f, 1.f);
	transform_matrix.SetIdentity();
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::SetPosition(float3 position)
{
	this->position = position;
	UpdateGlobalMatrix();
}

float3 ComponentTransform::GetGlobalPosition() const
{
	return global_pos;
}

float3 ComponentTransform::GetLocalPosition() const
{
	/*if (GetGameObject()->GetParent() != nullptr)
	{
		ComponentTransform* parent_transform = (ComponentTransform*)GetGameObject()->GetParent()->GetComponent(ComponentType::CompTransform);
		return GetGlobalPosition() - parent_transform->GetGlobalPosition();
	}*/
	return position; //If it's the parent. local position = global position
}

void ComponentTransform::SetRotation(float3 rotation)
{
	this->shown_rotation = rotation;
	this->rotation = Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);
	UpdateGlobalMatrix();
}

float3 ComponentTransform::GetGlobalRotation() const
{
	return global_rot;
}

float3 ComponentTransform::GetLocalRotation() const
{
	/*if (GetGameObject()->GetParent() != nullptr)
	{
		ComponentTransform* parent_transform = (ComponentTransform*)GetGameObject()->GetParent()->GetComponent(ComponentType::CompTransform);
		return GetGlobalRotation() - parent_transform->GetGlobalRotation();
	}*/
	return shown_rotation; //If it's the parent. local rotation = global rotation
}

void ComponentTransform::SetScale(float3 scale)
{
	this->scale = scale;
	UpdateGlobalMatrix();
}

float3 ComponentTransform::GetGlobalScale() const
{
	return global_scale;
}

float3 ComponentTransform::GetLocalScale() const
{
	/*if (GetGameObject()->GetParent() != nullptr)
	{
		ComponentTransform* parent_transform = (ComponentTransform*)GetGameObject()->GetParent()->GetComponent(ComponentType::CompTransform);
		return GetGlobalScale() - parent_transform->GetGlobalScale();
	}*/
	return scale; //If it's the parent. local scale = global scale
}

void ComponentTransform::UpdateGlobalMatrix()
{
	if (!this->GetGameObject()->IsRoot())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)this->GetGameObject()->GetParent()->GetComponent(Component::CompTransform);

		transform_matrix = transform_matrix.FromTRS(position, rotation, scale);
		transform_matrix = parent_transform->transform_matrix * transform_matrix;

		float3 _pos, _scale;
		Quat _rot;
		transform_matrix.Decompose(_pos, _rot, _scale);
		global_pos = _pos;
		global_rot = _rot.ToEulerXYZ() * RADTODEG;
		global_scale = _scale;
	}
	else
	{
		transform_matrix = float4x4::FromTRS(position, rotation, scale);
		for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
		{
			ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
			child_transform->UpdateGlobalMatrix();
		}

		global_pos = position;
		global_rot = shown_rotation;
		global_scale = scale;
	}

	GetGameObject()->UpdateBoundingBox();
	//If gameobject has a camera component
	GetGameObject()->UpdateCamera();
}

void ComponentTransform::UpdateLocals()
{
	if (!this->GetGameObject()->IsRoot())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)this->GetGameObject()->GetParent()->GetComponent(Component::CompTransform);

		float4x4 local_transform = transform_matrix.Inverted() * parent_transform->transform_matrix;

		local_transform.Decompose(position, rotation, scale);

		float3 _pos, _scale;
		Quat _rot;
		transform_matrix.Decompose(_pos, _rot, _scale);
		global_pos = _pos;
		global_rot = _rot.ToEulerXYZ() * RADTODEG;
		global_scale = _scale;
	}
}

const float4x4 ComponentTransform::GetMatrix() const
{
	return transform_matrix;
}

const float * ComponentTransform::GetOpenGLMatrix() const
{
	return transform_matrix.Transposed().ptr();
}

void ComponentTransform::SetMatrix(const float4x4 & matrix)
{
	transform_matrix = matrix;

	UpdateLocals();

	if (this->GetGameObject()->IsRoot())
	{
		for (std::list<GameObject*>::iterator it = this->GetGameObject()->childs.begin(); it != this->GetGameObject()->childs.end(); it++)
		{
			ComponentTransform* child_transform = (ComponentTransform*)(*it)->GetComponent(Component::CompTransform);
			child_transform->UpdateGlobalMatrix();
		}
	}
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
	float4 float_to_quat = data.GetVector4("rotation");
	rotation.x = float_to_quat.x;
	rotation.y = float_to_quat.y;
	rotation.z = float_to_quat.z;
	rotation.w = float_to_quat.w;
	shown_rotation = rotation.ToEulerXYZ();
	shown_rotation *= RADTODEG;
	scale = data.GetVector3("scale");
}
