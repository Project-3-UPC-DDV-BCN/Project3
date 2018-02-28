#include "GOAPGoal.h"
#include "Data.h"


GOAPGoal::GOAPGoal()
{
}

GOAPGoal::~GOAPGoal()
{
}

void GOAPGoal::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("name", GetName());
	data.AddUInt("UUID", GetUID());
	data.AddUInt("priority", priority);
	data.AddUInt("increment_rate", increment_rate);
	data.AddFloat("increment_time", increment_time);
	data.AddInt("conditions_num", conditions.size());	
	for (int i = 0; i < conditions.size(); ++i)
	{
		data.CreateSection("condition_" + std::to_string(i));
		data.AddInt("type", conditions[i]->GetType());
		data.AddInt("comparison", conditions[i]->GetComparisonMethod());
		data.AddString("name", conditions[i]->GetName());
		switch (conditions[i]->GetType())
		{
		case GOAPVariable::T_BOOL:
			data.AddBool("value", conditions[i]->GetValue());
			break;
		case GOAPVariable::T_FLOAT:
			data.AddFloat("value", conditions[i]->GetValue());
			break;
		default:
			break;
		}
		data.CloseSection();
	}
}

bool GOAPGoal::Load(Data & data)
{
	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("name"));
	priority = data.GetUInt("priority");
	increment_rate = data.GetUInt("increment_rate");
	increment_time = data.GetFloat("increment_time");
	int conditions_num = data.GetInt("conditions_num");
	for (int i = 0; i < conditions_num; ++i)
	{
		data.EnterSection("condition_" + std::to_string(i));
		GOAPVariable::VariableType type = (GOAPVariable::VariableType)data.GetInt("type");
		GOAPField::ComparisonMethod cm = (GOAPField::ComparisonMethod)data.GetInt("comparison");
		std::string name = data.GetString("name");
		switch (type)
		{
		case GOAPVariable::T_NULL:
			break;
		case GOAPVariable::T_BOOL:
			AddCondition(name, cm, data.GetBool("value"));
			break;
		case GOAPVariable::T_FLOAT:
			AddCondition(name, cm, data.GetFloat("value"));
			break;
		default:
			break;
		}
		data.LeaveSection();
	}
	return true;
}

void GOAPGoal::CreateMeta() const
{
}

void GOAPGoal::LoadToMemory()
{
}

void GOAPGoal::UnloadFromMemory()
{
}

void GOAPGoal::SetPriority(uint priority)
{
	this->priority = priority;
}

void GOAPGoal::SetIncrement(int increment_rate, float time_step)
{
	this->increment_rate = increment_rate;
	increment_time = time_step;
}

void GOAPGoal::AddCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, bool value)
{
	conditions.push_back(new GOAPField(name.c_str(), comparison_method, value));
}

void GOAPGoal::AddCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, float value)
{
	conditions.push_back(new GOAPField(name.c_str(), comparison_method, value));
}

const std::vector<GOAPField*>& GOAPGoal::GetConditions() const
{
	return conditions;
}

uint GOAPGoal::GetPriority() const
{
	return priority;
}

uint GOAPGoal::GetIncrementRate() const
{
	return increment_rate;
}

float GOAPGoal::GetIncrementTime() const
{
	return increment_time;
}

bool GOAPGoal::NeedIncrement()
{
	bool ret = false;

	if (timer.Read() - last_check > increment_time)
	{
		ret = true;
		last_check = timer.Read();
	}

	return ret;
}

void GOAPGoal::StartTimer()
{
	timer.Start();
}
