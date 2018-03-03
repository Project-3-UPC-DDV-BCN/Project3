#include "GOAPField.h"
#include "Globals.h"

GOAPField::GOAPField(const char* name, ComparisonMethod cm, bool b) : comp_method(cm)
{
	var = new GOAPVariable(name, b);
}

GOAPField::GOAPField(const char* name, ComparisonMethod cm, float num) : comp_method(cm)
{
	var = new GOAPVariable(name, num);
}

GOAPField::~GOAPField()
{
	RELEASE(var);
}

bool GOAPField::GetValueB()
{
	return var->GetValueB();
}

float GOAPField::GetValueF() const
{
	return var->GetValueF();
}

GOAPVariable::VariableType GOAPField::GetType() const
{
	return var->GetType();
}

GOAPField::ComparisonMethod GOAPField::GetComparisonMethod() const
{
	return comp_method;
}

const char * GOAPField::GetName() const
{
	return var->GetName();
}
