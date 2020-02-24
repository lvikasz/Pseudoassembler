#pragma once
#define instruction_size 22

extern char* InstructionSet[instruction_size];

struct Command
{
	// optional
	char* Label;

	char* Instruction;

	// optional
	char* Arg1;
	char* Arg2;
};

struct Variable
{
    char* Label;
    void* Pointer;
	int Size;
	int DefaultValue;
};

typedef struct Command Command;
typedef struct Variable Variable;

struct CommandInfo
{
	Command* commands;
	int commands_no;
};

struct VariablesInfo
{
	Variable* variables;
	int variables_no;
	int memory_size;
};

typedef struct CommandInfo CommandInfo;
typedef struct VariablesInfo VariablesInfo;