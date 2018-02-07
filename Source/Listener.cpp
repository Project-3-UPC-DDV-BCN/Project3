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

	return ret;
}

AkGameObjectID Listener::GetId() const
{
	return obj->GetID();
}

void Listener::ApplyReverb(float value, const char * bus)
{
}
