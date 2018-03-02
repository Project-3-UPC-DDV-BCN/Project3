#ifndef __GOAPEffect__
#define __GOAPEffect__

#include "GOAPVariable.h"

class GOAPEffect
{
public:
	enum EffectType
	{
		E_NULL,
		E_DECREASE,
		E_SET,
		E_SET_DIFFERENT, //internal use only
		E_INCREASE,
	};

public:
	GOAPEffect(const char* name, EffectType effect, float value);
	GOAPEffect(const char* name, bool value);
	~GOAPEffect();

	EffectType GetEffect()const;
	float GetValue()const;
	bool GetValue();

	GOAPVariable::VariableType GetType()const;

	const char* GetName()const;

private:
	GOAPVariable* var = nullptr;
	EffectType effect;
};

#endif // !__GOAPEffect__

