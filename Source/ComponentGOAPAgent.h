#ifndef __COMPONENTGOAPAGENT__
#define __COMPONENTGOAPAGENT__

#include "Component.h"

class ComponentGOAPAgent : public Component
{
public:
	ComponentGOAPAgent(GameObject* attached_gameobject);
	
	void Save(Data& data) const;
	void Load(Data& data);
};

#endif // !__COMPONENTGOAPAGENT__

