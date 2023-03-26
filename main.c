#include <stdio.h>
#include <stdbool.h>

typedef unsigned char uint8;

uint8 instruction_counter = 0;
uint8 reg[8] = {}; //reg6 = in; reg7 = out;

// Takes the values from reg1, reg2 and performs the operation on them.
// Stores the result in reg3.
uint8 alu(uint8 input) {
	uint8 operation = input << 5; // get only the 3 lowest bits by shifting by 5 left and the right
	operation >>= 5;

	switch (operation) {
		case 0:
			printf("Calculation: OR\n");
			reg[3] = (uint8)(reg[1] | reg[2]);
			break;
		case 1:
			printf("Calculation: NAND\n");
			reg[3] = (uint8)~(reg[1] & reg[2]);
			break;
		case 2:
			printf("Calculation: NOR\n");
			reg[3] = (uint8)~(reg[1] | reg[2]);
			break;
		case 3:
			printf("Calculation: AND\n");
			reg[3] = (uint8)(reg[1] & reg[2]);
			break;
		case 4:
			printf("Calculation: ADD\n");
			reg[3] = (uint8)(reg[1] + reg[2]);
			break;
		case 5:
			printf("Calculation: SUB\n");
			reg[3] = (uint8)(reg[1] - reg[2]);
			break;
		default:
			printf("Calculation: unused\n");
			break;
	}

	return 0;
}

// Checks if register 3 meets the given condition defined by the lowest 3 bits of the input.
// It overwrites the condition counter with the value of reg0 if the condition is met.
void condition(uint8 input) {
	uint8 cond = input << 5;
	cond >>= 5; // get only the 3 lowest bits by shifting by 5 left and the right
	bool condition_met;

	switch (cond) {
		case 0:
			printf("Condition: never\n");
			condition_met = false;
			break;
		case 1:
			printf("Condition: = 0\n");
			condition_met = reg[3] == 0;
			break;
		case 2:
			printf("Condition: < 0\n");
			condition_met = reg[3] < 0;
			break;
		case 3:
			printf("Condition: <= 0\n");
			condition_met = reg[3] <= 0;
			break;
		case 4:
			printf("Condition: always\n");
			condition_met = true;
			break;
		case 5:
			printf("Condition: != 0\n");
			condition_met = reg[3] != 0;
			break;
		case 6:
			printf("Condition: >= 0\n");
			condition_met = reg[3] >= 0;
			break;
		case 7:
			printf("Condition: > 0\n");
			condition_met = reg[3] > 0;
			break;
		default:
			printf("Condition: undefined\n");
			condition_met = false;
			break;
	}

	// If condition is met, jump to instruction number stored in reg0
	if (condition_met) {
		instruction_counter = reg[0];
	}
}

// Writes the lowest 6 bits of the input to reg0
void direct(uint8 input) {
	uint8 value = (input << 2) >> 2; // get only the 6 lowest bits by shifting by 2 left and the right
	reg[0] = (uint8)value;
}

void copy(uint8 input) {
	// get bit 3-6 by shifting 2 left and 3 right
	uint8 source = (input << 2);
	source >>= 5; //TODO check why it works in direct function but not in copy regarding bit shifting

	// get only the 3 lowest bits by shifting by 5 left and the right
	uint8 dest = (input << 5);
	dest >>= 5;

	if (dest == 6) { // Overwrite destination index when writing to output
		dest = 7;
	}

	printf("Source: %0.8b (%i)\n", source, source);
	printf("Dest: %0.8b (%i)\n", dest, dest);

	reg[dest] = reg[source];
}

int decoder(uint8 input) {
	printf("Input: %0.8b (%i)\n", input, input);

	uint8 instruction = (input >> 6) & 0b11;
	switch (instruction) {
		case 0:
			printf("Instruction: direct\n");
			direct(input);
			break;
		case 1:
			printf("Instruction: calculate\n");
			alu(input);
			break;
		case 2:
			printf("Instruction: copy\n");
			copy(input);
			break;
		case 3:
			printf("Instruction: condition\n");
			condition(input);
			break;
		default:
			return 1;
	}

	for (uint8 i = 0; i < (uint8)sizeof(reg); i++) {
		printf("%i: %08b (%i)\n", i, reg[i], reg[i]);
	}
	printf("--------------------\n");

	return 0;
}

int main() {
	// This is a program that keeps incrementing the output until the input equals 1
	uint8 bruteforce[8] = {1, 129, 2, 68, 158, 154, 179, 193};

	for (instruction_counter = 0; instruction_counter < (uint8)sizeof(bruteforce); instruction_counter++) {
		decoder(bruteforce[instruction_counter]);
		if (reg[7] == 50) { // Check if output equals required value and set input accordingly
			reg[6] = 1;
		} else {
			reg[6] = 0;
		}
	}

	return 0;
}
