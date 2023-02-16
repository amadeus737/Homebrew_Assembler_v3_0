#include "assembler.h"
#include "parser.h"
#include "symbolConfig.h"
#include "directive.h"
#include "archDefinition.h"

#include <sstream>

assembler::assembler(std::string filename)
{
	// set file exceptions, save the start file, add to file stack
	_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	_startFile = filename;

	registerOperations();
}

void assembler::registerOperations()
{
	registerDirective<includeDirective>(INCLUDE_STR);
	registerDirective<originDirective>(ORIGIN_STR);

	registerArchDefinition<archBitWidth>(INSTRUCTION_WIDTH_STR);
	registerArchDefinition<archBitWidth>(ADDRESS_WIDTH_STR);
	registerArchDefinition<archRegister>(REGISTER_STR);
	registerArchDefinition<archFlagDevice>(FLAG_STR);
	registerArchDefinition<archFlagDevice>(DEVICE_STR);
	registerArchDefinition<archRom>(DECODER_ROM_STR);
	registerArchDefinition<archRom>(PROGRAM_ROM_STR);
}

void assembler::assemble()
{
	pushFile(_startFile);
	processFile();
	pass0();
}

void assembler::pushFile(std::string filename)
{
	fileStackEntry entry;
	entry.filename = filename;
	entry.parentIndex = _fileStackIndex;
	entry.startLine = 0;

	if (_fileStackIndex != -1)
		_fileStack[_fileStackIndex].startLine = _lineNumber + 1;

	_fileStack.push_back(entry);
	_fileStackIndex = _fileStack.size() - 1;
}

void assembler::processFile()
{
	if (_file.is_open())
		_file.close();

	_file.open(_fileStack[_fileStackIndex].filename);

	if (_echo_major_tasks)
		std::cout << "\nProcessing file : " << _fileStack[_fileStackIndex].filename << "\n\n";
}

void assembler::pass0()
{
	std::string line;
	_lineNumber = 0;

	try
	{
		while (std::getline(_file, line))
		{
			int startLine = _fileStack[_fileStackIndex].startLine;

			if (startLine < _lineNumber && _lastFileStackIndex != _fileStackIndex) _lineNumber = startLine;
			_lastFileStackIndex = _fileStackIndex;

			if (startLine != 0 && _lineNumber < startLine)
			{
				_lineNumber++;
				continue;
			}

			if (_echo_source)
				std::cout << "     ==> source line #" << _lineNumber << " = " << line << "\n";

			// remove any comments and extract token
			parser::instance().strip_comment(line);
			auto token = parser::instance().extract_token_ws(line);
			std::string tokenString = "";

			if (token.has_value())
				tokenString = std::string(token.value());
			
			// for first pass, only process include directives or arch definitions
			if (tokenString == ".include" || tokenString == REGISTER_STR || tokenString == FLAG_STR ||
				tokenString == DEVICE_STR || tokenString == CONTROL_STR || tokenString == OPCODE_STR ||
				tokenString == OPCODE_ALIAS_STR || tokenString == OPCODE_SEQ_STR ||
				tokenString == OPCODE_SEQ_IF_STR || tokenString == OPCODE_SEQ_ELSE_STR ||
				tokenString == END_ARCH_STR || tokenString == INSTRUCTION_WIDTH_STR ||
				tokenString == ADDRESS_WIDTH_STR || tokenString == PROGRAM_ROM_STR ||
				tokenString == DECODER_ROM_STR)
				processLine(line, _lineNumber, token);

			_lineNumber++;
		}
	}
	catch (const std::ifstream::failure& e)
	{
		if (!_file.eof())
		{
			throw e;
		}
		else
		{
			_file.close();
			_lastFileStackIndex = _fileStackIndex;

			int parentIndex = _fileStack[_fileStackIndex].parentIndex;
			if (_fileStackIndex >= 0 && parentIndex != -1)
			{
				_fileStackIndex = parentIndex;
				
				processFile();
				pass0();
			}
		}
	}
}

void assembler::processLine(std::string& line, int linenum, std::optional<std::string> token)
{
	std::string tokenString = std::string(token.value());

	// skip tagged tokens (start with '#')
	if (tokenString.front() == '#')
		return;

	// ignore braces
	if (tokenString.front() == '{' || tokenString.front() == '}')
	{
		if (tokenString.front() == '}' && _echo_parsed_major)
			std::cout << "\n";

		return;
	}

	if (parser::instance().is_symbol(token.value()))
	{
	/*	if (_symbols.count(token.value()) > 0)
		{
			std::stringstream msg;
			msg << "Found a symbol at line <" << linenum << ">! Symbol [" << token.value() << "] already exists!";
			throw std::exception(msg.str().c_str());
		}
		else*/
		{
			if (_archDefinitions.count(token.value()) > 0)
			{
				_archDefinitions[token.value()]->process(*this, token.value(), line, linenum);
			}
		}
	}
	else if (parser::instance().is_directive(token.value()))
	{
		// strip off the directive symbol
		token.value().erase(token.value().begin(), token.value().begin() + 1);

		// only handle registered directives
		if (_directives.count(token.value()) == 0)
		{
			std::stringstream msg;
			msg << "Unknown directive at line <" << _lineNumber << ">! Found [."
				<< token.value() << "]";
			throw std::exception(msg.str().c_str());
		}
		_directives[token.value()]->process(*this, token.value(), std::move(line), _lineNumber);
	}
}

void assembler::setEcho(unsigned char e)
{
	_echo_architecture = (e & 0x80) == 0x80; // $1000 0000
	_echo_major_tasks = (e & 0x40) == 0x40;  // $0100 0000
	_echo_minor_tasks = (e & 0x20) == 0x20;  // $0010 0000
	_echo_warnings = (e & 0x10) == 0x10;     // $0001 0000
	_echo_parsed_major = (e & 0x08) == 0x08; // $0000 1000
	_echo_parsed_minor = (e & 0x04) == 0x04; // $0000 0100
	_echo_source = (e & 0x02) == 0x02;       // $0000 0010
	_echo_rom_data = (e & 0x01) == 0x01;     // $0000 0001
}

bool assembler::echoArchitecture()
{
	return _echo_architecture;
}

bool assembler::echoMajorTasks()
{
	return _echo_major_tasks;
}

bool assembler::echoMinorTasks()
{
	return _echo_minor_tasks;
}

bool assembler::echoWarnings()
{
	return _echo_warnings;
}

bool assembler::echoSource()
{
	return _echo_source;
}

bool assembler::echoParsedMajor()
{
	return _echo_parsed_major;
}

bool assembler::echoParsedMinor()
{
	return _echo_parsed_minor;
}

bool assembler::echoRomData()
{
	return _echo_rom_data;
}
