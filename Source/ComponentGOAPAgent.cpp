#include "ComponentGOAPAgent.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"
#include "GOAPVariable.h"
#include "Application.h"
#include "ModuleResources.h"
#include "CSScript.h"
#include "ModuleScriptImporter.h"

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
	for (int i = 0; i < actions.size(); ++i)
	{
		data.CreateSection("action_" + std::to_string(i));
		data.AddString("name", actions[i]->GetName());
		data.CloseSection();
	}
	data.AddInt("num_variables", blackboard.size());
	for (int i = 0; i < blackboard.size(); ++i)
	{
		data.CreateSection("variable_" + std::to_string(i));
		data.AddInt("type", blackboard[i]->GetType());
		data.AddString("name", blackboard[i]->GetName());
		switch (blackboard[i]->GetType())
		{
		case GOAPVariable::T_BOOL:
			data.AddBool("value", blackboard[i]->GetValueB());
			break;
		case GOAPVariable::T_FLOAT:
			data.AddFloat("value", blackboard[i]->GetValueF());
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

	int num_variables = data.GetInt("num_variables");
	for (int i = 0; i < num_variables; ++i)
	{
		data.EnterSection("variable_" + std::to_string(i));
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
	bool ret = true;
	if (App->IsPlaying() && !App->IsPaused())
	{
		if (path_valid)
		{
			if (curr_action == nullptr)
			{
				curr_action = path.front();
				curr_act_state = AS_RUNNING;
			}

			//check if the conditions for the current actions are fulfilled
			bool fulfilled = true;
			for (int c = 0; c < curr_action->GetNumPreconditions(); ++c)
			{
				GOAPField* precon = curr_action->GetPrecondition(c);
				if (!SystemFulfillCondition(precon)) //this path is not valid since the preconditions for this actions are not fulfilled
				{
					path_valid = false;
					new_path = true;
					fulfilled = false;
					curr_act_state = AS_FAIL;
					curr_action->OnFail();
					break;
				}
			}

			if (fulfilled == true)
			{
				//call the action script functions
				if (need_start)
				{
					curr_action->Start();
					need_start = false;
				}

				if (curr_act_state == AS_RUNNING)
				{
					curr_action->Update();

					//check if the requirements for next action are fulfilled
					GOAPAction* next_action = nullptr;
					bool next_fulfilled = true;
					if (path.size() > 1)
					{
						next_action = path.at(1);
						
						for (int c = 0; c < next_action->GetNumPreconditions(); ++c)
						{
							GOAPField* precon = next_action->GetPrecondition(c);
							if (!SystemFulfillCondition(precon))
							{
								next_fulfilled = false;
								break;
							}
						}
					}

					if (next_action != nullptr && next_fulfilled) // current action has finished his work
					{
						curr_act_state = AS_COMPLETED;
					}

				}

				if (curr_act_state == AS_COMPLETED)
				{
					curr_action->OnComplete();
					//apply the effects

					for (int e = 0; e < curr_action->GetNumEffects(); ++e)
					{
						GOAPEffect* effect = curr_action->GetEffect(e);
						ApplyEffect(effect);
					}

					curr_action = nullptr;
					path.erase(path.begin());
					if (path.empty()) //we finished the path, find a new path
					{
						path_valid = false;
						new_path = true;
					}
				}

				if (curr_act_state == AS_FAIL)//the action failed
				{
					curr_action->OnFail();
					path_valid = false;
					new_path = true;
				}

			}
		}
		else
		{
			FindActionPath();
		}
	}
	return ret;
}

void ComponentGOAPAgent::AddGoal(GOAPGoal * goal)
{
	goals.push_back(goal);
}

void ComponentGOAPAgent::AddAction(GOAPAction * action)
{
	actions.push_back(action);
	if(action->GetScript()!= nullptr)
		action->GetScript()->SetAttachedGameObject(this->GetGameObject());
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
	std::string cmp_name = name;
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (cmp_name == blackboard[i]->GetName())
		{
			var = blackboard[i]->GetValueF();
			return true;
		}
	}
	return false;
}

bool ComponentGOAPAgent::GetBlackboardVariable(const char * name, bool & var) const
{
	std::string cmp_name = name;
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (cmp_name == blackboard[i]->GetName())
		{
			var = blackboard[i]->GetValueB();
			return true;
		}
	}
	return false;
}

void ComponentGOAPAgent::SetBlackboardVariable(const char * name, float var)
{
	bool set = false;
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (blackboard[i]->GetName() == name)
		{
			blackboard[i]->SetValue(var);
			set = true;
			break;
		}
	}

	if (!set)
	{
		CONSOLE_ERROR("Couldn't set %s variable.", name);
	}
}

void ComponentGOAPAgent::SetBlackboardVariable(const char * name, bool var)
{
	bool set = false;
	for (int i = 0; i < blackboard.size(); ++i)
	{
		if (blackboard[i]->GetName() == name)
		{
			blackboard[i]->SetValue(var);
			set = true;
			break;
		}
	}

	if (!set)
	{
		CONSOLE_ERROR("Couldn't set %s variable.", name);
	}
}

void ComponentGOAPAgent::CompleteCurrentAction()
{
	curr_act_state = AS_COMPLETED;
}

void ComponentGOAPAgent::FailCurrentAction()
{
	curr_act_state = AS_FAIL;
}

void ComponentGOAPAgent::FindActionPath()
{
	if (new_path)
	{
		path.clear();
		goal_to_complete = GetGoalToComplete();
		if (goal_to_complete == nullptr)
		{
			CONSOLE_WARNING("GOAP Agent don't have a goal to complete!");
			return;
		}
		possible_paths = 1;
		completed_paths.push_back(false);
		std::vector<GOAPEffect*> e_path;
		effects_to_fulfill.push_back(e_path);
		std::vector<GOAPAction*> path;
		tmp_paths.push_back(path);
		FillEffectsFromGoal(goal_to_complete);
		calculating_path = true;
	}

	if (calculating_path)
	{
		int iterations = 0;
		int completed = 0;
		for (int p = 0; p < possible_paths && iterations < MAX_ITERATIONS; p++)
		{
			if (!completed_paths[p])
			{
				while (iterations < MAX_ITERATIONS && !effects_to_fulfill[p].empty())
				{
					//the effect to fulfill
					GOAPEffect* effect = effects_to_fulfill[p].front();

					//find an action that fulfill this effect
					GOAPAction* action = nullptr;
					for (int a = 0; a < actions.size(); ++a)
					{
						for (int e = 0; e < actions[a]->GetNumEffects(); ++e) //check that each action effects
						{
							GOAPEffect* action_effect = actions[a]->GetEffect(e);
							std::string cmp_name = effect->GetName();
							if (effect->GetType() == action_effect->GetType() && effect->GetEffect() == action_effect->GetEffect() ///possible bug
								&& cmp_name == action_effect->GetName()) // first check if this effect fulfills the needs
							{
								if (effect->GetEffect() == GOAPEffect::E_DECREASE || effect->GetEffect() == GOAPEffect::E_INCREASE) //if it's an increase or decrease, the value doesn't matter
								{
									action = actions[a];
									break;
								}
								else //check the values
								{
									if (effect->GetEffect() == GOAPEffect::E_SET) // check they are equal
									{
										GOAPVariable::VariableType t = effect->GetType();
										bool set = false;
										switch (t)
										{
										case GOAPVariable::T_BOOL:
											if (effect->GetValueB() == action_effect->GetValueB())
												set = true;
											break;
										case GOAPVariable::T_FLOAT:
											if (effect->GetValueF() == action_effect->GetValueF())
												set = true;
											break;
										default:
											break;
										}
										if (set)
										{
											action = actions[a];
											break;
										}
									}
									else if (effect->GetEffect() == GOAPEffect::E_SET_DIFFERENT) //check they are different
									{
										GOAPVariable::VariableType t = effect->GetType();
										bool set = false;
										switch (t)
										{
										case GOAPVariable::T_BOOL:
											if (effect->GetValueB() != action_effect->GetValueB())
												set = true;
											break;
										case GOAPVariable::T_FLOAT:
											if (effect->GetValueF() != action_effect->GetValueF())
												set = true;
											break;
										default:
											break;
										}
										if (set)
										{
											action = actions[a];
											break;
										}
									}
								}
							}
						}

						if (action != nullptr)
							break;
					}

					//check if an action is found
					if (action == nullptr)
					{
						//this means there's no action that fulfills the effect, so this path is not valid. Remove it
						effects_to_fulfill[p].clear();
						tmp_paths[p].clear();
						completed_paths[p] = true;
						completed++;
						continue;
					}
					else
					{
						//add this action to the path
						tmp_paths[p].push_back(action);
					}

					//check if the action has any preconditions and add them in the effects to fulfill list
					for (int precon = 0; precon < action->GetNumPreconditions(); ++precon)
					{
						AddEffectsFormField(action->GetPrecondition(precon), p);
					}

					//remove the effect
					effects_to_fulfill[p].erase(effects_to_fulfill[p].begin());

					iterations++;
				}

				if (effects_to_fulfill[p].empty())
				{
					completed_paths[p] = true;
					std::reverse(std::begin(tmp_paths[p]), std::end(tmp_paths[p]));
					completed++;
				}
			}
			else completed++;

			if (completed == possible_paths)
			{
				calculating_path = false;
				int best_path = GetBestTmpPath();
				path = tmp_paths[best_path];
				path_valid = true;
				//reset all variables
				ResetVariables();
			}
		}
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

void ComponentGOAPAgent::ResetTmpEffects()
{
	for (std::vector<GOAPEffect*>::iterator it = created_effects.begin(); it != created_effects.end();)
	{
		RELEASE(*it);
		it = created_effects.erase(it);
	}
}

GOAPEffect * ComponentGOAPAgent::TmpEffectsContains(const char * name, GOAPEffect::EffectType t) const
{
	for (std::vector<GOAPEffect*>::const_iterator it = created_effects.begin(); it != created_effects.end();++it)
	{
		if ((*it)->GetName() == name && (*it)->GetEffect() == t)
		{
			return (*it);
		}
	}
	return nullptr;
}

void ComponentGOAPAgent::FillEffectsFromGoal(GOAPGoal * goal)
{
	//Get the condition to compare
	GOAPField* condition = goal->GetCondition();
	
	//check depending on type
	GOAPVariable::VariableType type = condition->GetType();
	GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
	switch (type)
	{
	case GOAPVariable::T_BOOL:
	{	//bools can be compared: equal or different
		bool condition_var = condition->GetValueB();
		bool blackboard_var;
		if (GetBlackboardVariable(condition->GetName(), blackboard_var))
		{
			switch (cm)
			{
			case GOAPField::CM_EQUAL: //check if the variable is different, otherwise this is already fulfilled
			{
				if (blackboard_var != condition_var)
				{
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueB() != condition->GetValueB())
						{
							effect = new GOAPEffect(condition->GetName(), condition->GetValueB());
							created_effects.push_back(effect);
						}
					for (int p = 0; p < possible_paths; ++p) //add the effect to all posible paths effects to fulfill
					{
						effects_to_fulfill[p].push_back(effect);
					}
				}
				break;
			}
			case GOAPField::CM_DIFFERENT://check if the variable is equal, otherwise this is already fulfilled
			{
				if (blackboard_var == condition_var)
					{
						GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
						if (effect == nullptr || effect->GetValueB() != condition->GetValueB())
						{
							effect = new GOAPEffect(condition->GetName(), condition->GetValueB());
							created_effects.push_back(effect);
						}
						for (int p = 0; p < possible_paths; ++p) //add the effect to all posible paths effects to fulfill
						{
							effects_to_fulfill[p].push_back(effect);
						}
					}
				break;
			}
			}
		}
		else
		{
			CONSOLE_ERROR("Blackboard doesn't contain the variable %s", condition->GetName());
		}
		break;

	}
	case GOAPVariable::T_FLOAT:
	{
		float condition_var = condition->GetValueF();
		float blackboard_var;
		if (GetBlackboardVariable(condition->GetName(), blackboard_var))
		{
			switch (cm)
			{
			case GOAPField::CM_EQUAL: //check if the variable is different, otherwise this is already fulfilled
			{
				if (blackboard_var != condition_var)
				{
					//check the possible ways the condition can be fulfilled, if there's more than one way, different possible paths will be created				
					int new_pos_path = AddPossiblePath(0);

					///An action that sets the value to the contition_var will fulfill it
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);

					if (blackboard_var < condition_var) // if the bb_var is lower, find an increment effect
					{
						effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
						if (effect == nullptr)
						{
							effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
							created_effects.push_back(effect);
						}
						effects_to_fulfill[new_pos_path].push_back(effect);
					}
					else // if the bb_var is higher, find a decrement effect
					{
						effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
						if (effect == nullptr)
						{
							effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
							created_effects.push_back(effect);
						}
						effects_to_fulfill[new_pos_path].push_back(effect);
					}
				}
				break;
			}
			case GOAPField::CM_DIFFERENT: //check if the variables are equal, otherwise this is already fulfilled
			{
				if (blackboard_var == condition_var)
				{
					// check ways of fulfilling the goal: set_different, increase, decrease
					int increase_path = AddPossiblePath(0);
					int decrease_path = AddPossiblePath(0);

					/// a set to a different value will fulfill the condition
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET_DIFFERENT);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET_DIFFERENT, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);

					///an increse also fulfill the condition
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[increase_path].push_back(effect);

					/// same for decrease
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[decrease_path].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_HIGHER: //check if the variable is lower or equal, otherwise this is already fulfilled
			{
				if (blackboard_var <= condition_var)
				{
					//only increasing values will be tanken in acount here
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_LOWER: //check if the variable is higher or equal, otherwise this is already fulfilled
			{
				if (blackboard_var >= condition_var)
				{
					//only decreasing values will be tanken in acount here
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_HIGHER_OR_EQUAL://check if the variable is lower, otherwise this is already fulfilled
			{
				if (blackboard_var < condition_var)
				{
					//two possible solutions: set to the value or increase
					int increase_path = AddPossiblePath(0);

					/// set to the value
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);

					/// increase
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[increase_path].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_LOWER_OR_EQUAL:
			{
				if (blackboard_var > condition_var)
				{
					//two possible effects: set to the value or decrease
					int decrease_path = AddPossiblePath(0);

					/// set to the value
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[0].push_back(effect);

					/// decrease
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[decrease_path].push_back(effect);
				}
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		break;
	}
	
}

void ComponentGOAPAgent::AddEffectsFormField(GOAPField * condition, int path_index)
{
	//check depending on type
	GOAPVariable::VariableType type = condition->GetType();
	GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
	switch (type)
	{
	case GOAPVariable::T_BOOL:
	{	//bools can be compared: equal or different
		bool condition_var = condition->GetValueB();
		bool blackboard_var;
		if (GetBlackboardVariable(condition->GetName(), blackboard_var))
		{
			switch (cm)
			{
			case GOAPField::CM_EQUAL: //check if the variable is different, otherwise this is already fulfilled
			{
				if (blackboard_var != condition_var)
				{
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET); //bool values are always set effects
					if (effect == nullptr || effect->GetValueB() != condition->GetValueB())
					{
						effect = new GOAPEffect(condition->GetName(), condition->GetValueB());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);
					
				}
				break;
			}
			case GOAPField::CM_DIFFERENT://check if the variable is equal, otherwise this is already fulfilled
			{
				if (blackboard_var == condition_var)
				{
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueB() != condition->GetValueB())
					{
						effect = new GOAPEffect(condition->GetName(), condition->GetValueB());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);
				}
				break;
			}
			}
		}
		else
		{
			CONSOLE_ERROR("Blackboard doesn't contain the variable %s", condition->GetName());
		}
		break;

	}
	case GOAPVariable::T_FLOAT:
	{
		float condition_var = condition->GetValueF();
		float blackboard_var;
		if (GetBlackboardVariable(condition->GetName(), blackboard_var))
		{
			switch (cm)
			{
			case GOAPField::CM_EQUAL: //check if the variable is different, otherwise this is already fulfilled
			{
				if (blackboard_var != condition_var)
				{
					//check the possible ways the condition can be fulfilled, if there's more than one way, different possible paths will be created				
					int new_pos_path = AddPossiblePath(path_index);

					///An action that sets the value to the contition_var will fulfill it
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);

					if (blackboard_var < condition_var) // if the bb_var is lower, find an increment effect
					{
						effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
						if (effect == nullptr)
						{
							effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
							created_effects.push_back(effect);
						}
						effects_to_fulfill[new_pos_path].push_back(effect);
					}
					else // if the bb_var is higher, find a decrement effect
					{
						effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
						if (effect == nullptr)
						{
							effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
							created_effects.push_back(effect);
						}
						effects_to_fulfill[new_pos_path].push_back(effect);
					}
				}
				break;
			}
			case GOAPField::CM_DIFFERENT: //check if the variables are equal, otherwise this is already fulfilled
			{
				if (blackboard_var == condition_var)
				{
					// check ways of fulfilling the goal: set_different, increase, decrease
					int increase_path = AddPossiblePath(path_index);
					int decrease_path = AddPossiblePath(path_index);

					/// a set to a different value will fulfill the condition
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET_DIFFERENT);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET_DIFFERENT, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);

					///an increse also fulfill the condition
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[increase_path].push_back(effect);

					/// same for decrease
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[decrease_path].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_HIGHER: //check if the variable is lower or equal, otherwise this is already fulfilled
			{
				if (blackboard_var <= condition_var)
				{
					//only increasing values will be tanken in acount here
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_LOWER: //check if the variable is higher or equal, otherwise this is already fulfilled
			{
				if (blackboard_var >= condition_var)
				{
					//only decreasing values will be tanken in acount here
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_HIGHER_OR_EQUAL://check if the variable is lower, otherwise this is already fulfilled
			{
				if (blackboard_var < condition_var)
				{
					//two possible solutions: set to the value or increase
					int increase_path = AddPossiblePath(path_index);

					/// set to the value
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);

					/// increase
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_INCREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_INCREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[increase_path].push_back(effect);
				}
				break;
			}
			case GOAPField::CM_LOWER_OR_EQUAL:
			{
				if (blackboard_var > condition_var)
				{
					//two possible effects: set to the value or decrease
					int decrease_path = AddPossiblePath(path_index);

					/// set to the value
					GOAPEffect* effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_SET);
					if (effect == nullptr || effect->GetValueF() != condition->GetValueF())
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_SET, condition->GetValueF());
						created_effects.push_back(effect);
					}
					effects_to_fulfill[path_index].push_back(effect);

					/// decrease
					effect = TmpEffectsContains(condition->GetName(), GOAPEffect::E_DECREASE);
					if (effect == nullptr)
					{
						effect = new GOAPEffect(condition->GetName(), GOAPEffect::E_DECREASE, 0.f); //the increment value doesn't matter
						created_effects.push_back(effect);
					}
					effects_to_fulfill[decrease_path].push_back(effect);
				}
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

int ComponentGOAPAgent::AddPossiblePath(int ref)
{
	int ret = possible_paths;

	std::vector < GOAPAction*> path = tmp_paths[ref];
	tmp_paths.push_back(path);

	std::vector<GOAPEffect*> e_path = effects_to_fulfill[ref];
	effects_to_fulfill.push_back(e_path);

	completed_paths.push_back(false);

	possible_paths++;

	return ret;
}

int ComponentGOAPAgent::GetBestTmpPath()
{
	int ret = 0;

	int lower_cost = 99999999999;

	for (int i = 0; i < possible_paths; ++i) 
	{
		if (tmp_paths[i].size()>0)
		{
			int path_cost = 0;

			for (int a = 0; a < tmp_paths[i].size(); ++a) //calculate path i cost
			{
				path_cost += tmp_paths[i][a]->GetCost();
			}

			if (path_cost < lower_cost)
			{
				lower_cost = path_cost;
				ret = i;
			}
		}
	}
	return ret;
}

void ComponentGOAPAgent::ResetVariables()
{
	ResetTmpEffects();
	for (int p = 0; p < possible_paths; ++p)
	{
		tmp_paths[p].clear();
	}
	tmp_paths.clear();

	for (int p = 0; p < possible_paths; ++p)
	{
		effects_to_fulfill[p].clear();
	}
	effects_to_fulfill.clear();

	for (int p = 0; p < possible_paths; ++p)
	{
		completed_paths[p] = false;
	}
	completed_paths.clear();
}

bool ComponentGOAPAgent::SystemFulfillCondition(GOAPField * condition)
{
	GOAPVariable::VariableType var_t = condition->GetType();

	switch (var_t)
	{
	case GOAPVariable::T_BOOL:
	{
		bool var;
		if (GetBlackboardVariable(condition->GetName(), var))
		{
			GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
			switch (cm)
			{
			case GOAPField::CM_EQUAL:
				if (var == condition->GetValueB())
				{
					return true;
				}
				break;
			case GOAPField::CM_DIFFERENT:
				if (var != condition->GetValueB())
				{
					return true;
				}
				break;
			default:
				break;
			}
		}
		break;
	}
	case GOAPVariable::T_FLOAT:
	{
		float var;
		if (GetBlackboardVariable(condition->GetName(), var))
		{
			GOAPField::ComparisonMethod cm = condition->GetComparisonMethod();
			switch (cm)
			{
			case GOAPField::CM_EQUAL:
				if (var == condition->GetValueF())
				{
					return true;
				}
				break;
			case GOAPField::CM_DIFFERENT:
				if (var != condition->GetValueF())
				{
					return true;
				}
				break;
			case GOAPField::CM_HIGHER:
				if (var > condition->GetValueF())
				{
					return true;
				}
				break;
			case GOAPField::CM_LOWER:
				if (var < condition->GetValueF())
				{
					return true;
				}
				break;
			case GOAPField::CM_HIGHER_OR_EQUAL:
				if (var >= condition->GetValueF())
				{
					return true;
				}
				break;
			case GOAPField::CM_LOWER_OR_EQUAL:
				if (var <= condition->GetValueF())
				{
					return true;
				}
				break;
			}
		}
		break;
	}
	default:
		break;
	}
	return false;
}

void ComponentGOAPAgent::ApplyEffect(GOAPEffect * effect)
{
	GOAPEffect::EffectType type = effect->GetEffect();

	switch (type)
	{
	case GOAPEffect::E_DECREASE:
	{
		float curr_value;
		if (GetBlackboardVariable(effect->GetName(), curr_value))
		{
			curr_value -= effect->GetValueF();
			SetBlackboardVariable(effect->GetName(), curr_value);
		}
		break;
	}
	case GOAPEffect::E_SET:
	{
		switch (effect->GetType())
		{
		case GOAPVariable::T_BOOL:
			SetBlackboardVariable(effect->GetName(), effect->GetValueB());
			break;
		case GOAPVariable::T_FLOAT:
			SetBlackboardVariable(effect->GetName(), effect->GetValueF());
			break;
		default:
			break;
		}
		
		break;
	}
	case GOAPEffect::E_SET_DIFFERENT:
		//no custom effects can do this
		break;
	case GOAPEffect::E_INCREASE:
	{
		float curr_value;
		if (GetBlackboardVariable(effect->GetName(), curr_value))
		{
			curr_value += effect->GetValueF();
			SetBlackboardVariable(effect->GetName(), curr_value);
		}
		break;
	}
	default:
		break;
	}
}
