#pragma once

#include <string>
#include <optional>

// Formats for literal number types
enum class LiteralNumType { None, Binary, Decimal, Hexadecimal };

class parser
{
public:
	// singleton
	static parser& instance()
	{
		static parser _instance;
		return _instance;
	}
	
	bool is_command(const std::string& s);
	bool is_directive(const std::string& s);
	bool is_label(const std::string &s);
	bool is_indirect(const std::string& s);
	bool is_address(const std::string& s);
	bool is_register(const std::string& s);

	void strip_comment(std::string& s);

	bool try_consume_comma(std::string& s);
	bool try_consume_equals(std::string& s);
	bool try_strip_indirect(std::string& s);
	bool try_strip_label(std::string& s);
	bool try_strip_address(std::string& s);

	std::optional<std::string> extract_token_ws(std::string& s);
	std::optional<std::string> extract_token_ws_comma(std::string& s);
	std::optional<std::string> extract_token_str(std::string& s);

	void trim_leading_ws(std::string& s);
	void trim_trailing_ws(std::string& s);
	void trim_ws(std::string& s);

	std::string get_lead_trimmed(std::string s);
	std::string get_trail_trimmed(std::string s);
	std::string get_trimmed(std::string s);

	LiteralNumType get_num_type(std::string& s);
	int parse_literal_num(std::string& s, LiteralNumType t);
	int parse_literal_num(std::string& s);
};