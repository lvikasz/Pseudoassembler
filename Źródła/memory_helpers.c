#include <stdio.h>
#include <string.h>
#include "memory_helpers.h"

int resolve_memory(Command command, Variable* variables, int registers[], int variables_no)
{
	// array expression
	if (contains(command.Arg2, '('))
	{
		char* copy = malloc(strlen(command.Arg2) + 1);
		char* value;
		int shift;
		int register_no;

		strncpy(copy, command.Arg2, strlen(command.Arg2) + 1);

		value = strtok(copy, "(");
		shift = strtol(value, NULL, 10);
		value = strtok(0, "(");
		replace_char(value, ')', '\0');
		register_no = strtol(value, NULL, 10);

		free(copy);
		return *(int*)(registers[register_no] + (shift/* / sizeof(int)*/));
	}

	// variable
	else
	{
		int i;
		for (i = 0; i < variables_no; i++)
		{
			if (strcmp(variables[i].Label, command.Arg2) == 0)
				return *(int *)(variables[i].Pointer);
		}
	}
}

int resolve_memory_address(Command command, Variable* variables, int variables_no)
{
	int i;
	for (i = 0; i < variables_no; i++)
	{
		if (strcmp(variables[i].Label, command.Arg2) == 0) {
			return variables[i].Pointer;
		}
	}
}

Variable resolve_memory_label(char* label, Variable* variables, int variables_no)
{
	int i = 0;
	for (i = 0; i < variables_no; i++)
	{
		if (strcmp(variables[i].Label, label) == 0)
			return variables[i];
	}
}

void store_to_memory(Command command, Variable* variables, int registers[], int variables_no, int value)
{
	if (contains(command.Arg2, '(')) {
		char* copy = malloc(strlen(command.Arg2));
		char* register1;
		int shift;
		int register_no;

		strncpy(copy, command.Arg2, strlen(command.Arg2));

		register1 = strtok(copy, "(");
		shift = strtol(register1, NULL, 10);

		register1 = strtok(0, "(");
		replace_char(register1, ')', '\0');

		register_no = strtol(register1, NULL, 10);

		free(copy);
		*(int*)(registers[register_no] + (shift/* / sizeof(int)*/)) = value;
	}
	else
	{
		int i;
		for (i = 0; i < variables_no; i++)
		{
			if (strcmp(variables[i].Label, command.Arg2) == 0)
				*(int *)variables[i].Pointer = value;
		}
	}
}

int find_label_number(Command* commands, int command_no, char* label)
{
	for (int i = 0; i < command_no; i++) {
		if (strcmp(commands[i].Label, label) == 0)
			return i;
	}

	return -1;
}