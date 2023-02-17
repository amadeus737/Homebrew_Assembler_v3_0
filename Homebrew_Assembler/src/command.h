#pragma once

#include "config.h"
#include "util.h"

class command
{
public:
	virtual ~command() {};
	virtual void process(class assembler& a, const std::string& d, std::string remainder, int line) const {}
	virtual void process(class assembler& a, std::string& line, int value0, int value1, int startAddress) const {}
};

class commandAlias : public command
{
public:
	commandAlias(const command* c)
		:
		_command(c)
	{}

	virtual void process(class assembler& a, const std::string& d, std::string r, int l) const override
	{
		_command->process(a, d, r, l);
	}

	virtual void process(class assembler& a, std::string& il, int iv0, int iv1, int sa) const override
	{
		_command->process(a, il, iv0, iv1, sa);
	}

private:
	const command* const _command;
};
