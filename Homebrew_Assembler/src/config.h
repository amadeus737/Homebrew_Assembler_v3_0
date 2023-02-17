#pragma once

// note that space ' ' character is interpreted as no character defined
constexpr const char BIN_KEY = '%';
constexpr const char DEC_KEY = ' ';
constexpr const char HEX_KEY = '$';

constexpr const char COMMENT_KEY = ';';

constexpr const char DIRECTIVE_KEY = '.';
constexpr const char* LABEL_DECORATORS = "[]_";
constexpr const char LABEL_END_KEY = ':';
constexpr const char INDIRECT_BEGIN_KEY = '[';
constexpr const char INDIRECT_END_KEY = ']';
constexpr const char ADDRESS_KEY = '&';

constexpr const char* CONST_STR = "constant";
constexpr const char* VAR_STR = "variable";

constexpr const char* SEGMENT_STR = "segment";
constexpr const char* INCLUDE_STR = "include";
constexpr const char* ORIGIN_STR = "org";
constexpr const char* REGISTER_STR = "register";
constexpr const char* FLAG_STR = "flag";
constexpr const char* DEVICE_STR = "device";
constexpr const char* CONTROL_STR = "control";
constexpr const char* OPCODE_STR = "opcode";
constexpr const char* OPCODE_ALIAS_STR = "opcode_alias";
constexpr const char* OPCODE_SEQ_STR = "seq";
constexpr const char* OPCODE_SEQ_IF_STR = "seq_if";
constexpr const char* OPCODE_SEQ_ELSE_STR = "seq_else";
constexpr const char* END_ARCH_STR = "**endarch**";

// also add multiple passes for symbol resolution (like look-ahead label addressing)

constexpr const char* INSTRUCTION_WIDTH_STR = "instruction_width";
constexpr const char* ADDRESS_WIDTH_STR = "address_width";
constexpr const char* PROGRAM_ROM_STR = "program_rom";
constexpr const char* DECODER_ROM_STR = "decoder_rom";