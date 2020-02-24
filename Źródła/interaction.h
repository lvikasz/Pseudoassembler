#include "InstructionSet.h"

#pragma once

void print_instruction(Command* command);
void print_underlined_instruction(Command* command);

void print_registers(int registers[], int state_register);
void print_instructions(Command* commands, int current_command_no, int commands_no);
void print_memory_by_label(Command* command, Variable* variables, int variables_no);
void print_memory(void* memory, int mem_size);

void change_screen_mode(
	Command* commands,
	int mode,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size);

void interact(
	Command* commands,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size,
	int debug_mode);

void end_interaction(
	Command* commands,
	int current_instruction_no,
	int commands_no,
	int registers[],
	int state_register,
	int* memory,
	int mem_size);


void read(Command* command, Variable* variables, int variables_no);
void align(const char *in, char alignment, int current_line);