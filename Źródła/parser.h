#include <stdbool.h>
#include "InstructionSet.h"

CommandInfo parse_file(char* content);
char* read_file(char* fileName);

// helper functions
void skip_whitespaces(char* s[]);
bool match(char* s[], const char *token);
bool matches_defined_instruction(char* s);

// Parsing functions
// Formula ::= <label> <instruction> <arguments>
// <label> ::= any literal string [a-zA-Z]
// <instruction> ::= string literal which matches one of instruction from InstructionSet
// <arguments> ::= <argument1> <argument2>
// <argument1> ::= [a-z0-9]
// <argument2> ::= [a-z0-9]

// Identify a instruction
bool parse_formula(char* s, Command *instruction);
bool labels(char* s[], Command *instruction);
bool instructions(char* s[], Command *instruction);
bool arguments(char* s[], Command *instruction);