#ifndef __GOAPGoal__
#define __GOAPGoal__

#include "Resource.h"
#include <vector>
#include "Timer.h"
#include "GOAPField.h"

class Data;

class GOAPGoal : public Resource
{
public:
	GOAPGoal();
	~GOAPGoal();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	// Set the priority value (1 to 100)
	void SetPriority(uint priority);
	// Define the increment rate and time step for the periodical increment
	void SetIncrement(int increment_rate, float time_step);

	// Adds a condition to the goal
	void AddCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, bool value);
	void AddCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, float value);

	const std::vector<GOAPField*>& GetConditions() const;
	uint GetPriority()const;
	uint GetIncrementRate()const;
	float GetIncrementTime()const;
	bool NeedIncrement();

	GOAPField* GetCondition(int index) const;
	int GetNumConditions()const;

	void StartTimer();

private:
	std::vector<GOAPField*> conditions;
	uint priority = 0; // form 1 to 100 -> 100 max priority
	uint increment_rate = 0;
	float increment_time = 0.0f; // 0, no increment
	Timer timer;
	float last_check = 0.0f;
};

#endif