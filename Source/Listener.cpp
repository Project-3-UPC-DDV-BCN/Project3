#include "Listener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioEvent.h"
#include "GameObject.h"
#include "ComponentTransform.h"

Listener::Listener(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Audio Listener");
	SetType(ComponentType::CompAudioListener);
	SetGameObject(attached_gameobject);

	ComponentTransform* trans = (ComponentTransform*) attached_gameobject->GetComponent(Component::CompTransform);

	obj = App->audio->CreateListener(attached_gameobject->GetName().c_str(), trans->GetGlobalPosition());
	App->audio->SetDefaultListener(this);
}

Listener::~Listener()
{
}

bool Listener::Update()
{
	bool ret = true;

	//Update Listener Positioin

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

AkGameObjectID Listener::GetId() const
{
	return obj->GetID();
}

void Listener::ApplyReverb(float value, const char * bus)
{
	obj->SetAuxiliarySends(value, bus, App->audio->GetDefaultListener()->GetId());
}

void Listener::Save(Data & data) const
{
}

void Listener::Load(Data & data)
{
}
