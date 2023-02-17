#pragma once

#include "command.h"

class opcodeInstruction : public command
{
public:
	virtual void process(assembler& assembler, std::string& line, int value0, int value1, int startAddress) const override
	{
		int oc_value = assembler.getValueByUniqueOpcodeString(line);
		int instruction_width = assembler.getInstructionWidth();

		//for (int i = 0; i < instruction_width; i++)
		{
			assembler.addByteToProgramRom(oc_value);
			/*
			if (i < instruction_width - 1)
				assembler.AddByteToProgramROM(0);
			else
				assembler.AddByteToProgramROM(oc_value);
				*/
		}

		if (value0 != -1)
		{
			assembler.addByteToProgramRom(value0);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value0);
			}*/
		}

		if (value1 != -1)
		{
			assembler.addByteToProgramRom(value1);
			/*for (int i = 0; i < instruction_width; i++)
			{
				if (i < instruction_width - 1)
					assembler.AddByteToProgramROM(0);
				else
					assembler.AddByteToProgramROM(value1);
			}*/
		}
	}
};
