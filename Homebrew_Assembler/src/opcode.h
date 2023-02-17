#pragma once

#include <iostream>
#include <vector>
#include <string>

enum class ArgType { None, Register, Numeral, Ascii, DerefReg, DerefNum, DerefAscii };
enum class PatternType { None, Seq, Seq_If, Seq_Else };

class controlPattern
{
public:
	int pattern;
	std::vector<int> flags;
	PatternType type;
};

class controlPatterns
{
public:
	controlPattern cpattern[2];
	int count;
};

class opcode
{
public:
	// Used to specify opcode arguments
	class arg
	{
	public:
		ArgType _type;
		std::string _string;
	};

	opcode()
	{
		_mnemonic = "";
		_value = -1;
		_controlPatterns.clear();
		_arguments.clear();
	}

	void setMnemonic(const std::string& s) { _mnemonic = s; }
	void setValue(const int& v) { _value = v; }

	void addNewControlPattern(const controlPattern& p)
	{
		std::vector<int> flags;
		for (int i = 0; i < p.flags.size(); i++)
		{
			flags.push_back(p.flags[i]);
		}

		controlPatterns cp;
		cp.count = 1;
		cp.cpattern[0].pattern = p.pattern;
		cp.cpattern[0].type = p.type;
		cp.cpattern[0].flags = flags;

		_controlPatterns.push_back(cp);
	}

	void addToLastControlPattern(const controlPattern& p)
	{
		if (_controlPatterns.size() <= 0) return;

		std::vector<int> flags;
		for (int i = 0; i < p.flags.size(); i++)
		{
			flags.push_back(p.flags[i]);
		}

		controlPatterns cp = _controlPatterns[_controlPatterns.size() - 1];
		cp.count++;
		cp.cpattern[cp.count - 1].pattern = p.pattern;
		cp.cpattern[cp.count - 1].type = p.type;
		cp.cpattern[cp.count - 1].flags = flags;

		_controlPatterns[_controlPatterns.size() - 1] = cp;
	}

	void addArgument(arg a) { _arguments.push_back(a); }

	std::string mnemonic() { return _mnemonic; }
	int value() { return _value; }
	int numArgs() { return _arguments.size(); }
	arg getArg(int i) { return _arguments[i]; }
	int numCycles() { return _controlPatterns.size(); }
	controlPatterns& getPatterns(int i) { return _controlPatterns[i]; }

	controlPattern getPattern(int i, int j) { return _controlPatterns[i].cpattern[j]; }

	std::string getUniqueString()
	{
		std::string unique_str = _mnemonic + "_";

		for (int i = 0; i < _arguments.size(); i++)
		{
			switch (_arguments[i]._type)
			{
			case ArgType::Register:
			case ArgType::DerefReg:
				unique_str += _arguments[i]._string + "_";
				break;

			case ArgType::Numeral:
				unique_str += "#_";
				break;

			case ArgType::DerefNum:
				unique_str += "[#]_";
				break;

			case ArgType::Ascii:
				unique_str += "ASCII_";
				break;

			case ArgType::DerefAscii:
				unique_str += "[ASCII]_";
				break;
			}
		}

		// Erase the '_' character on the end
		unique_str = std::string(unique_str.begin(), unique_str.end() - 1);

		return unique_str;
	}

private:
	std::string _mnemonic;
	int _value;
	std::vector<controlPatterns> _controlPatterns;
	std::vector<arg> _arguments;
};