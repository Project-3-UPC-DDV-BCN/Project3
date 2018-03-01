#include "GOAPEffect.h"
#include "Globals.h"

GOAPEffect::GOAPEffect(const char * name, EffectType effect, float value) : effect(effect)
{
	var = new GOAPVariable(name, value);
}

GOAPEffect::GOAPEffect(const char * name, bool value)
{
	var = new GOAPVariable(name, value);
}

GOAPEffect::~GOAPEffect()
{
	RELEASE(var);
}

GOAPEffect::EffectType GOAPEffect::GetEffect() const
{
	return effect;
}

float GOAPEffect::GetValue() const
{
	return var->GetValue();
}

bool GOAPEffect::GetValue()
{
	return var->GetValue();
}

GOAPVariable::VariableType GOAPEffect::GetType() const
{
	return var->GetType();
}

const char * GOAPEffect::GetName() const
{
	return var->GetName();
}
