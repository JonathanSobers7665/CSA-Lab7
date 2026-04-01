/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "header.h"

// This is the function to fill in the data section.
void setupDataMemory(        char* base_address,
                             unsigned int offset,
                             char *datasection,
                             unsigned int numberOfBytes){
    // Copy each byte from datasection into memory starting at the given offset.
    unsigned int i;
    for (i = 0; i < numberOfBytes; i++) {
        write_byte(base_address, offset + i, (unsigned char)datasection[i]);
    }
    return;
}

// This is the function to build I type instruction based on Instruction_storage
unsigned int buildIInstruction(unsigned char opcode,
                               unsigned char rs,
                               unsigned char rt,
                               int immediate){
    unsigned int machineCode = 0;
    unsigned int mask = 0;
    machineCode = immediate & 0x0000FFFF;
    mask = ((unsigned int)(rs & 0x000000FF)) << 21;
    machineCode = machineCode | mask;
    mask = ((unsigned int)(rt & 0x000000FF)) << 16;
    machineCode = machineCode | mask;
    mask = ((unsigned int)(opcode & 0x000000FF)) << 26;
    machineCode = machineCode | mask;
    //  machineCode should be OK now.
    return machineCode;  // finally return a 32-bit machine code.
}

// This is the function to build J type instruction based on Instruction_storage
unsigned int buildJInstruction(unsigned char opcode,
                               int immediate){
     unsigned int machineCode = 0;
     unsigned int mask = 0;

     // J-type format: oooooo iiiiiiii iiiiiiii iiiiiiii iiiiiiii
     // Bits 25-0: the jump target address (26 bits)
     machineCode = immediate & 0x03FFFFFF;       // keep lower 26 bits of immediate
     mask = ((unsigned int)(opcode & 0x3F)) << 26;
     machineCode = machineCode | mask;

    return machineCode;  // finally return a 32-bit machine code.
}

// This is the function to build R type instruction based on Instruction_storage
unsigned int buildRInstruction(unsigned char opcode,
                               unsigned char rs,
                               unsigned char rt,
                               unsigned char rd,
                               unsigned char shamt,
                               unsigned char function){
     unsigned int machineCode = 0;
     unsigned int mask = 0;

     // R-type format: 000000ss sssttttt dddddaaa aaffffff
     // opcode (6) | rs (5) | rt (5) | rd (5) | shamt (5) | function (6)
     mask = ((unsigned int)(opcode  & 0x3F)) << 26;
     machineCode |= mask;
     mask = ((unsigned int)(rs      & 0x1F)) << 21;
     machineCode |= mask;
     mask = ((unsigned int)(rt      & 0x1F)) << 16;
     machineCode |= mask;
     mask = ((unsigned int)(rd      & 0x1F)) << 11;
     machineCode |= mask;
     mask = ((unsigned int)(shamt   & 0x1F)) << 6;
     machineCode |= mask;
     mask =  (unsigned int)(function & 0x3F);
     machineCode |= mask;

    return machineCode;  // finally return a 32-bit machine code.
}

// This is the function to store the instructions.
void setupInstructionMemory( char* base_memory_address,
                                     int codeOffset,  // this is the code index.
                                     MIPS_Instruction *instructionStorage){
    int i = 0;
    unsigned int totalinstruction = 0;
    unsigned char opcode = 0;
    unsigned int machineCode = 0;
    do {
        // if la instruction.
        if (strcmp(instructionStorage[i].instruction, "la") == 0 ){
        // Notice: This instruction is already done as an example.

        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs, rt and immediate number as address.

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b101111  (custom pseudo-instruction opcode used by this simulator)

            // Then combine the answer above and finish this part of the code.
            opcode = 0b101111;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);

            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "lb") == 0 ){// if lb instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs (base register), rt (destination register), immediate (byte offset)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b100000  (from MIPS reference: lb opcode = 100000)

            opcode = 0b100000;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "bge") == 0 ){// if bge instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs, rt, immediate (branch offset)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b000110 (bge is a pseudo-instruction; simulator encodes it as
        //          an I-type with two registers and a branch offset)

            opcode = 0b000110;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "lw") == 0 ){// if lw instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs (base register), rt (destination register), immediate (word offset)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b100011  (from MIPS reference: lw opcode = 100011)

            opcode = 0b100011;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "sw") == 0 ){// if sw instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs (base register), rt (source register to store), immediate (word offset)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b101011  (from MIPS reference: sw opcode = 101011)

            opcode = 0b101011;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "add") == 0 ){// if add instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  R instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  000000ss sssttttt dddddaaa aaffffff
        // needs:   opcode=0, rs, rt, rd, shamt=0, function code

        // Thirdly, what is the opcode of this instruction?
        // Answer:  opcode = 0b000000 (all R-type instructions have opcode 0)
        //          function code for add = 0b100000  (from MIPS reference)

            opcode = 0b000000;
            machineCode = buildRInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].rd,
                                            instructionStorage[i].shamt,
                                            instructionStorage[i].function);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "addi") == 0 ){// if addi instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  I instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  ooooooss sssttttt iiiiiiii iiiiiiii
        // needs:   opcode, rs (source register), rt (destination register), immediate (signed 16-bit value)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b001000  (from MIPS reference: addi opcode = 001000)

            opcode = 0b001000;
            machineCode = buildIInstruction(opcode,
                                            instructionStorage[i].rs,
                                            instructionStorage[i].rt,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "j") == 0 ){// if j instruction.
        // First, Answer this: what type of instruction is this? J, R or I instruction?
        // Answer:  J instruction

        // Secondly, what kind of machine language data format should it be and what registers/numbers are needed?
        // Answer:  oooooo iiiiiiii iiiiiiii iiiiiiii iiiiiiii
        // needs:   opcode and 26-bit jump target address (immediate)

        // Thirdly, what is the opcode of this instruction?
        // Answer:  0b000010  (from MIPS reference: j opcode = 000010)

            opcode = 0b000010;
            machineCode = buildJInstruction(opcode,
                                            instructionStorage[i].immediate);
            write_dword(base_memory_address, codeOffset+i*4, machineCode);
        }
        else if (strcmp(instructionStorage[i].instruction, "syscall") == 0 || // if end of the code
                 (strcmp(instructionStorage[i].instruction, "END") == 0 ))
        {
            // No need to worry about this. This is the end of code indicator.
            break;  // exit here.
        }
        i++;
    }while (1);
    totalinstruction = i;
}

// load the code into the memory starts from offset.
void loadCodeToMem(char *mem){
    // Finish this function in Lab 7. This lab would
    //  1. Parse the MIPS ASM file with parse_MIPS(); (this is already done )
    //     in main function. Data is already stored in array Data_storage[];
    //     Code is already stored into array Instruction_storage[].
    //  2. Store the data section into memory space starts from offset 0x2000 (8K)
    //  2. Convert all the instructions into 32-bit binary code based on MIPS instruction format.
    //  3. Store the code into memory starts from offset 0x0000. (beginning of the memory space)
    unsigned int dataSection = DATASECTION;
    unsigned int instructionSection = CODESECTION;
    // Starting from point 2 above.
    // Step 1: refer to the code in memory.c. Write Data_storage into memory space starting from 0x2000.
    setupDataMemory(mem, dataSection, Data_storage, totalDataByte);
    // Step 2: Convert all the instructions stored in
    //         Instruction_storage[] to binary code and store them into the memory starts at 0x0000
    // Just need to compile to following instructions:
    //         la, add, lb, bge, lw, sw, addi, j
    setupInstructionMemory(mem, instructionSection, Instruction_storage);

    // Memory dump the information on the screen.
    puts("\n---- Data Section ----\n");
    memory_dump(mem, DATASECTION, 256);
    puts("\n---- Code Section ----\n");
    memory_dump(mem, CODESECTION, 256);
}