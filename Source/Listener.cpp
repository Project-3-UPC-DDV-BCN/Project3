#include "Listener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioEvent.h"

Listener::Listener(GameObject * attached_gameobject)
{
	SetActive(true);
	SetName("Audio Listener");
	SetType(ComponentType::CompAudioListener);
	SetGameObject(attached_gameobject)

	obj = App->audio->CreateListener(own->GetName(), trans->GetPosition());
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
