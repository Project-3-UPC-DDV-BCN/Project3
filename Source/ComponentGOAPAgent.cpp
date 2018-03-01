#include "ComponentGOAPAgent.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"
#include "Application.h"
#include "ModuleResources.h"

ComponentGOAPAgent::ComponentGOAPAgent(GameObject * attached_gameobject)
{
	SetGameObject(attached_gameobject);
	SetType(Component::ComponentType::CompGOAPAgent);
	SetName("GOAPAgent");
	SetActive(true);
}

void ComponentGOAPAgent::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());

	data.AddInt("num_goals", goals.size());
	for (int i = 0; i < goals.size(); ++i)
	{
		data.CreateSection("goal_" + std::to_string(i));
		data.AddString("name", goals[i]->GetName());
		data.CloseSection();
	}
	data.AddInt("num_actions", actions.size());
	for (int i = 0; i < goals.size(); ++i)
	{
		data.CreateSection("action_" + std::to_string(i));
		data.AddString("name", actions[i]->GetName());
		data.CloseSection();
	}
	data.AddInt("num_variables", blackboard.size());
	for (int i = 0; i < blackboard.size(); ++i)
	{
		data.CreateSection("varaiable_" + std::to_string(i));
		data.AddInt("type", blackboard[i]->GetType());
		data.AddString("name", blackboard[i]->GetName());
		switch (blackboard[i]->GetType())
		{
		case GOAPVariable::T_BOOL:
			data.AddBool("value", blackboard[i]->GetValue());
			break;
		case GOAPVariable::T_FLOAT:
			data.AddFloat("value", blackboard[i]->GetValue());
			break;
		default:
			break;
		}
		data.CloseSection();
	}
}

void ComponentGOAPAgent::Load(Data & data)
{
	SetType((Component::ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));

	int num_goals = data.GetInt("num_goals");
	for (int i = 0; i < num_goals; ++i)
	{
		data.EnterSection("goal_" + std::to_string(i));
		AddGoal(App->resources->GetGOAPGoal(data.GetString("name")));
		data.LeaveSection();
	}

	int num_actions = data.GetInt("num_actions");
	for (int i = 0; i < num_actions; ++i)
	{
		data.EnterSection("action_" + std::to_string(i));
		AddAction(App->resources->GetGOAPAction(data.GetString("name")));
		data.LeaveSection();
	}

	int num_variables = data.GetInt("num_varaibles");
	for (int i = 0; i < num_variables; ++i)
	{
		data.EnterSection("varaiable_" + std::to_string(i));
		std::string name = data.GetString("name");
		GOAPVariable::VariableType type = (GOAPVariable::VariableType)data.GetInt("type");
		data.LeaveSection();
		switch (type)
		{
		case GOAPVariable::T_NULL:
			break;
		case GOAPVariable::T_BOOL:
			AddVariable(name, data.GetBool("value"));
			break;
		case GOAPVariable::T_FLOAT:
			AddVariable(name, data.GetFloat("value"));
			break;
		default:
			break;
		}
		data.LeaveSection();
	}
}

bool ComponentGOAPAgent::Update()
{
	return false;
}

void ComponentGOAPAgent::AddGoal(GOAPGoal * goal)
{
	goals.push_back(goal);
}

void ComponentGOAPAgent::AddAction(GOAPAction * action)
{
	actions.push_back(action);
}

void ComponentGOAPAgent::AddVariable(std::string & name, bool value)
{
	blackboard.push_back(new GOAPVariable(name.c_str(), value));
}

void ComponentGOAPAgent::AddVariable(std::string & name, float value)
{
	blackboard.push_back(new GOAPVariable(name.c_str(), value));
}

void ComponentGOAPAgent::FindActionPath()
{
	//clear current path
	path.clear();

	GOAPGoal* goal_to_complete = GetGoalToComplete();
}

GOAPGoal * ComponentGOAPAgent::GetGoalToComplete()
{
	GOAPGoal* ret = goals[0];
	for (int i = 1; i < goals.size(); ++i)
	{
		if (ret->GetPriority() < goals[i]->GetPriority())
			ret = goals[i];
	}
	return ret;
}
