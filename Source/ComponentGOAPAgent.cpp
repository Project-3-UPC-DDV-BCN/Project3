#include "ComponentGOAPAgent.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"
#include "GOAPVariable.h"
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

bool ComponentGOAPAgent::GetBlackboardVariable(const char * name, float & var) const
{
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (name == blackboard[i]->GetName())
		{
			var = blackboard[i]->GetValue();
			return true;
		}
	}
	return false;
}

bool ComponentGOAPAgent::GetBlackboardVariable(const char * name, bool & var) const
{
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (name == blackboard[i]->GetName())
		{
			var = blackboard[i]->GetValue();
			return true;
		}
	}
	return false;
}

void ComponentGOAPAgent::FindActionPath()
{
	//clear current path
	path.clear();

	GOAPGoal* goal_to_complete = GetGoalToComplete();

	if (goal_to_complete != nullptr)
	{
		std::vector<std::vector<GOAPEffect*>> effects_to_fulfill;
		//Track the effects to fulfill for the goal 
		for (int i = 0; i < goal_to_complete->GetNumConditions(); ++i)
		{
			GOAPField* condition = goal_to_complete->GetCondition(i);
			GOAPVariable::VariableType type = condition->GetType();
			switch (type)
			{
			case GOAPVariable::T_NULL:
				break;
			case GOAPVariable::T_BOOL:
			{
				bool cond_var = condition->GetValue();
				bool black_var;
				if (GetBlackboardVariable(condition->GetName(), black_var))
				{
					GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
					switch (cm)
					{
					case GOAPField::CM_EQUAL:
						if (black_var != cond_var)
							effects_to_fulfill[0].push_back(new GOAPEffect(condition->GetName(), cond_var));
						break;
					case GOAPField::CM_DIFFERENT:
						if (black_var == cond_var)
							effects_to_fulfill[0].push_back(new GOAPEffect(condition->GetName(), cond_var));
						break;
					default:
						break;
					}
				}
				else
				{
					CONSOLE_ERROR("Blackboard has no variable %s, condition %s is avoided.", condition->GetName());
				}
				break;
			}
			case GOAPVariable::T_FLOAT:
			{
				float cond_var = condition->GetValue();
				float black_var;
				if (GetBlackboardVariable(condition->GetName(), black_var))
				{
					GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
					switch (cm)
					{
					case GOAPField::CM_NULL:
						break;
					case GOAPField::CM_EQUAL:
						if (cond_var != black_var)
						{
							if (black_var > cond_var)
								effects_to_fulfill[0].push_back(new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f));
							else
								effects_to_fulfill[0].push_back(new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f));
						}
						break;
					case GOAPField::CM_DIFFERENT:
						break;
					case GOAPField::CM_HIGHER:
						break;
					case GOAPField::CM_LOWER:
						break;
					case GOAPField::CM_HIGHER_OR_EQUAL:
						break;
					case GOAPField::CM_LOWER_OR_EQUAL:
						break;
					default:
						break;
					}
				}
				else
				{
					CONSOLE_ERROR("Blackboard has no variable %s, condition %s is avoided.", condition->GetName());
				}
			}
			default:
				break;
			}
		}

	}
	else
	{
		CONSOLE_WARNING("GOAP Agent don't have a goal to complete!");
	}
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
