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

	void AddGoal(GOAPGoal* goal);
	void AddAction(GOAPAction* action);
	void AddVariable(std::string & name, bool value);
	void AddVariable(std::string & name, float value);

private:
	void FindActionPath();


public:
	std::vector<GOAPGoal*> goals;
	std::vector<GOAPAction*> actions;
	std::vector<GOAPVariable*> blackboard;

private:
	std::vector<GOAPAction*> path;

};

#endif // !__COMPONENTGOAPAGENT__

