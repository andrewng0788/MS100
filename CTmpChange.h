#pragma once
template<class Type>
class CTmpChange: public CObject
{
public:
	CTmpChange(Type *var, Type value)
	{
		this->saveValue = *var;
		this->var = var;
		*var = value;
	};
	~CTmpChange()
	{
		*var = saveValue;
	}
	Type saveValue;
	Type *var;
};