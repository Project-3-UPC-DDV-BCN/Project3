#include "GOAPField.h"

GOAPField::GOAPField(GOAPFieldType t, ComparisonMethod cm, bool b) : type(t), comp_method(cm)
{
	value.b_value = b;
}

GOAPField::GOAPField(GOAPFieldType t, ComparisonMethod cm, float num) : type(t), comp_method(cm)
{
	value.num_value = num;
}

GOAPField::~GOAPField()
{
}

bool GOAPField::GetValue()
{
	return value.b_value;
}

float GOAPField::GetValue() const
{
	return value.num_value;
}

GOAPField::GOAPFieldType GOAPField::GetType() const
{
	return type;
}

GOAPField::ComparisonMethod GOAPField::GetComparisonMethod() const
{
	return comp_method;
}
