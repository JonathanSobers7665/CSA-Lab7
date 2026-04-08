/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
// Please finish the following functions for lab 8.
// Lab 8 will perform the following functions:
//   1. Fetch the code stored in memory
//   2. Decode the code and prepare for the execution of the code.
//   3. Setup the execution function for CPU.

// Lab 9 will perform the following functions:
//   4. Execute the code stored in the memory and print the results.

#include "header.h"
#include "lab8header.h"

extern char *regNameTab[N_REG];

// PCRegister: global program counter, always points to the next instruction.
// Defined here once; declared extern in lab8header.h.
unsigned int PCRegister = 0;

// ---------------------------------------------------------------
// CPU: Main CPU loop.
// Fetches and decodes each instruction until machine code 0x00000000 is found.
// Lab 9 will add execution via CPU_Execution().
void CPU(char *mem){
    unsigned int machineCode = 0;
    unsigned char opcode = 0;

    // Set PC to the start of the code section (CODESECTION = 0x0000)
    PCRegister = CODESECTION;

    do{
        printf("\nPC:%x\n", PCRegister);

        // Step 1: Fetch the 32-bit instruction at the current PC
        machineCode = CPU_fetchCode(mem, PCRegister);

        // End of program: machine code 0 means no more instructions
        if (machineCode == 0)
            break;

        // Advance PC by 4 bytes before decode (one MIPS instruction = 4 bytes)
        PCRegister += 4;

        // Step 2: Decode the instruction - returns opcode or funct code
        opcode = CPU_Decode(machineCode);
        printf("Decoded Opcode is: %02X. \n", opcode);

        // Lab 9: uncomment when CPU_Execution is complete
        // CPU_Execution(opcode, machineCode, mem);

    }while (1);

    // Print final register state and data memory contents
    printRegisterFiles();
    printDataMemoryDump(mem);
}

// ---------------------------------------------------------------
// Lab 8 - Step 1: CPU_fetchCode
// Reads the 32-bit machine code word from memory at the given offset.
// Simply wraps read_dword() from memory.c.
unsigned int CPU_fetchCode(char *mem, int codeOffset){
    // read_dword returns the 4-byte word stored at mem + codeOffset
    return read_dword(mem, codeOffset);
}

// ---------------------------------------------------------------
// Lab 8 - Step 2: CPU_Decode
// Extracts and returns the identifying 6-bit code from a machine code word.
//
// MIPS formats:
//   R-type: bits[31:26] = 000000, bits[5:0] = function code  → return funct
//   I-type: bits[31:26] = opcode                             → return opcode
//   J-type: bits[31:26] = opcode                             → return opcode
//
// For R-type the opcode is always 0, so we return the function code instead
// so the caller can identify which R-type instruction it is.
unsigned char CPU_Decode(unsigned int machineCode){
    // Extract top 6 bits (bits 31-26)
    unsigned char opcode = (unsigned char)((machineCode >> 26) & 0x3F);

    if (opcode == 0x00){
        // R-type: return the 6-bit function code from bits 5-0
        unsigned char funct = (unsigned char)(machineCode & 0x3F);
        printf("R-type instruction detected. Function code: %02X\n", funct);
        return funct;
    }
    else {
        // I-type or J-type: opcode field directly identifies the instruction
        printf("I/J-type instruction detected. Opcode: %02X\n", opcode);
        return opcode;
    }
}

// ---------------------------------------------------------------
// Lab 9: CPU_Execution
// Executes the instruction identified by opcode/funct.
// NOTE: For Lab 8 this is a skeleton only - will be completed in Lab 9.
//
// Important: because CPU_Decode returns funct for R-type instructions,
// the add instruction uses funct=0x20, while lb uses opcode=0x20.
// To avoid a duplicate case clash in the switch, we store whether the
// instruction was R-type in the top bit of opcode via a convention:
// CPU_Execution receives the raw machineCode so it can re-check opcode bits.
void CPU_Execution(unsigned char opcode, unsigned int machineCode, char *mem){
    unsigned char rt = 0;
    unsigned char rs = 0;
    unsigned char rd = 0;
    // Re-extract the raw opcode from machineCode to distinguish R-type from I-type
    unsigned char rawOpcode = (unsigned char)((machineCode >> 26) & 0x3F);

    // Handle R-type instructions separately (rawOpcode == 0)
    if (rawOpcode == 0x00){
        // R-type: opcode parameter holds the funct code
        rd = (unsigned char)((machineCode & 0x0000F800) >> 11);
        rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
        rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
        switch(opcode){
            case 0b100000: // add: rd = rs + rt  (funct = 0x20)
                regFile[rd] = regFile[rs] + regFile[rt];
                if (DEBUG_CODE)
                    printf("add: regFile[%d](%s) = 0x%08X\n", rd, regNameTab[rd], regFile[rd]);
                break;
            default:
                printf("Unknown R-type funct: %02X  machineCode: %08X\n", opcode, machineCode);
                exit(3);
        }
        return;
    }

    // Handle I-type and J-type instructions
    switch (rawOpcode)
    {
        // la instruction (pseudo, custom opcode 0b101111 = 0x2F)
        // Operation: rt = immediate  (loads address into register)
        case 0b101111:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            regFile[rt] = machineCode & 0x0000FFFF; // load 16-bit address
            if (DEBUG_CODE){
                printf("la: regFile[%d](%s) = 0x%08X\n", rt, regNameTab[rt], regFile[rt]);
                printf("****** PC Register is %08X ******\n", PCRegister);
            }
            break;

        // lb: rt = sign_extend(Memory[rs + imm] : 1 byte)  opcode = 0x20
        case 0b100000:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF); // sign-extend 16-bit
                regFile[rt] = (signed char)*(mem + regFile[rs] + imm);
            }
            if (DEBUG_CODE)
                printf("lb: regFile[%d](%s) = 0x%08X\n", rt, regNameTab[rt], regFile[rt]);
            break;

        // lw: rt = Memory[rs + imm] : 4 bytes   opcode = 0x23
        case 0b100011:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                regFile[rt] = *(int *)(mem + regFile[rs] + imm);
            }
            if (DEBUG_CODE)
                printf("lw: regFile[%d](%s) = 0x%08X\n", rt, regNameTab[rt], regFile[rt]);
            break;

        // sw: Memory[rs + imm] = rt   opcode = 0x2B
        case 0b101011:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                *(int *)(mem + regFile[rs] + imm) = regFile[rt];
            }
            if (DEBUG_CODE)
                printf("sw: regFile[%d](%s) stored to mem[rs+%d]\n", rt, regNameTab[rt], (short)(machineCode & 0x0000FFFF));
            break;

        // addi: rt = rs + sign_extend(imm)   opcode = 0x08
        case 0b001000:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                regFile[rt] = regFile[rs] + imm;
            }
            if (DEBUG_CODE)
                printf("addi: regFile[%d](%s) = 0x%08X\n", rt, regNameTab[rt], regFile[rt]);
            break;

        // bge: if (rs >= rt) PC = target << 2   opcode = 0x06
        case 0b000110:
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            {
                int target = (machineCode & 0x0000FFFF) << 2;
                if (regFile[rs] >= regFile[rt]){
                    PCRegister = target; // branch taken
                    if (DEBUG_CODE)
                        printf("bge: branch taken, PC = 0x%08X\n", PCRegister);
                } else {
                    if (DEBUG_CODE)
                        printf("bge: branch not taken\n");
                }
            }
            break;

        // j: PC = target << 2   opcode = 0x02
        case 0b000010:
            {
                unsigned int target = (machineCode & 0x03FFFFFF) << 2;
                PCRegister = target;
                if (DEBUG_CODE)
                    printf("j: jump to PC = 0x%08X\n", PCRegister);
            }
            break;

        // Should never reach default when all instructions handled correctly
        default:
            printf("Wrong instruction! opcode: %02X  machineCode: %08X\n", rawOpcode, machineCode);
            exit(3);
            break;
    }
}

// ---------------------------------------------------------------
// Lab 8 - Step 3: printRegisterFiles
// Prints all N_REG registers (32 general purpose + lo + hi) showing
// the register name from regNameTab and its current value from regFile.
void printRegisterFiles(){
    int i;
    printf("\n========== Register File Dump ==========\n");
    for (i = 0; i < N_REG; i++){
        // Left-align register name in 8 chars, print value as 8-digit hex
        printf("%-8s = 0x%08X\n", regNameTab[i], (unsigned int)regFile[i]);
    }
    printf("========================================\n");
}

// ---------------------------------------------------------------
// Lab 8 - Step 4: printDataMemoryDump
// Dumps the first 256 bytes of the data section starting at DATASECTION (0x2000).
// Calls memory_dump() from memory.c which was completed in Lab 6.
void printDataMemoryDump(char *mem){
    printf("\n========== Data Section Memory Dump (offset 0x%04X) ==========\n", DATASECTION);
    memory_dump(mem, DATASECTION, 256);
    printf("==============================================================\n");
}
