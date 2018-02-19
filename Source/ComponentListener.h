#ifndef _LISTENER_H_
#define _LISTENER_H_

#include "Component.h"
#include "ModuleAudio.h"

class ComponentListener 
	: public Component 
{
public:
	ComponentListener(GameObject* attached_gameobject);
	~ComponentListener();
	bool Update();
	void UpdatePosition();
	AkGameObjectID GetId() const;
	void ApplyReverb(float value, const char* bus);

	void Save(Data& data) const;
	void Load(Data& data);
private:

	Wwise::SoundObject* obj;
	AABB box = AABB::AABB(float3(-1.0f, -1.0f, -1.0f), float3(1.0f, 1.0f, 1.0f));


};


#endif // !_LISTENER_H_
