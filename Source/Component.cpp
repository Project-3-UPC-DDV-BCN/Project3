#include "Component.h"
#include "Application.h"

Component::Component()
{
	type = CompUnknown;
	active = true;
	gameobject = nullptr;
	name = "Unknown";
	uuid = App->RandomNumber().Int();
}

Component::~Component()
{
}

void Component::OnEnable()
{
}

void Component::OnDisable()
{
}

void Component::SetActive(bool active)
{
	this->active = active;
	if (this->active) {
		OnEnable();
	}
	else {
		OnDisable();
	}
}

bool Component::IsActive() const
{
	return active;
}

void Component::SetGameObject(GameObject * attached_gameobject)
{
	gameobject = attached_gameobject;
}

GameObject * Component::GetGameObject() const
{
	return gameobject;
}

void Component::SetType(ComponentType type)
{
	this->type = type;
}

Component::ComponentType Component::GetType() const
{
	return type;
}

void Component::SetName(std::string name)
{
	this->name = name;
}

std::string Component::GetName() const
{
	return name;
}

UID Component::GetUID() const
{
	return uuid;
}

void Component::SetUID(UID uid)
{
	uuid = uid;
}

bool Component::Start()
{
	return true;
}

bool Component::Update()
{
	return true;
}

bool Component::CleanUp()
{
	return true;
}

void Component::Save(Data & data) const
{
}

void Component::Load(Data & data)
{
}
