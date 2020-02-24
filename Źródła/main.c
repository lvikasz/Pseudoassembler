#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "assembler.h"

#define DEBUG_MODE_OFF 0
#define DEBUG_MODE_ON 1

int main(int argc, char** argv) {
	if (argc < 2)
	{
		//printf("You should pass source file as an argument");
		printf("Usage: Asm.exe (optional -d for debug mode) file_name");
		return 1;
	}

	char* content = read_file(argv[1]);
	//char* content = read_file("C:\\Users\\basin\\Desktop\\srednia.txt");
	//char* content = read_file("C:\\Users\\basin\\Desktop\\sort.txt");
	//char* content = read_file("C:\\Users\\basin\\Desktop\\przyklady\\milion.txt");
	//char* content = read_file("C:\\Users\\basin\\Desktop\\programy_pseudoass\\suma1.txt");
	//char* content = read_file("C:\\Users\\basin\\Desktop\\HASM\\suma_podanych_liczb.txt");
	//char* content = read_file("C:\\Users\\basin\\source\\repos\\Asembler\\a.txt");
	CommandInfo commands = parse_file(content);

	if (argc == 3)
		assembly(commands.commands, commands.commands_no, DEBUG_MODE_ON);
	else
		assembly(commands.commands, commands.commands_no, DEBUG_MODE_OFF);

	getchar();

	return 0;
}