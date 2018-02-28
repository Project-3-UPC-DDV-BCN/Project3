#include "GOAPVariable.h"

GOAPVariable::GOAPVariable(const char * name, bool b) : name(name), type(T_BOOL)
{
	value.b_value = b;
}

GOAPVariable::GOAPVariable(const char * name, float num) : name(name), type(T_FLOAT)
{
	value.num_value = num;
}

GOAPVariable::~GOAPVariable()
{
}

bool GOAPVariable::GetValue()
{
	return value.b_value;
}

float GOAPVariable::GetValue() const
{
	return value.num_value;
}

const char * GOAPVariable::GetName() const
{
	return name.c_str();
}

void GOAPVariable::SetValue(bool b)
{
	value.b_value = b;
}

void GOAPVariable::SetValue(float num)
{
	value.num_value = num;
}

GOAPVariable::VariableType GOAPVariable::GetType() const
{
	return type;
}
