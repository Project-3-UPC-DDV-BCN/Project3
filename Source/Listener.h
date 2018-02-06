#ifndef _LISTENER_H_
#define _LISTENER_H_

#include "Component.h"
#include "ModuleAudio.h"

class Listener 
	: public Component 
{
public:
	Listener(GameObject* attached_gameobject);
	~Listener();
	bool Update();
	AkGameObjectID GetId() const;
	void ApplyReverb(float value, const char* bus);


private:

	Wwise::SoundObject* obj;
	AABB box = AABB::AABB(float3(-1.0f, -1.0f, -1.0f), float3(1.0f, 1.0f, 1.0f));


};


#endif // !_LISTENER_H_
