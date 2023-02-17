#pragma once

#include "assembler.h"
#include "command.h"

#include <iomanip>
#include <sstream>

class includeDirective : public command
{
public:
	void process(assembler& a, const std::string& d, std::string remainder, int line) const override
	{
		auto token = parser::instance().extract_token_str(remainder);
		if (!token.has_value())
		{
			// no data
			std::stringstream msg;
			msg << "Processing include directive ." << d << " at line <" << line << ">! No data!";
			throw std::exception(msg.str().c_str());
		}

		// check for garbage after directive
		if (!remainder.empty())
		{
			std::stringstream msg;
			msg << "Processing include directive ." << d << " at line <" << line << ">! Does not include : [";
			msg << remainder << "]!!";
			throw std::exception(msg.str().c_str());
		}

		std::string tokenString = token.has_value() ? std::string(token.value()) : "";
		parser::instance().trim_ws(tokenString);

		if (!tokenString.empty())
		{
			if (a.echoMajorTasks())
				std::cout << "          *** Processing include directive for file: " << tokenString << "\n";

			a.pushFile("code\\" + tokenString);
			a.processFile();
		}
		else
		{
			// bad value
			std::stringstream msg;
			msg << "Processing include directive " << d << " at line <" << line << ">! Does not include : [";
			msg << tokenString << "]!!";
			throw std::exception(msg.str().c_str());
		}
	}
};

class originDirective : public command
{
public:
	void process(assembler& a, const std::string& d, std::string remainder, int line) const override
	{
		auto valueToken = parser::instance().extract_token_ws_comma(remainder);
		if (!valueToken.has_value())
		{
			std::stringstream msg;
			msg << "Assembling directive at line <" << line << ">! Org is not assigned a valid value!";
			throw std::exception(msg.str().c_str());
		}

		std::string valueString = std::string(valueToken.value());
		int parsedValue = parser::instance().parse_literal_num(valueString);

		if (parsedValue != -1)
		{
			try
			{
				a.setAddress(parsedValue);

				if (a.echoParsedMajor())
					std::cout << "          *** Setting Address to $" << hex8 << parsedValue << "\n\n";
			}
			catch (const std::exception& e)
			{
				std::stringstream msg;
				msg << "Processing directive ." << d << " at line <" << line << ">! Encountered exception!" << std::endl;
				msg << "\t" << e.what();
				throw std::exception(msg.str().c_str());
			}
		}
		else
		{
			// bad value
			std::stringstream msg;
			msg << "Processing ORG directive " << d << " at line <" << line << ">! What is meant by : [";
			msg << parsedValue << "]!!";
			throw std::exception(msg.str().c_str());
		}
	}
};