#include <stdio.h>
#include <stdbool.h>

typedef unsigned char uint8;

uint8 instruction_index = 0;
uint8 reg[6] = {};
uint8 in = 0;
uint8 out = 0;

// Takes the values from reg1, reg2 and performs the operation on them.
// Stores the result in reg3.
void alu(uint8 input) {
	uint8 operation = input & 0b111; // get bits 1-3 (reading from right to left. Bit order: 87654321)

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
}

// Checks if register 3 meets the given condition defined by the lowest 3 bits of the input.
// It overwrites the condition counter with the value of reg0 if the condition is met.
void condition(uint8 input) {
	uint8 cond = input & 0b111; // get bits 1-3
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
		instruction_index = reg[0];
	}
}

// Writes the lowest 6 bits of the input to reg0.
void direct(uint8 input) {
	uint8 value = input & 0b111111; // get bits 1-6
	reg[0] = (uint8)value;
}

// Copies the a value from one register to another with bit 1-3 being the destination and 4-6 the source.
void copy(uint8 input) {
	uint8 src = (input >> 3) & 0b111; // get bits 4-6
	uint8 dest = input & 0b111; // get bits 1-3
	uint8 *src_reg, *dest_reg;

	printf("Source: %0.8b (%i)\n", src, src);
	printf("Dest: %0.8b (%i)\n", dest, dest);

	if (src == 6) {
		src_reg = &in;
	} else {
		src_reg = &reg[src];
	}
	if (dest == 6) {
		dest_reg = &out;
	} else {
		dest_reg = &reg[dest];
	}

	*dest_reg = *src_reg;
}

// Takes the highest 2 bits of an instruction to get the code for the operation and executes it.
void decoder(uint8 input) {
	printf("Input: %0.8b (%i)\n", input, input);
	printf("Instruction index: %i (max. 255)\n", instruction_index);

	uint8 op_code = (input >> 6) & 0b11; // get bits 7-8
	switch (op_code) {
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
			printf("Instruction: undefined\n");
			break;
	}

	for (uint8 i = 0; i < (uint8)sizeof(reg); i++) {
		printf("reg%i: %08b (%i)\n", i, reg[i], reg[i]);
	}
	printf("in:   %0.8b (%i)\n", in, in);
	printf("out:  %0.8b (%i)\n", out, out);
	printf("--------------------\n");
}

int main() {
	// This is a program that keeps incrementing the output until the input equals 1
	uint8 bruteforce[8] = {1, 129, 2, 68, 158, 154, 179, 193};

	for (instruction_index = 0; instruction_index < (uint8)sizeof(bruteforce); instruction_index++) {
		decoder(bruteforce[instruction_index]);
		if (out == 50) { // Check if output equals required value and set input accordingly
			in = 1;
		} else {
			in = 0;
		}
	}

	return 0;
}
