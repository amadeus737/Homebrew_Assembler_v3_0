#include "parser.h"
#include "symbolConfig.h"

#include <algorithm>

// symbols are sorta like commands...they cannot start with a digit and can contain any non-register alphanumeric or underscore
// characters
bool parser::is_symbol(const std::string& s)
{
	return s.size() > 0 &&
		!isdigit(s.front()) &&
		std::all_of(s.begin(), s.end(), [](char c)
			{
				return isalnum(c) || c == '_';
			}) &&
		std::any_of(s.begin(), s.end(), [](char c)
			{
				return isalnum(c);
			});
}

// directives start with the DIRECTIVE_KEY (see symbolConfig.h), and are otherwise alphanumeric
bool parser::is_directive(const std::string& s)
{
	return s.size() > 1 && s.front() == DIRECTIVE_KEY && std::all_of(s.begin() + 1, s.end(), [](char c) { return isalnum(c); });
}

// labels can optionally contain any LABEL_DECORATORS and must end in the LABEL_END_KEY
bool parser::is_label(const std::string& s)
{
	return s.size() > 1 && s.back() == LABEL_END_KEY && !isdigit(s.front()) &&
		std::all_of(s.begin(), std::prev(s.end()), [](char c)
			{
				bool retVal = false;
				if (strlen(LABEL_DECORATORS) > 0)
				{
					for (int i = 0; i < strlen(LABEL_DECORATORS); i++)
						retVal = retVal || c == LABEL_DECORATORS[i];
				}

				return isalnum(c) || retVal;
			}) &&
		std::any_of(s.begin(), std::prev(s.end()), [](char c) { return isalnum(c); });
}

// erase any strings starting with the character specified by the COMMENT_KEY (see symbolConfig.h)
void parser::strip_comment(std::string& s)
{
	// Find the position in the string corresponding to the COMMENT_KEY
	size_t pos = s.find_first_of(COMMENT_KEY);

	// If that position is not undefined, erase from that position until end of string
	if (pos != std::string::npos)
	{
		s.erase(pos);
	}
}

// One of the primary functions used in the parser...it will extract a "token", defined as a collection
// of consecutive characters without any whitespace in-between (e.g., a space).
std::optional<std::string> parser::extract_token_ws(std::string& s)
{
	// First, delete any whitespace occurring before non-ws characters
	trim_leading_ws(s);

	// If there are actually characters in the string...
	if (s.size() > 0)
	{
		// Search the string and find characters corresponding to space
		const auto delimiter = std::find_if(s.begin(), s.end(), [](char c)
			{
				return (isspace(c));
			});

		// If we found a space character, we will not be at the end of the string
		if (delimiter != s.end())
		{
			// Build a string that only includes the characters up until that first
			// space character found
			std::string t = std::string(s.begin(), delimiter);

			// Delete the corresponding characters in the passed-in string reference
			// so that further chaining of tokenization can be performed
			s.erase(s.begin(), delimiter);

			// Return the string token
			return t;
		}
		else
		{
			return std::move(s);
		}
	}

	return { };
}

// Similar idea to the function above, except it also includes commas as characters to parse out
std::optional<std::string> parser::extract_token_ws_comma(std::string& s)
{
	trim_leading_ws(s);

	if (s.size() > 0)
	{
		const auto delimiter = std::find_if(s.begin(), s.end(), [](char c)
			{
				return (isspace(c) || c == ',');
			});

		if (delimiter != s.end())
		{
			std::string t = std::string(s.begin(), delimiter);
			s.erase(s.begin(), delimiter + 1);
			return t;
		}
		else
		{
			return std::move(s);
		}
	}

	return { };
}

// Used for parsing strings between double quotes
std::optional<std::string> parser::extract_token_str(std::string& s)
{
	int count = 0;
	bool parsingString = true;
	std::string parsedString = "";
	while (parsingString && count < 10)
	{
		// Extract the string token
		auto token = extract_token_ws(s);
		std::string tokenString = token.has_value() ? std::string(token.value()) : "";
		if (count == 0)
		{
			// Early exit if the string doesn't start with double quote
			if (tokenString.front() != '"')
			{
				parsingString = false;

				// Probably need an error message here!

				break;
			}
			else
			{
				// Chop off open quote
				tokenString = tokenString.substr(1, std::string::npos);

				// Make sure the end of the string closes with a double quote
				if (!tokenString.empty() && tokenString.back() == '"' && tokenString.length() > 1)
				{
					// Chop off end quote
					tokenString = tokenString.substr(0, tokenString.length() - 1);
					parsedString += " " + tokenString;
					s.erase(s.begin(), s.end());

					return parsedString;
				}
				else
				{
					parsedString += " " + tokenString;
					s.erase(s.begin(), s.end());
				}
			}
		}
		else
		{
			if (!tokenString.empty() && tokenString.back() == '"' && tokenString.length() > 1)
			{
				// Chop off end quote
				tokenString = tokenString.substr(0, tokenString.length() - 1);
				parsedString += " " + tokenString;
				s.erase(s.begin(), s.end());
				return parsedString;
			}
			else
			{
				parsedString += " " + tokenString;
				s.erase(s.begin(), s.end());
			}
		}

		count++;
	}

	return { };
}

// Trim off leading spaces
void parser::trim_leading_ws(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !isspace(c); }));
}

// Trim off trailing spaces
void parser::trim_trailing_ws(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); }).base(), s.end());
}

// Trim off both leading and trailing spaces
void parser::trim_ws(std::string& s)
{
	trim_leading_ws(s);
	trim_trailing_ws(s);
}

// Check the token string for the different literal number types supported in this assembler
LiteralNumType parser::get_num_type(std::string& s)
{
	if (s.size() == 0)
		return LiteralNumType::None;

	// If the first character is not a number...
	if (!isdigit(s[0]))
	{
		if (BIN_KEY != ' ' && s.front() == BIN_KEY)
		{
			// Matches non-empty BIN_KEY so is binary.
			// Strip off the symbol for further processing.
			s = std::string(s.begin() + 1, s.end());
			return LiteralNumType::Binary;
		}
		else if (DEC_KEY != ' ' && s.front() == DEC_KEY)
		{
			// Matches non-empty DEC_KEY so is decimal.
			// Strip off the symbol for futher processing.
			s = std::string(s.begin() + 1, s.end());
			return LiteralNumType::Decimal;
		}
		else if (HEX_KEY != ' ' && s.front() == HEX_KEY)
		{
			// Matches non-empty HEX_KEY so is hexadecimal.
			// Strip off the symbol for futher processing.
			s = std::string(s.begin() + 1, s.end());

			// Also, handle formats like 0xhhhh, for example.
			return std::all_of(std::next(s.begin(), 1), s.end(), [](char c) {
				return isdigit(c) ||
					(tolower(c) >= 'a' && tolower(c) <= 'f');
				}) ? LiteralNumType::Hexadecimal : LiteralNumType::None;
		}
		else
		{
			// Otherwise, the type is undefined
			return LiteralNumType::None;
		}
	}

	// Handle formats like 0xnnn, 0bnnnn
	if (s.size() > 3)
	{
		if (tolower(s[1]) == 'x' || tolower(s[1]) == 'h')
		{
			s = s.substr(2, std::string::npos);

			return std::all_of(std::next(s.begin()), s.end(), [](char c) {
				return isdigit(c) ||
					(tolower(c) >= 'a' && tolower(c) <= 'f');
				}) ? LiteralNumType::Hexadecimal : LiteralNumType::None;
		}

		if (tolower(s[1]) == 'b')
		{
			s = s.substr(2, std::string::npos);
			return std::all_of(std::next(s.begin()), s.end(), [](char c) { return c == '0' || c == '1'; }) ? LiteralNumType::Binary : LiteralNumType::None;
		}

		if (tolower(s[1]) == 'd')
		{
			s = s.substr(2, std::string::npos);
			return std::all_of(std::next(s.begin()), s.end(), [](char c) { return isdigit(c); }) ? LiteralNumType::Decimal : LiteralNumType::None;
		}
	}

	// WHY is std::next required here???
	return std::all_of(std::next(s.begin()), s.end(), [](char c) { return isdigit(c); }) ? LiteralNumType::Decimal : LiteralNumType::None;
}

// Parse the number when the number type is known using the appropriate number base in the stoi function
int parser::parse_literal_num(std::string& s, LiteralNumType t)
{
	switch (t)
	{
	case LiteralNumType::Binary:
		return std::stoi(s, 0, 2);
		break;

	case LiteralNumType::Decimal:
		return std::stoi(s);
		break;

	case LiteralNumType::Hexadecimal:
		return std::stoi(s, 0, 16);
		break;

	default:
		//assert("bad int literal parse type!" && false);
		return -1;
	}
}

// Wrapper function which also automatically calls the number type function
int parser::parse_literal_num(std::string& s)
{
	return parse_literal_num(s, get_num_type(s));
}