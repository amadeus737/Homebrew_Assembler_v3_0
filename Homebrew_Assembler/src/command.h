#pragma once

#include "config.h"
#include "util.h"

class command
{
public:
	virtual ~command() {};
	virtual void process(class assembler& a, const std::string& d, std::string remainder, int line) const = 0;
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

private:
	const command* const _command;
};
