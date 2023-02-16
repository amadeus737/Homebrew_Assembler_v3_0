#pragma once

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
	bool echoArchitecture();
	bool echoMajorTasks();
	bool echoMinorTasks();
	bool echoWarnings();
	bool echoSource();
	bool echoParsedMajor();
	bool echoParsedMinor();
	bool echoRomData();

	// start assembly
	void assemble();

	void pushFile(std::string filename);
	void processFile();
	void processLine(std::string& line, int linenum, std::optional<std::string> token);

	// addressing stuff
	void setAddress(int a) { _address = a; }
	void setInstructionWidth(int i) { _instructionWidth = i; }
	void setAddressWidth(int a) { _addressWidth = a; }

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
	void registerArchDefinition(std::string name)
	{
		assert(_archDefinitions.count(name) == 0);
		_archDefinitions.emplace(name, std::make_unique<a>());
	}


private:
	// file stuff
	std::ifstream _file;
	std::string _startFile;
	std::vector<fileStackEntry> _fileStack;
	int _fileStackIndex = -1;
	int _lastFileStackIndex = -1;
	bool _arch_defined = false;
	int _lineNumber = 0;
	
	// echo stuff
	bool _echo_architecture = false;
	bool _echo_major_tasks = false;
	bool _echo_minor_tasks = false;
	bool _echo_warnings = false;
	bool _echo_source = false;
	bool _echo_parsed_major = false;
	bool _echo_parsed_minor = false;
	bool _echo_rom_data = false;

	// Token identifier stuff
	std::map<std::string, std::unique_ptr<symbol>>   _directives;
	std::map<std::string, std::unique_ptr<symbol>>	 _archDefinitions;
	//std::map<std::string, std::unique_ptr<instruction>> _instructions;

	// addressing stuff
	int _address = 0;
	int _instructionWidth = 0;
	int _addressWidth = 0;
};