#include <stdio.h>

#include "InstructionSet.h"


#define instruction_size 22
char* InstructionSet[instruction_size] = { 
    "DS", "DC",
    "A", "AR",
    "S", "SR",
    "M", "MR", 
    "D", "DR",
    "C", "CR",
    "L", "LR", "LA", "ST",
    "J", "JZ", "JP", "JN",
	// RD = read
    "RD", "PRT"
    };