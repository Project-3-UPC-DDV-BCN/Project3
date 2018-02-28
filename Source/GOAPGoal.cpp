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
		switch (conditions[i]->GetType())
		{
		case GOAPField::T_BOOL:
			data.AddBool("value", conditions[i]->GetValue());
			break;
		case GOAPField::T_NUMBER:
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
		GOAPField::GOAPFieldType type = (GOAPField::GOAPFieldType)data.GetInt("type");
		GOAPField::ComparisonMethod cm = (GOAPField::ComparisonMethod)data.GetInt("comparison");
		switch (type)
		{
		case GOAPField::T_NULL:
			break;
		case GOAPField::T_BOOL:
			AddCondition(type, cm, data.GetBool("value"));
			break;
		case GOAPField::T_NUMBER:
			AddCondition(type, cm, data.GetFloat("value"));
			break;
		default:
			break;
		}
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

void GOAPGoal::AddCondition(GOAPField::GOAPFieldType value_type, GOAPField::ComparisonMethod comparison_method, bool value)
{
	conditions.push_back(new GOAPField(value_type, comparison_method, value));
}

void GOAPGoal::AddCondition(GOAPField::GOAPFieldType value_type, GOAPField::ComparisonMethod comparison_method, float value)
{
	conditions.push_back(new GOAPField(value_type, comparison_method, value));
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
