#ifndef __COMPONENTGOAPAGENT__
#define __COMPONENTGOAPAGENT__

#include "Component.h"
#include <vector>

class GOAPAction;
class GOAPGoal;
class GOAPVariable;

class ComponentGOAPAgent : public Component
{
public:
	ComponentGOAPAgent(GameObject* attached_gameobject);
	
	void Save(Data& data) const;
	void Load(Data& data);

	bool Update();

public:
	std::vector<GOAPAction*> goals;
	std::vector<GOAPAction*> actions;
	std::vector<GOAPVariable*> blackboard;
};

#endif // !__COMPONENTGOAPAGENT__

