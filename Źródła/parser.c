#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "InstructionSet.h"
#include "stringHelpers.h"

#define instruction_size 22

#define MAX_LABEL_SIZE 20
#define MAX_INSTRUCTION_SIZE 10
#define MAX_ARGUMENTS_SIZE 20
#define MAX_LINE_SIZE 140

void skip_whitespaces(char* s[])
{
	while (*s[0] == ' ' || *s[0] == '\t' || *s[0] == ',')
		*s += 1;
}

bool match(char* s[], const char* token)
{
	if (strncmp(*s, token, strlen(token)) == 0)
	{
		*s += strlen(token);
		return true;
	}

	return false;
}

bool matches_defined_instruction(char* s)
{
	int i = 0;
	for (i = 0; i < instruction_size; i++)
	{
		if (strcmp(s, InstructionSet[i]) == 0)
			return true;
	}

	return false;
}

bool labels(char* s[], Command *instruction)
{
	skip_whitespaces(s);

	char *orginal = *s;
	int i = 0;
	char* label = malloc(MAX_LABEL_SIZE);

	if (label == NULL)
		return false;

	while (*s[0] != ' ')
	{
		*s += 1;
		i += 1;
	}

	strncpy(label, orginal, i);
	label[i] = '\0';

	if (matches_defined_instruction(label))
	{
		*s = orginal;
		(*instruction).Label = "";
		return false;
	}

	(*instruction).Label = label;
	return true;
}

bool instructions(char* s[], Command *instruction)
{
	skip_whitespaces(s);

	char *orginal = *s;
	int i = 0;
	char* instruction_name = malloc(MAX_INSTRUCTION_SIZE);

	if (instruction_name == NULL)
		return false;

	while (*s[0] != ' ' && isalpha(*s[0]))
	{
		*s += 1;
		i += 1;
	}

	if (instruction_name == NULL)
		return false;

	strncpy(instruction_name, orginal, i);
	instruction_name[i] = '\0';

	if (!matches_defined_instruction(instruction_name))
	{
		*s = orginal;
		return false;
	}

	(*instruction).Instruction = instruction_name;
	return true;
}

bool arguments(char* s[], Command *instruction)
{
	skip_whitespaces(s);

	char* orginal = *s;
	char* orginal2;

	int i = 0;
	int j = 0;

	char* arg1 = malloc(MAX_ARGUMENTS_SIZE);
	char* arg2 = malloc(MAX_ARGUMENTS_SIZE);

	if (arg1 == NULL || arg2 == NULL)
		return false;

	while (*s[0] != ' ' && *s[0] != '\n' && *s[0] != '\0' && isprint(*s[0]) && *s[0] != ',')
	{
		*s += 1;
		i += 1;
	}

	strncpy(arg1, orginal, i);
	arg1[i] = '\0';

	if (arg1 == "")
	{
		(*instruction).Arg1 = "";
		return true;
	}

	(*instruction).Arg1 = arg1;

	skip_whitespaces(s);
	orginal2 = *s;

	while (*s[0] != ' ' && *s[0] != '\n' && *s[0] != '\0' && isprint(*s[0]) && *s[0] != ',')
	{ *s += 1; 	j += 1;	}

	skip_whitespaces(s);

	strncpy(arg2, orginal2, j);
	arg2[j] = '\0';

	// is it necessary?
	if (arg2 == "")
	{
		(*instruction).Arg2 = "";
		return true;
	}

	(*instruction).Arg2 = arg2;
	return true;
}

bool parse_formula(char* s, Command *instruction)
{
	skip_whitespaces(&s);
	char* original = s;

	if (!isprint(s[0]))
	{
		(*instruction).Label = NULL;
		(*instruction).Instruction = NULL;
		(*instruction).Arg1 = NULL;
		(*instruction).Arg2 = NULL;

		return false;
	}

	labels(&s, instruction);
	if (!instructions(&s, instruction))
		return false;

	if (!arguments(&s, instruction))
		return false;

	s = original;
	return true;
}

CommandInfo parse_file(char* content) {
	char* line = malloc(MAX_LINE_SIZE);

	int command_no = 0;
	int j = 0;
	int i = 0;
	CommandInfo commandInfo;
	commandInfo.commands = NULL;
	commandInfo.commands_no = -1;

	// instructions
	// int x = count_occurencies(content, '\n')  + 1;

	Command* commands = malloc(1 * sizeof(Command));

	if (commands == NULL ||  line == NULL)
		return commandInfo;

	for (i = 0; i < strlen(content); i++)
	{
		printf("%c", content[i]);

		if (content[i] == '\n')
		{
			line[j] = '\0';
			line = replace_char(line, ',', ' ');
			Command* tmp = realloc(commands, (command_no + 2) * sizeof(Command));

			if (tmp == NULL)
				tmp = NULL;
			
			else
				commands = tmp;

			if (parse_formula(line, &(commands[command_no])))
				command_no += 1;
			j = 0;
			//free(line);
			//line = malloc(MAX_LINE_SIZE);
		}

		else if (content[i] < 0)
			break;

		else 
		{
			line[j] = content[i];
			j += 1;
		}
	}

	free(content);

	line[j] = '\0';
	line = replace_char(line, ',', ' ');

	if (parse_formula(line, &commands[command_no]))
		command_no += 1;

	free(line);
	Command* tmp2;
	tmp2 = realloc(commands, (command_no * sizeof(Command)));

	if (tmp2 != NULL)
	{
		commands = tmp2;
	}

	// for (int i = 0; i < count_occurencies(content, '\n')  + 1 ; i++) {
	// 	if (commands[i].Instruction	!= NULL)
	// 		printf("%s %s %s %s\n", commands[i].Label, commands[i].Instruction, commands[i].Arg1, commands[i].Arg2);
	// }

	commandInfo.commands = commands;
	commandInfo.commands_no = command_no;

	return commandInfo;
}

char* read_file(char* fileName)
{
	FILE *file;
	int fsize = 0;
	char *fileContent;

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		printf("Cannot read file");
		return 1;
	}

	// get size of file
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	//allocation of memory
	fileContent = malloc(fsize + 1);

	// cannot allocate memory
	if (fileContent == NULL)
		return 2;

	fread(fileContent, sizeof(char), fsize, file);
	fileContent[fsize] = 0;


	fclose(file);
	return fileContent;
}