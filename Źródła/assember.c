#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <conio.h>

#include "assembler.h"
#include "stringHelpers.h"
#include "InstructionSet.h"
#include "interaction.h"
#include "memory_helpers.h"

#include "fort.h"

// state of last arithmetic operation or comaprison
// 00b = 0 - zero
// 01b = 1 - +
// 10b = 2 - -
// 11b = 3 - error
#define ZERO 0
#define POSITIVE 1
#define NEGATIVE 2
#define ERROR 3

VariablesInfo allocate_memory(Command *commands, int command_no, int** memory) {
	int i = 0;
	int memory_size = 0;
	int variables_no = 0;
	int memory_pointer = 0;
	char* value;
	Variable* variables = malloc(command_no * sizeof(Variable));

	for (i = 0; i < command_no; i++)
	{
		if (commands[i].Instruction != NULL)
		{
			if (strcmp(commands[i].Instruction, "DC") == 0 || strcmp(commands[i].Instruction, "DS") == 0)
			{
				char* cpy = malloc(strlen(commands[i].Arg1) + 1);
				strncpy(cpy, commands[i].Arg1, strlen(commands[i].Arg1) + 1);

				variables_no += 1;

				if (contains(commands[i].Arg1, '*'))
				{
					char* table_args = strtok(commands[i].Arg1, "*");

					int size = strtol(table_args, NULL, 10);

					variables[i].Size = size;
					memory_size += size;
				}
				else
				{
					variables[i].Size = 1;
					memory_size += 1;
				}

				variables[i].Label = commands[i].Label;

				if (contains(commands[i].Arg1, '('))
				{
					value = strtok(commands[i].Arg1, "(");
					value = strtok(0, "(");
					replace_char(value, ')', '\0');
					variables[i].DefaultValue = strtol(value, NULL, 10);

				}
				commands[i].Arg1 = cpy;
			}
		}
	}

	variables = realloc(variables, variables_no * sizeof(Variable));
	*memory = malloc(memory_size * sizeof(int));

	for (i = 0; i < variables_no; i++)
	{
		(*memory)[memory_pointer] = variables[i].DefaultValue;
		variables[i].Pointer = &((*memory)[memory_pointer]);

		memory_pointer += variables[i].Size;
	}

	VariablesInfo info;
	info.variables = variables;
	info.memory_size = memory_size;
	info.variables_no = variables_no;

	return info;
}

void set_state(int register_to_check, int registers[], int* state_register)
{
	if (registers[register_to_check] > 0)
		*state_register = POSITIVE;

	else if (registers[register_to_check] == 0)
		*state_register = ZERO;

	else if (registers[register_to_check] < 0)
		*state_register = NEGATIVE;
}

void AR(Command command, int registers[], int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	registers[register1] += registers[register2];
	set_state(register1, registers, state_register);
}

void SR(Command command, int registers[], int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	registers[register1] -= registers[register2];
	set_state(register1, registers, state_register);
}

void MR(Command command, int registers[], int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	registers[register1] *= registers[register2];
	set_state(register1, registers, state_register);
}

void DR(Command command, int registers[], int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	if (registers[register2] == 0)
	{
		*state_register = ERROR;
		return;
	}

	registers[register1] /= registers[register2];
	set_state(register1, registers, state_register);
}

void A(Command command, Variable* variables, int registers[], int variables_no, int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int value2 = resolve_memory(command, variables, registers, variables_no);

	registers[register1] += value2;
	set_state(register1, registers, state_register);
}

void S(Command command, Variable* variables, int registers[], int variables_no, int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int value2 =  resolve_memory(command, variables, registers, variables_no);

	registers[register1] -= value2;
	set_state(register1, registers, state_register);
}

void M(Command command, Variable* variables, int registers[], int variables_no, int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int value2 =  resolve_memory(command, variables, registers, variables_no);

	registers[register1] *= value2;
	set_state(register1, registers, state_register);
}

void D(Command command, Variable* variables, int registers[], int variables_no, int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int value2 =  resolve_memory(command, variables, registers, variables_no);

	if (value2 == 0)
		state_register = ERROR;

	registers[register1] /= value2;
	set_state(register1, registers, state_register);
}

void C(Command command, Variable* variables, int registers[], int variables_no, int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int value2 =  resolve_memory(command, variables, registers, variables_no);
	if (registers[register1] < value2)
		*state_register = NEGATIVE;

	else if (registers[register1] == value2)
		*state_register = ZERO;

	else if (registers[register1] > value2)
		*state_register = POSITIVE;
}

void CR(Command command, int registers[], int* state_register)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	if (registers[register1] < registers[register2])
		*state_register = NEGATIVE;

	else if (registers[register1] == registers[register2])
		*state_register = ZERO;

	else if (registers[register1] > registers[register2])
		*state_register = POSITIVE;
}

void L(Command command, Variable* variables, int registers[], int variables_no)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	registers[register1] =  resolve_memory(command, variables, registers, variables_no);

}

void LA(Command command, Variable* variables, int registers[], int variables_no)
{
	int register1 = strtol(command.Arg1, NULL, 10);
	registers[register1] = resolve_memory_address(command, variables, variables_no);
}

void LR(Command command, int registers[])
{
	int register1 = strtol(command.Arg1, NULL, 10);
	int register2 = strtol(command.Arg2, NULL, 10);

	registers[register1] = registers[register2];
}


void ST(Command command, Variable* variables, int registers[], int variables_no)
{
	char* copy = malloc(10);
	int register1;
	size_t arg_len = strlen(command.Arg1);

	strncpy(copy, command.Arg1, arg_len);
	register1 = strtol(copy, NULL, 10);

	store_to_memory(command, variables, registers, variables_no, registers[register1]);
}

int J(Command* commands, Command command, int command_no)
{
	return find_label_number(commands, command_no, command.Arg1);
}

int JZ(Command* commands, Command command, int command_no, int i, int* state_register)
{
	if (*state_register == ZERO)
		return find_label_number(commands, command_no, command.Arg1);

	return i + 1;
}

int JP(Command* commands, Command command, int command_no, int i, int* state_register)
{
	if (*state_register == POSITIVE)
		return find_label_number(commands, command_no, command.Arg1);

	return i + 1;
}

int JN(Command* commands, Command command, int command_no, int i, int* state_register)
{
	if (*state_register == NEGATIVE)
		return find_label_number(commands, command_no, command.Arg1);

	return i + 1;
}

void RD(Command* command, Variable* variables, int variables_no, int* memory)
{
	read(command, variables, variables_no, memory);
}

void PRT(Command* command, Variable* variables, int variables_no)
{
	print_memory_by_label(command, variables, variables_no);
}

void assembly(Command* commands, int commands_no, int debug_mode)
{
	int i = 0;
	int* memory = NULL;
	int memory_size = 0;
	int current_instruction_no = 0;

	// registers: from 0 to 13 - available; 14 instruction pointer; 15 - data pointer
	int registers[16];
	int state_register = ZERO;

	Variable* variables = NULL;
	int variables_no = 0;

	VariablesInfo info = allocate_memory(commands, commands_no, &memory);
	variables = info.variables;
	memory_size = info.memory_size;
	variables_no = info.variables_no;

	registers[14] = memory;

	while (i < commands_no)
	{
		current_instruction_no = i;
		registers[15] = current_instruction_no;

		if (strcmp(commands[i].Instruction, "A") == 0)
			A(commands[i], variables, registers, variables_no, &state_register);

		else if (strcmp(commands[i].Instruction, "AR") == 0)
			AR(commands[i], registers, &state_register);

		else if (strcmp(commands[i].Instruction, "S") == 0)
			S(commands[i], variables, registers, variables_no, &state_register);

		else if (strcmp(commands[i].Instruction, "SR") == 0)
			SR(commands[i], registers, &state_register);

		else if (strcmp(commands[i].Instruction, "M") == 0)
			M(commands[i], variables, registers, variables_no, &state_register);

		else if (strcmp(commands[i].Instruction, "MR") == 0)
			MR(commands[i], registers, &state_register);

		else if (strcmp(commands[i].Instruction, "D") == 0)
			D(commands[i], variables, registers, variables_no, &state_register);

		else if (strcmp(commands[i].Instruction, "DR") == 0)
			DR(commands[i], registers, &state_register);

		else if (strcmp(commands[i].Instruction, "C") == 0)
			C(commands[i], variables, registers, variables_no, &state_register);

		else if (strcmp(commands[i].Instruction, "CR") == 0)
			CR(commands[i], registers, &state_register);

		else if (strcmp(commands[i].Instruction, "L") == 0)
			L(commands[i], variables, registers, variables_no);

		else if (strcmp(commands[i].Instruction, "LA") == 0)
			LA(commands[i], variables, registers, variables_no);

		else if (strcmp(commands[i].Instruction, "LR") == 0)
			LR(commands[i], registers);

		else if (strcmp(commands[i].Instruction, "ST") == 0)
			ST(commands[i], variables, registers, variables_no);

		else if (strcmp(commands[i].Instruction, "RD") == 0)
			RD(&commands[i], variables, variables_no, memory);

		else if (strcmp(commands[i].Instruction, "PRT") == 0)
			PRT(&commands[i], variables, variables_no);

		else if (strcmp(commands[i].Instruction, "J") == 0)
		{
			i = J(commands, commands[i], commands_no);
			continue;
		}

		else if (strcmp(commands[i].Instruction, "JZ") == 0)
		{
			i = JZ(commands, commands[i], commands_no, i, &state_register);
			continue;
		}

		else if (strcmp(commands[i].Instruction, "JP") == 0)
		{
			i = JP(commands, commands[i], commands_no, i, &state_register);
			continue;
		}

		else if (strcmp(commands[i].Instruction, "JN") == 0)
		{
			i = JN(commands, commands[i], commands_no, i, &state_register);
			continue;
		}

		interact(
			commands,
			current_instruction_no,
			commands_no,
			registers,
			state_register,
			memory,
			memory_size,
			debug_mode
		);
		i += 1;
	}

	end_interaction(
		commands,
		current_instruction_no,
		commands_no,
		registers,
		state_register,
		memory,
		memory_size
	);
	/*interact(
		commands,
		current_instruction_no,
		commands_no,
		registers,
		state_register,
		memory,
		memory_size,
		debug_mode
	);*/

	free(memory);
	free(commands);
}