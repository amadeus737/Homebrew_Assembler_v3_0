#include "assembler.h"

#include <iostream>
#include <conio.h>

int main(int argc, char* argv[])
{
	// On the command-line, we expect ./asm file.s, where asm is the name of this
	// executable and file.s is the file containing the program that you would
	// like assembled. It is implied that file.s either contains all the architecture
	// definitions needed to define your homebrew cpu or includes the appropriate
	// architecture file with those definitions.
	if (argc < 2)
	{
		std::cout << "Please specify an input file!" << std::endl;
	}
	else
	{
		// try-catch any fatal errors
		try
		{
			assembler assembler(argv[1]);
		
			// set the echo verbosity - 8 bit value
			//  -> bit 7 : echo architecture file definitions
			//  -> bit 6 : echo major tasks
			//  -> bit 5 : echo minor tasks
			//  -> bit 4 : echo warnings
			//  -> bit 3 : echo major parsing information
			//  -> bit 2 : echo minor parsing information
			//  -> bit 1 : echo source code
			//  -> bit 0 : echo rom contents
			assembler.setEcho(0xFF);

			assembler.assemble();
		}
		catch (const std::exception& e)
		{
			std::cout << "Fatal error: " << e.what() << std::endl;
		}
	}

	// wait for a keypress
	while (!_kbhit());

	return 0;
}