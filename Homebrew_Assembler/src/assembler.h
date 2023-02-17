#pragma once

#include "command.h"
#include "opcode.h"
#include "symbol.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <optional>

class fileStackEntry
{
public:
	std::string filename;
	int parentIndex = -1;
	int startLine = 0;
};

class assembler
{
public:
	// Constructors
	assembler(std::string filename);

	// Echo stuff
	void setEcho(unsigned char e);
	bool echoArchitecture() { return _echo_architecture; }
	bool echoMajorTasks() { return _echo_major_tasks; }
	bool echoMinorTasks() { return _echo_minor_tasks; }
	bool echoWarnings() { return _echo_warnings; }
	bool echoSource() { return _echo_source; }
	bool echoParsedMajor() { return _echo_parsed_major; }
	bool echoParsedMinor() { return _echo_parsed_minor; }
	bool echoRomData() { return _echo_rom_data; }

	// start assembly
	void assemble();

	// source file handling
	void pushFile(std::string filename);
	void processFile();
	void processLine(std::string& line, int linenum, std::optional<std::string> token);

	// addressing stuff
	void setAddress(int a) { _address = a; 	if (_address > _max_address) _max_address = _address;
	}
	int getAddress() const { return _address; }

	// general stuff
	void setInstructionWidth(int i) { _instructionWidth = i; }
	void setAddressWidth(int a) { _addressWidth = a; }
	int getInstructionWidth() { return _instructionWidth; }
	int getAddressWidth() { return _addressWidth; }

	// Symbol stuff
	SymbolType getSymbolType(const std::string& n);
	int getSymbolAddress(const std::string& n) const;
	void addLabel(const std::string& n, int a, int l);
	void addConstant(const std::string& n, int a, int l);
	void addVariable(const std::string& n, int a, int l);
	void addFlag(const std::string& n, int a, int l);
	void addRegister(const std::string& n, int a, int l);
	void addControlLine(const std::string& n, int a, int l);
	void addOpcode(int v, const opcode& oc);
	void addOpcodeAlias(int v, const opcode& oca);
	void addNewControlPatternToCurrentOpcode(controlPattern cp);
	void addToLastControlPatternInCurrentOpcode(controlPattern cp);

	// Flag stuff
	int getFlagCount() { return _nFlags; }
	const std::vector<int>& getSymbolAddresses(SymbolType t);
	std::vector<int> lastAddedFlags;

	// Opcode stuff
	bool isAMnemonic(const std::string& s);
	int getValueByUniqueOpcodeString(const std::string& s);
	int getValueByUniqueOpcodeAliasString(const std::string& s);
	int numOpcodeCycles();
	int lastOpcodeIndex();
	opcode& getOpcode(int v);

	// Decoder Rom stuff
	void addDecoderRom(bool write, int inputs, int outputs);
	void writeDecoderRom();

	// ProgramRom stuff
	void setActiveSegment(int i) { _activeSegmentIndex = i; }
	void addProgramRom(bool write, int inputs, int outputs);
	void addByteToProgramRom(int8_t byte, int address = -1);
	void writeProgramRom();

private:
	// used to link parse tokens with specific functions defined in:
	//  directiveDefine.h, archDefine.h, and instructionDefine.h
	void registerOperations();

	// Used for linking include files
	void popFile();

	void pass0();
	void pass1();

	template <class d>
	void registerDirective(std::string name)
	{
		assert(_directives.count(name) == 0);
		_directives.emplace(name, std::make_unique<d>());
	}

	template <class a>
	void registerArchTag(std::string name)
	{
		assert(_archtags.count(name) == 0);
		_archtags.emplace(name, std::make_unique<a>());
	}

	template <class i>
	void registerInstruction(std::string name)
	{
		assert(_instructions.count(name) == 0);
		_instructions.emplace(name, std::make_unique<i>());
	}

private:
	// file stuff
	std::ifstream _file;
	std::string _startFile;
	std::vector<fileStackEntry> _fileStack;
	int _fileStackIndex = -1;
	int _lastFileStackIndex = -1;
	int _lineNumber = 0;

	// general stuff
	int _instructionWidth = 0;
	int _addressWidth = 0;

	// echo stuff
	bool _echo_architecture = false;
	bool _echo_major_tasks = false;
	bool _echo_minor_tasks = false;
	bool _echo_warnings = false;
	bool _echo_source = false;
	bool _echo_parsed_major = false;
	bool _echo_parsed_minor = false;
	bool _echo_rom_data = false;

	// Flag stuff
	int _nFlags = 0;

	// Symbol stuff
	std::map<std::string, symbol> _symbols;
	std::vector<int> _constantAddresses;
	std::vector<int> _variableAddresses;
	std::vector<int> _labelAddresses;
	std::vector<int> _registerAddresses;
	std::vector<int> _flagAddresses;
	std::vector<int> _controlLineAddresses;

	// Opcode stuff
	std::map<int, opcode> _opcodes;
	std::map<int, opcode> _opcode_aliases;
	std::vector<std::string> _mnemonics;
	int _lastOpcodeIndex = -1;

	// Token identifier stuff
	std::map<std::string, std::unique_ptr<command>>  _directives;
	std::map<std::string, std::unique_ptr<command>>	 _archtags;
	std::map<std::string, std::unique_ptr<command>> _instructions;

	// addressing stuff
	int _address = 0;
	int _last_address = -1;
	int _max_address = 0;

	// decode rom stuff
	bool _write_decode_rom = false;
	int _maxControlLineValue = -1;
	int _maxOpcodeValue = -1;
	int _maxNumCycles = -1;
	int _in_bits_decode;
	int _out_bits_decode;

	// program rom stuff
	int _activeSegmentIndex;
	bool _write_program_rom = false;
	int _in_bits_program;
	int _out_bits_program;
};