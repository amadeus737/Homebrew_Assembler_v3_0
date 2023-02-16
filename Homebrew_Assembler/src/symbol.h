#pragma once

#include "symbolConfig.h"

#include <string>

class symbol
{
public:
	virtual ~symbol() {};
	virtual void process(class assembler& a, const std::string& d, std::string remainder, int line) const = 0;
	virtual void process(class assembler& a, const std::string& d, std::string& label, std::string remainder, int line) const
	{
		throw std::exception((std::string("Symbol [.") + d + "] does not support labels [" + label + "]").c_str());
	}
};

class symbolAlias : public symbol
{
public:
	symbolAlias(const symbol* s)
		:
		_symbol(s)
	{}

	virtual void process(class assembler& a, const std::string& d, std::string r, int l) const override
	{
		_symbol->process(a, d, r, l);
	}

private:
	const symbol* const _symbol;
};
