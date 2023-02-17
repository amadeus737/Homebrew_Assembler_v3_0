#include "assembler.h"
#include "parser.h"
#include "config.h"
#include "directive.h"
#include "archtag.h"
#include "instruction.h"

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

	registerArchTag<archBitWidth>(INSTRUCTION_WIDTH_STR);
	registerArchTag<archBitWidth>(ADDRESS_WIDTH_STR);
	registerArchTag<archRom>(DECODER_ROM_STR);
	registerArchTag<archRom>(PROGRAM_ROM_STR);
	registerArchTag<archRegister>(REGISTER_STR);
	registerArchTag<archFlagDevice>(FLAG_STR);
	registerArchTag<archFlagDevice>(DEVICE_STR);
	registerArchTag<archControlLine>(CONTROL_STR);
	registerArchTag<archOpcode>(OPCODE_STR);
	registerArchTag<archOpcode>(OPCODE_ALIAS_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_IF_STR);
	registerArchTag<archOpcodeSeq>(OPCODE_SEQ_ELSE_STR);

	registerInstruction<opcodeInstruction>(OPCODE_STR);
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

	if (parser::instance().is_command(token.value()))
	{
	/*	if (_symbols.count(token.value()) > 0)
		{
			std::stringstream msg;
			msg << "Found a symbol at line <" << linenum << ">! Symbol [" << token.value() << "] already exists!";
			throw std::exception(msg.str().c_str());
		}
		else*/
		{
			if (_archtags.count(token.value()) > 0)
			{
				_archtags[token.value()]->process(*this, token.value(), line, linenum);
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

SymbolType assembler::getSymbolType(const std::string& n)
{
	std::map<std::string, symbol>::iterator i = _symbols.find(n);

	if (i != _symbols.end())
		return (i->second).getType();

	return SymbolType::None;
}

int assembler::getSymbolAddress(const std::string& n) const
{
	auto i = _symbols.find(n);
	return (i->second).getAddress();
}

const std::vector<int>& assembler::getSymbolAddresses(SymbolType t)
{
	switch (t)
	{
	case SymbolType::Constant:
		return _constantAddresses;

	case SymbolType::Variable:
		return _variableAddresses;

	case SymbolType::Label:
		return _labelAddresses;

	case SymbolType::Register:
		return _registerAddresses;

	case SymbolType::Flag:
		return _flagAddresses;

	case SymbolType::ControlLine:
		return _controlLineAddresses;
	}
}

void assembler::addConstant(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeConstant(n, a, l));
	_constantAddresses.push_back(a);
}

void assembler::addVariable(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeVariable(n, a, l));
	_variableAddresses.push_back(a);
}

void assembler::addLabel(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeLabel(n, a, l));
	_labelAddresses.push_back(a);
}

void assembler::addRegister(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeRegister(n, a, l));
	_registerAddresses.push_back(a);
}

void assembler::addFlag(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeFlag(n, a, l));
	_nFlags++;

	_flagAddresses.push_back(a);
}

void assembler::addControlLine(const std::string& n, int a, int l)
{
	_symbols.emplace(n, symbol::makeControlLine(n, a, l));

	_controlLineAddresses.push_back(a);

	if (a > _maxControlLineValue) _maxControlLineValue = a;
}

void assembler::addOpcode(int v, const opcode& oc)
{
	_lastOpcodeIndex = v;
	_opcodes.emplace(v, oc);

	if (v > _maxOpcodeValue) _maxOpcodeValue = v;

	_mnemonics.push_back(_opcodes[v].mnemonic());
	//registerInstruction<archOpcode>(_opcodes[v].getUniqueString());
}

void assembler::addOpcodeAlias(int v, const opcode& oca)
{
	_opcode_aliases.emplace(v, oca);

	_mnemonics.push_back(_opcode_aliases[v].mnemonic());
	//registerInstruction<archOpcode>(_opcode_aliases[v].getUniqueString());
}

void assembler::addNewControlPatternToCurrentOpcode(controlPattern cp)
{
	_opcodes[_lastOpcodeIndex].addNewControlPattern(cp);
	if (_opcodes[_lastOpcodeIndex].numCycles() > _maxNumCycles) _maxNumCycles = _opcodes[_lastOpcodeIndex].numCycles();
}

void assembler::addToLastControlPatternInCurrentOpcode(controlPattern cp)
{
	_opcodes[_lastOpcodeIndex].addToLastControlPattern(cp);
}

bool assembler::isAMnemonic(const std::string& s)
{
	return std::find(_mnemonics.begin(), _mnemonics.end(), s) != _mnemonics.end();
}

opcode& assembler::getOpcode(int v)
{
	return _opcodes[v];
}

int assembler::getValueByUniqueOpcodeString(const std::string& m)
{
	for (auto it = _opcodes.begin(); it != _opcodes.end(); ++it)
		if (it->second.getUniqueString() == m)
			return it->first;
}

int assembler::getValueByUniqueOpcodeAliasString(const std::string& m)
{
	for (auto it = _opcode_aliases.begin(); it != _opcode_aliases.end(); ++it)
		if (it->second.getUniqueString() == m)
			return it->first;
}

int assembler::numOpcodeCycles()
{
	return _opcodes[_lastOpcodeIndex].numArgs();
}

int assembler::lastOpcodeIndex()
{
	return _lastOpcodeIndex;
}

void assembler::addDecoderRom(bool write, int inputs, int outputs)
{
	_write_decode_rom = write;
	_in_bits_decode = inputs;
	_out_bits_decode = outputs;
}

void assembler::addProgramRom(bool write, int inputs, int outputs)
{
	int size = outputs * pow(2, inputs);

	_write_program_rom = write;
	_in_bits_program = inputs;
	_out_bits_program = outputs;
}

void assembler::addByteToProgramRom(int8_t byte, int address)
{
	// fill in later!
}