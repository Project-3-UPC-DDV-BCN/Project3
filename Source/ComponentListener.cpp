#include "ComponentListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioEvent.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ComponentListener::ComponentListener(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Audio Listener");
	SetType(ComponentType::CompAudioListener);
	SetGameObject(attached_gameobject);

	ComponentTransform* trans = (ComponentTransform*) attached_gameobject->GetComponent(Component::CompTransform);

	obj = App->audio->CreateListener(attached_gameobject->GetName().c_str(), trans->GetGlobalPosition());
	App->audio->SetDefaultListener(this);
}

ComponentListener::~ComponentListener()
{
}

bool ComponentListener::Update()
{
	bool ret = true;

	//Update ComponentListener Positioin

	ComponentTransform* trans = (ComponentTransform*) GetGameObject()->GetComponent(Component::ComponentType::CompTransform);

	if (trans != nullptr)
	{
		float3 pos = trans->GetGlobalPosition();
		Quat rot = Quat::FromEulerXYZ(trans->GetGlobalRotation().x * DEGTORAD, trans->GetGlobalRotation().y * DEGTORAD, trans->GetGlobalRotation().z * DEGTORAD);

		float3 up = rot.Transform(float3(0, 1, 0));
		float3 front = rot.Transform(float3(0, 0, 1));

		up.Normalize();
		front.Normalize();

		obj->SetPosition(pos.x, pos.y, pos.z, front.x, front.y, -front.z, up.x, up.y, up.z);

		box.minPoint = trans->GetGlobalPosition() - float3(1, 1, 1);
		box.maxPoint = trans->GetGlobalPosition() + float3(1, 1, 1);
	}


	return ret;
}

AkGameObjectID ComponentListener::GetId() const
{
	return obj->GetID();
}

void ComponentListener::ApplyReverb(float value, const char * bus)
{
	obj->SetAuxiliarySends(value, bus, App->audio->GetDefaultListener()->GetId());
}

void ComponentListener::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.CreateSection("Obj position");
		data.AddFloat("pos_x", obj->GetPosition().X);
		data.AddFloat("pos_y", obj->GetPosition().Y);
		data.AddFloat("pos_z", obj->GetPosition().Z);
		data.AddFloat("front_x", obj->GetFront().X);
		data.AddFloat("front_y", obj->GetFront().Y);
		data.AddFloat("front_z", obj->GetFront().Z);	
		data.AddFloat("top_x", obj->GetTop().X);
		data.AddFloat("top_y", obj->GetTop().Y);
		data.AddFloat("top_z", obj->GetTop().Z);
	data.CloseSection();
	data.CreateSection("AABB");
		data.AddFloat("min_point_x", box.minPoint.x);
		data.AddFloat("min_point_y", box.minPoint.y);
		data.AddFloat("min_point_z", box.minPoint.z);
		data.AddFloat("max_point_x", box.maxPoint.x);
		data.AddFloat("max_point_y", box.maxPoint.y);
		data.AddFloat("max_point_z", box.maxPoint.z);
	data.CloseSection();
}

void ComponentListener::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	data.EnterSection("Obj position");
		obj->SetPosition(
			data.GetFloat("pos_x")
		,	data.GetFloat("pos_y")
		,	data.GetFloat("pos_z")
		,	data.GetFloat("front_x")
		,	data.GetFloat("front_y")
		,	data.GetFloat("front_z")
		,	data.GetFloat("top_x")
		,	data.GetFloat("top_y")
		,	data.GetFloat("top_z")
		);
	data.LeaveSection();
	data.EnterSection("AABB");
		box.minPoint.Set(
			data.GetFloat("min_point_x")
		,	data.GetFloat("min_point_y")
		,	data.GetFloat("min_point_z")
		);
		box.maxPoint.Set(
			data.GetFloat("max_point_x")
		,	data.GetFloat("max_point_y")
		,	data.GetFloat("max_point_z")
		);
	data.LeaveSection();
}
