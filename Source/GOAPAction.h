#ifndef __GOAPAction__
#define __GOAPAction__

#include "Resource.h"
#include "GOAPField.h"
#include "GOAPEffect.h"
#include <vector>

class CSScript;

class GOAPAction : public Resource
{
public:
	GOAPAction(const char* name, uint cost = 1);
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
	void AddEffect(std::string& name, GOAPEffect::EffectType effect, float value);

	//Get Effects
	int GetNumEffects() const;
	GOAPEffect* GetEffect(int index)const;

	//Get Precondition
	int GetNumPreconditions() const;
	GOAPField* GetPrecondition(int index) const;

	//Returns the cost of this action
	uint GetCost() const;

	//Calls script Update
	void Update();
	//Calls script Start
	void Start();
	//Calls script OnComplete
	void OnComplete();
	//Calls script OnFail()
	void OnFail();

	CSScript* GetScript()const;

private:
	std::vector<GOAPField*> preconditions;
	std::vector<GOAPEffect*> effects;
	uint cost = 0;
	CSScript* script = nullptr;
	
};

#endif // !__GOAPAction__
