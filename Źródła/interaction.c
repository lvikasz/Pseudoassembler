#include <stdio.h>
#include <string.h>

#include "interaction.h"
#include "memory_helpers.h"
#include "fort.h"

#define MAX_INT_LENGTH 12
#define MEMORY_COLUMN_SIZE 5
#define MAX_LINE_SIZE 140
#define MAX_LINES_IN_FILE 1000

// screen modes
#define REGISTERS 1
#define MEMORY 2
#define INSTRUCTIONS 3

int screen_mode = REGISTERS;

void align(const char *in, char alignment, int current_line)
{
	int col, i, l, r, line;
	int w[1024] = { 0 };
	const char *s;

	for (s = in, i = col = 0; s[i]; s += i + 1) {
		for (i = 0; s[i] && s[i] != '$' && s[i] != '\n'; i++);

		if (s[i] == 27) i += 4;

		if (i > w[col]) w[col] = i;

		if (col++ >= 1024) abort();

		if (s[i] == '\n') col = 0;
		if (!s[i]) break;
	}

	for (s = in, i = col = 0, line = 0; s[i]; s += i + 1) {
		if (current_line == line)
			printf("%c[1;7m", 27);

		else
			printf("%c[0m", 27);

		for (i = 0; s[i] && s[i] != '$' && s[i] != '\n'; i++);

		switch (alignment) {
		case 'l':	r = w[col] - i; break;
		case 'c':	r = (w[col] - i) / 2; break;
		case 'r':	r = 0; break;
		}
		l = w[col++] - i - r + 1;

		while (l--) putchar(' ');
		
		printf("%.*s", i, s);

		while (r--) putchar(' ');

		if (current_line == line)
			printf("%c[0m", 27);

		if (s[i] != '$') {
			putchar('\n');
			col = 0;
			line += 1;
		}

		if (!s[i]) break;
	}
}

void print_instruction(Command* command)
{
	if ((*command).Label != NULL && strcmp((*command).Label, "") != 0)
		printf("%s ", (*command).Label);

	printf("%s ", (*command).Instruction);

	if ((*command).Arg1 != NULL)
		printf("%s", (*command).Arg1);

	if ((*command).Arg2 != NULL && strcmp((*command).Arg2, "") != 0)
		printf(", %s", (*command).Arg2);
}

void print_underlined_instruction(Command* command)
{
	if ((*command).Label != NULL && strcmp((*command).Label, "") != 0)
		printf("%c[1;7m%s %c[0m", 27, (*command).Label, 27);

	printf("\t\t%c[1;7m%s %c[0m", 27, (*command).Instruction, 27);

	if ((*command).Arg1 != NULL)
		printf("%c[1;7m%s%c[0m", 27, (*command).Arg1, 27);

	if ((*command).Arg2 != NULL && strcmp((*command).Arg2, "") != 0)
		printf("%c[1;7m, %s%c[0m", 27, (*command).Arg2, 27);
}

void print_registers(int registers[], int state_register)
{
	ft_table_t *table = ft_create_table();
	char* value1 = malloc(MAX_INT_LENGTH);
	char* value2 = malloc(MAX_INT_LENGTH);

	ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

	ft_write_ln(table, "Register", "Value");

	for (int i = 0; i < 16; i++)
	{
		_itoa(i, value1, 10);
		_itoa(registers[i], value2, 10);

		ft_write_ln(table, value1, value2);
	}

	ft_add_separator(table);

	_itoa(state_register, value1, 10);
	ft_write_ln(table, "STATE", value1);

	printf("%s\n", ft_to_string(table));

	ft_destroy_table(table);
	free(value1);
	free(value2);
}

void print_instructions(Command* commands, int current_command_no, int commands_no)
{
	char* prt = malloc(140 * 100);
	char* num = malloc(10);
	memset(prt, '\0', sizeof(prt));

	for (int i = 0; i < commands_no; i++)
	{
		_itoa(i + 1, num, 10);

		strcat(prt, num);
		strcat(prt, "$");

		if ((commands[i]).Label != NULL && strcmp((commands[i]).Label, "") != 0)
		{
			strcat(prt, commands[i].Label);
		}

		strcat(prt, "$");
		strcat(prt, commands[i].Instruction);
		strcat(prt, " $");

		if (commands[i].Arg1 != NULL)
		{
			strcat(prt, commands[i].Arg1);

		}
		strcat(prt, "$");

		if (commands[i].Arg2 != NULL && strcmp(commands[i].Arg2, "") != 0)
		{
			strcat(prt, commands[i].Arg2);
		}

		strcat(prt, "$\n");
	}
	
	align(prt, 'l',  current_command_no);
	free(prt);
}

void print_memory_by_label(Command* command, Variable* variables, int variables_no)
{
	int* i;
	Variable var = resolve_memory_label((*command).Arg1, variables, variables_no);
	system("cls");
	printf("Value(s) for %s: ", (*command).Arg1);
	for (i = var.Pointer; i < ((char *)var.Pointer + (var.Size * sizeof(int))); i++) {
		printf("%d\n", *i);
	}
	getchar();
}


void print_memory(void* memory, int mem_size)
{
	int* i;
	int* j;
	char* value[MEMORY_COLUMN_SIZE + 1];
	char* header[MEMORY_COLUMN_SIZE + 1];
	int k;

	for (k = 0; k < MEMORY_COLUMN_SIZE + 1; k++) {
		value[k] = malloc(MAX_INT_LENGTH);
	}

	for (k = 0; k < MEMORY_COLUMN_SIZE + 1; k++) {
		header[k] = malloc(MAX_INT_LENGTH);
	}

	ft_table_t *table = ft_create_table();
	ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

	header[0] = "";
	int header_value = 0;
	for (k = 1; k < MEMORY_COLUMN_SIZE + 1; k++)
	{
		sprintf(header[k], "+0x%X", header_value);
		header_value += 4;
	}

	ft_row_write_ln(table, MEMORY_COLUMN_SIZE + 1, header);

	//ft_write_ln(table, "", "+0x0", "+0x4", "+0x8", "+0xC", "0x10");

	int added = 0;

	while (added < mem_size) {
		sprintf(value[0], "0x%X", ((int*)memory + added));

		if (mem_size - added < MEMORY_COLUMN_SIZE) {
			k = 1;
			for (int* j = (int*)memory + added; j < (int*)memory + mem_size; j++, k++) {
				_itoa(*j, value[k], 10);
			}
			ft_row_write_ln(table, k, value);

			break;
		}

		k = 1;
		for (int* j = (int*)memory + added; j < (int*)memory + added + MEMORY_COLUMN_SIZE; j++, k++) {
			_itoa(*j, value[k], 10);
		}
		ft_row_write_ln(table, MEMORY_COLUMN_SIZE + 1, value);

		added += MEMORY_COLUMN_SIZE;
	}

	printf("%s\n", ft_to_string(table));
	ft_destroy_table(table);

	for (k = 0; k < 6; k++) {
		free(value[k]);
	}
}

void change_screen_mode(
	Command* commands,
	int mode,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size)
{
	system("cls");
	printf("Press c to continue.\n");

	// reg, mem, inst
	switch (mode)
	{
	case REGISTERS:
		print_instruction(&commands[current_instruction_no]);
		printf("\n");
		print_registers(registers, state_register);
		break;
	case MEMORY:
		print_instruction(&commands[current_instruction_no]);
		printf("\n");
		print_memory(memory, mem_size);
		break;
	case INSTRUCTIONS:
		print_instructions(commands, current_instruction_no, commands_no);
		break;
	default:
		break;
	}
}

void interact(
	Command* commands,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size,
	int debug_mode)
{
	change_screen_mode(commands, screen_mode, current_instruction_no, commands_no, registers, state_register, memory, mem_size);

	if (debug_mode == 1)
	{
		char c = '\0';
		do
		{
			if (_kbhit())
			{
				c = _getch();
				if (isdigit(c)) {
					if (screen_mode != atoi(&c))
						screen_mode = atoi(&c);
					change_screen_mode(commands, screen_mode, current_instruction_no, commands_no, registers, state_register, memory, mem_size);
				}
			}

		} while (c != 'c');
	}
}

void end_interaction(
	Command* commands,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size)
{
	char c = '\0';
	do
	{
		if (_kbhit())
		{
			c = _getch();
			if (isdigit(c)) {
				if (screen_mode != atoi(&c))
					screen_mode = atoi(&c);
				change_screen_mode(commands, screen_mode, current_instruction_no, commands_no, registers, state_register, memory, mem_size);
			}
		}
	} while (c != 'q');
}

void read(Command* command, Variable* variables, int variables_no)
{
	int i = 0;
	int value;
	int* mem_pointer;
	Variable var;
	if ((*command).Arg1 == NULL || strcmp((*command).Arg1, "") == 0)
		return;

	var = resolve_memory_label((*command).Arg1, variables, variables_no);
	mem_pointer = var.Pointer;

	system("cls");
	printf("Please, enter value(s) for label %s\n", (*command).Arg1);
	for (i = 0; i < var.Size; i++)
	{
		scanf("%d", &value);
		(*mem_pointer) = value;
		mem_pointer++;
	}
	system("cls");
}