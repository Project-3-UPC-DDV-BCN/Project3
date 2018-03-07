#ifndef __COMPONENTGOAPAGENT__
#define __COMPONENTGOAPAGENT__

#include "Component.h"
#include <vector>
#include "GOAPEffect.h"
#include "GOAPField.h"

#define MAX_ITERATIONS 50

class GOAPAction;
class GOAPGoal;
class GOAPVariable;

enum ActionState
{
	AS_NULL,
	AS_FAIL,
	AS_RUNNING,
	AS_COMPLETED
};

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

	bool GetBlackboardVariable(const char* name, float& var) const;
	bool GetBlackboardVariable(const char* name, bool& var) const;

	void SetBlackboardVariable(const char* name, float var);
	void SetBlackboardVariable(const char* name, bool var);

	void CompleteCurrentAction();
	void FailCurrentAction();

private:
	void FindActionPath();
	GOAPGoal* GetGoalToComplete();

	void ResetTmpEffects();
	GOAPEffect* TmpEffectsContains(const char* name, GOAPEffect::EffectType t) const;

	//Fill the effects froma goal for a new path
	void FillEffectsFromGoal(GOAPGoal* goal);

	//Adds the field condition to effects while pathfinding
	void AddEffectsFormField(GOAPField* field, int path_index);

	int AddPossiblePath(int ref);

	int GetBestTmpPath();

	void ResetVariables();

	bool SystemFulfillCondition(GOAPField* condition);

	void ApplyEffect(GOAPEffect* effect);

public:
	std::vector<GOAPGoal*> goals;
	std::vector<GOAPAction*> actions;
	std::vector<GOAPVariable*> blackboard;

private:
	std::vector<GOAPAction*> path;
	bool path_valid = false;
	bool new_path = true;
	bool calculating_path = false;

	std::vector<std::vector<GOAPAction*>> tmp_paths;
	std::vector<std::vector<GOAPEffect*>> effects_to_fulfill;
	std::vector<bool> completed_paths;
	std::vector<GOAPEffect*> created_effects;
	uint possible_paths = 0;
	GOAPGoal* goal_to_complete = nullptr;

	GOAPAction* curr_action = nullptr;
	bool need_start = true;
	ActionState curr_act_state = AS_NULL;

};

#endif // !__COMPONENTGOAPAGENT__

