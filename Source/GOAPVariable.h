#ifndef __GOAPVariable__
#define __GOAPVariable__

#include <string>

class GOAPVariable
{
public:
	enum VariableType
	{
		T_NULL,
		T_BOOL,
		T_FLOAT
	};

public:
	GOAPVariable(const char* name, bool b);
	GOAPVariable(const char* name, float num);
	~GOAPVariable();

	bool GetValueB();
	float GetValueF() const;
	const char* GetName() const;

	void SetValue(bool b);
	void SetValue(float num);

	VariableType GetType()const;

private:
	union Value
	{
		bool b_value;
		float num_value;
	} value;

	VariableType type = T_NULL;
	std::string name;
};


#endif // !__GOAPVariable__

