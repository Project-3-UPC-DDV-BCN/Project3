#ifndef __GOAPAction__
#define __GOAPAction__

#include "Resource.h"
#include "GOAPField.h"
#include "GOAPVariable.h"
#include <vector>

class CSScript;

class GOAPAction : public Resource
{
public:
	GOAPAction(const char* name);
	~GOAPAction();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	// Adds a condition to the goal
	void AddPreCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, bool value);
	void AddPreCondition(std::string& name, GOAPField::ComparisonMethod comparison_method, float value);

	//Adds an effect
	void AddEffect(std::string& name, bool value);
	void AddEffect(std::string& name, float value);

private:
	std::vector<GOAPField*> preconditions;
	std::vector<GOAPVariable*> effects;

	CSScript* script = nullptr;
	
};

#endif // !__GOAPAction__
