#pragma once
#include "InstructionSet.h"

int resolve_memory(Command command, Variable* variables, int registers[], int variables_no);
int resolve_memory_address(Command command, Variable* variables, int variables_no);
Variable resolve_memory_label(char* label, Variable* variables, int variables_no);
void store_to_memory(Command command, Variable* variables, int registers[], int variables_no, int value);
int find_label_number(Command* commands, int command_no, char* label);