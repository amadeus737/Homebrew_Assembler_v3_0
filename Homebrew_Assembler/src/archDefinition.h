#pragma once

#include "assembler.h"
#include "symbol.h"
#include "parser.h"

#include <sstream>

class archBitWidth : public symbol
{
public:
	void process(assembler& assembler, const std::string& label, std::string remainder, int line) const override
	{
		auto sizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			if (label == INSTRUCTION_WIDTH_STR)
				std::cout << "          *** Instruction Width set to " << std::string(sizeToken.value()) << "\n\n";

			if (label == ADDRESS_WIDTH_STR)
				std::cout << "          *** Address Width set to " << std::string(sizeToken.value()) << "\n\n";
		}

		if (label == INSTRUCTION_WIDTH_STR)
			assembler.setInstructionWidth(stoi(std::string(sizeToken.value())));
		
		if (label == ADDRESS_WIDTH_STR)
			assembler.setAddressWidth(stoi(std::string(sizeToken.value())));
	}
};

class archRom : public symbol
{
public:
	void process(assembler& assembler, const std::string& label, std::string remainder, int line) const override
	{
		auto writeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!writeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid write token!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(writeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << writeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		auto inSizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!inSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid input size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(inSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << inSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		auto outSizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!outSizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid output size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(outSizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << outSizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}


		bool write = std::stoi(std::string(writeToken.value())) == 1;

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
		{
			if (label == DECODER_ROM_STR)
				std::cout << "          *** Decoder Rom with " << std::string(inSizeToken.value()) << " inputs and " << std::string(outSizeToken.value()) << " outputs (";
			
			if (label == PROGRAM_ROM_STR)
				std::cout << "          *** Program Rom with " << std::string(inSizeToken.value()) << " inputs and " << std::string(outSizeToken.value()) << " outputs (";

			if (write)
				std::cout << "write)\n";
			else
				std::cout << "non-write)\n";

			std::cout << "\n";
		}

	//	if (label == DECODER_ROM_STR)
			//assembler.AddDecoderRom(write, stoi(inSizeToken.value()), stoi(outSizeToken.value()));

	//	if (label == PROGRAM_ROM_STR)
			//assembler.AddProgramRom(write, stoi(inSizeToken.value()), stoi(outSizeToken.value()));
	}
};

class archRegister : public symbol
{
public:
	void process(assembler& assembler, const std::string& label, std::string remainder, int line) const override
	{
		auto sizeToken = parser::instance().extract_token_ws_comma(remainder);
		if (!sizeToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! There is no valid size!";
			throw std::exception(msg.str().c_str());
		}

		if (!isdigit(std::string(sizeToken.value())[0]))
		{
			std::stringstream msg;
			msg << "Assembling command " << label << " at line <" << line << ">! Invalid command size [";
			msg << sizeToken.value() << "]!!";
			throw std::exception(msg.str().c_str());
		}

		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = parser::instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());

				if (assembler.echoParsedMajor() && assembler.echoArchitecture())
					std::cout << "          *** Adding " << std::string(sizeToken.value()) << "-bit Register [" << nameTokenString << "]\n";

			//	assembler.addRegister(nameTokenString, stoi(std::string(sizeToken.value())), line);
			}
			else
			{
				tokensRemain = false;
			}
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
			std::cout << "\n";
	}
};

class archFlagDevice : public symbol
{
public:
	void process(assembler& assembler, const std::string& label, std::string remainder, int line) const override
	{
		bool tokensRemain = true;
		while (tokensRemain)
		{
			auto nameToken = parser::instance().extract_token_ws_comma(remainder);
			if (nameToken.has_value())
			{
				std::string nameTokenString = std::string(nameToken.value());

				if (assembler.echoParsedMajor() && assembler.echoArchitecture())
				{
					if (label == FLAG_STR)
						std::cout << "          *** Adding flag [" << nameTokenString << "]\n";

					if (label == DEVICE_STR)
						std::cout << "          *** Adding decice [" << nameTokenString << "]\n";
				}

				//if (label == FLAG_STR)
					//	assembler.AddFlag(nameTokenString, assembler.GetFlagCount() + 1, line);

				//if (label == DEVICE_STR)
					//	assembler.AddDevice(nameTokenString, assembler.GetFlagCount() + 1, line);
			}
			else
			{
				tokensRemain = false;
			}
		}

		if (assembler.echoParsedMajor() && assembler.echoArchitecture())
			std::cout << "\n";
	}
};