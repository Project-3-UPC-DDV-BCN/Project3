#ifndef __GOAPField__
#define __GOAPField__

class GOAPField
{
public:
	enum GOAPFieldType
	{
		T_NULL,
		T_BOOL,
		T_NUMBER
	};

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
	GOAPField(GOAPFieldType t, ComparisonMethod cm, bool b);
	GOAPField(GOAPFieldType t, ComparisonMethod cm, float num);
	~GOAPField();

	//Returns the value of the field
	bool GetValue();
	//Returns the value of the field
	float GetValue() const;

	//Returns the type of the field
	GOAPFieldType GetType() const;

	//Returns the comparison method for the field
	ComparisonMethod GetComparisonMethod() const;

private:
	union Value
	{
		bool b_value;
		float num_value;
	} value;

	GOAPFieldType type = T_NULL;
	ComparisonMethod comp_method = CM_NULL;
};

#endif