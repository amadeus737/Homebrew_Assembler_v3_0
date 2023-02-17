#pragma once

#include <string>

enum class SymbolType { None, Constant, Variable, Label, Register, Flag, ControlLine };

// In my homebrew assembler, a symbol is any non-whitespace token that is not an instruction.
// This class essentially links the symbol string tokens with other useful information, like:
//  - the symbol type
//  - an integer value
//  - the line the symbol was found on
class symbol
{
public:
	symbol(symbol&& donor) noexcept
		:
		_name(std::move(donor._name)),
		_type(donor._type),
		_address(donor._address),
		_line(donor._line)
	{}

	// These just access and return the private members of the class
	const std::string& getName() const { return _name; }
	int getAddress() const { return _address; }
	SymbolType getType() const { return _type; }
	int getLine() const { return _line; }

	static symbol makeConstant(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::Constant, a, l);
	}

	static symbol makeVariable(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::Variable, a, l);
	}

	static symbol makeLabel(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::Label, a, l);
	}

	static symbol makeRegister(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::Register, a, l);
	}

	static symbol makeFlag(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::Flag, a, l);
	}

	static symbol makeControlLine(const std::string& n, int a, int l)
	{
		return symbol(n, SymbolType::ControlLine, a, l);
	}

	bool operator<(const symbol& other) const { return _name < other._name; }
	bool operator==(const symbol& other) const { return _type == other._type && _name == other._name; }

private:
	symbol(const std::string& n, SymbolType t, int a, int l)
		:
		_name(n),
		_type(t),
		_address(a),
		_line(l)
	{}

private:
	std::string _name;
	SymbolType _type;
	int _address;
	int _line;
};