#ifndef __GOAPField__
#define __GOAPField__

#include "GOAPVariable.h"

class GOAPField
{
public:
	enum ComparisonMethod
	{
		CM_NULL,
		CM_EQUAL,
		CM_DIFFERENT,
		CM_HIGHER,
		CM_LOWER,
		CM_HIGHER_OR_EQUAL,
		CM_LOWER_OR_EQUAL
	};

public:
	GOAPField(const char* name, ComparisonMethod cm, bool b);
	GOAPField(const char* name, ComparisonMethod cm, float num);
	~GOAPField();

	//Returns the value of the field
	bool GetValueB();
	//Returns the value of the field
	float GetValueF() const;

	//Returns the type of the field
	GOAPVariable::VariableType GetType() const;

	//Returns the comparison method for the field
	ComparisonMethod GetComparisonMethod() const;

	//Get the field name
	const char* GetName()const;

private:
	
	GOAPVariable* var = nullptr;
	ComparisonMethod comp_method = CM_NULL;
};

#endif