/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
// Lab 8 performs the following functions:
//   1. Fetch the code stored in memory
//   2. Decode the code and prepare for the execution of the code.
//   3. Setup the execution function for CPU.
// Lab 9 performs the following functions:
//   4. Execute the code stored in the memory and print the results.

#include "header.h"
#include "lab8header.h"

extern char *regNameTab[N_REG];

// PCRegister: global program counter, always points to the next instruction.
// Defined here once; declared extern in lab8header.h.
unsigned int PCRegister = 0;

// ---------------------------------------------------------------
// CPU: Main CPU loop.
// Fetches, decodes, and executes each instruction until
// machine code 0x00000000 is found (end of program).
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

        // Advance PC by 4 bytes before decode/execute
        PCRegister += 4;

        // Step 2: Decode - returns opcode for I/J-type, funct code for R-type
        opcode = CPU_Decode(machineCode);
        printf("Decoded Opcode is: %02X. \n", opcode);

        // Step 3: Execute the instruction (Lab 9)
        CPU_Execution(opcode, machineCode, mem);

    }while (1);

    // Print final register state and data memory after execution
    printRegisterFiles();
    printDataMemoryDump(mem);
}

// ---------------------------------------------------------------
// Lab 8 - Step 1: CPU_fetchCode
// Reads the 32-bit machine code word from memory at the given offset.
unsigned int CPU_fetchCode(char *mem, int codeOffset){
    return read_dword(mem, codeOffset);
}

// ---------------------------------------------------------------
// Lab 8 - Step 2: CPU_Decode
// Extracts and returns the identifying 6-bit code from a machine code word.
// R-type: top 6 bits = 000000, so return function code (bits 5-0) instead.
// I/J-type: return top 6 bits (opcode) directly.
unsigned char CPU_Decode(unsigned int machineCode){
    unsigned char opcode = (unsigned char)((machineCode >> 26) & 0x3F);

    if (opcode == 0x00){
        unsigned char funct = (unsigned char)(machineCode & 0x3F);
        printf("R-type instruction detected. Function code: %02X\n", funct);
        return funct;
    }
    else {
        printf("I/J-type instruction detected. Opcode: %02X\n", opcode);
        return opcode;
    }
}

// ---------------------------------------------------------------
// Lab 9: CPU_Execution
// Executes the instruction. For R-type, opcode = funct code.
// For I/J-type, re-reads raw opcode from machineCode directly.
void CPU_Execution(unsigned char opcode, unsigned int machineCode, char *mem){
    unsigned char rt = 0;
    unsigned char rs = 0;
    unsigned char rd = 0;

    // Re-extract raw opcode bits[31:26] to tell R-type apart from I/J-type
    unsigned char rawOpcode = (unsigned char)((machineCode >> 26) & 0x3F);

    // ---- R-type instructions (rawOpcode == 0) ----
    if (rawOpcode == 0x00){
        rd = (unsigned char)((machineCode & 0x0000F800) >> 11);
        rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
        rt = (unsigned char)((machineCode & 0x001F0000) >> 16);

        switch(opcode){
            // add: rd = rs + rt
            // Parser stores funct=0 (not the official 0x20) so handle both
            case 0b000000:
            case 0b100000:
                regFile[rd] = regFile[rs] + regFile[rt];
                if (DEBUG_CODE)
                    printf("add: regFile[%d](%s) = 0x%08X\n",
                           rd, regNameTab[rd], regFile[rd]);
                break;
            default:
                printf("Unknown R-type funct: %02X  machineCode: %08X\n",
                       opcode, machineCode);
                exit(3);
        }
        return;
    }

    // ---- I-type and J-type instructions ----
    switch (rawOpcode)
    {
        // la: rt = DATASECTION + immediate  (pseudo, opcode 0x2F)
        case 0b101111:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            regFile[rt] = (machineCode & 0x0000FFFF) + DATASECTION;
            if (DEBUG_CODE)
                printf("la: regFile[%d](%s) = 0x%08X\n",
                       rt, regNameTab[rt], regFile[rt]);
            break;

        // lb: rt = sign_extend(Memory[rs+imm]:1)  opcode 0x20
        case 0b100000:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                regFile[rt] = (signed char)*(mem + regFile[rs] + imm);
            }
            if (DEBUG_CODE)
                printf("lb: regFile[%d](%s) = 0x%08X\n",
                       rt, regNameTab[rt], regFile[rt]);
            break;

        // lw: rt = Memory[rs+imm]:4  opcode 0x23
        case 0b100011:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                regFile[rt] = *(int *)(mem + regFile[rs] + imm);
            }
            if (DEBUG_CODE)
                printf("lw: regFile[%d](%s) = 0x%08X\n",
                       rt, regNameTab[rt], regFile[rt]);
            break;

        // sw: Memory[rs+imm]:4 = rt  opcode 0x2B
        case 0b101011:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                *(int *)(mem + regFile[rs] + imm) = regFile[rt];
            }
            if (DEBUG_CODE)
                printf("sw: mem[regFile[%d]+imm] = regFile[%d](%s) = 0x%08X\n",
                       rs, rt, regNameTab[rt], regFile[rt]);
            break;

        // addi: rt = rs + sign_extend(imm)  opcode 0x08
        case 0b001000:
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            {
                int imm = (short)(machineCode & 0x0000FFFF);
                regFile[rt] = regFile[rs] + imm;
            }
            if (DEBUG_CODE)
                printf("addi: regFile[%d](%s) = 0x%08X\n",
                       rt, regNameTab[rt], regFile[rt]);
            break;

        // bge: if (rs >= rt) PC = immediate<<2  opcode 0x06
        case 0b000110:
            rs = (unsigned char)((machineCode & 0x03E00000) >> 21);
            rt = (unsigned char)((machineCode & 0x001F0000) >> 16);
            {
                unsigned int target = (machineCode & 0x0000FFFF) << 2;
                if ((int)regFile[rs] >= (int)regFile[rt]){
                    PCRegister = target;
                    if (DEBUG_CODE)
                        printf("bge: TAKEN, PC=0x%08X\n", PCRegister);
                } else {
                    if (DEBUG_CODE)
                        printf("bge: NOT taken\n");
                }
            }
            break;

        // j: PC = target<<2  opcode 0x02
        case 0b000010:
            {
                unsigned int target = (machineCode & 0x03FFFFFF) << 2;
                PCRegister = target;
                if (DEBUG_CODE)
                    printf("j: jump to PC=0x%08X\n", PCRegister);
            }
            break;

        default:
            printf("Wrong instruction! opcode:%02X machineCode:%08X\n",
                   rawOpcode, machineCode);
            exit(3);
            break;
    }
}

// ---------------------------------------------------------------
// Lab 8 - Step 3: printRegisterFiles
// Prints all N_REG registers with name and current hex value.
void printRegisterFiles(){
    int i;
    printf("\n========== Register File Dump ==========\n");
    for (i = 0; i < N_REG; i++){
        printf("%-8s = 0x%08X\n", regNameTab[i], (unsigned int)regFile[i]);
    }
    printf("========================================\n");
}

// ---------------------------------------------------------------
// Lab 8 - Step 4: printDataMemoryDump
// Dumps first 256 bytes of the data section (offset 0x2000).
// Writes directly without calling memory_dump() to avoid its
// internal getchar() which swallows input and cuts off the output.
void printDataMemoryDump(char *mem){
    unsigned char printout;
    int i, j;
    unsigned int offset = DATASECTION;
    unsigned int dumpsize = 256;

    printf("\n===== Data Section Memory Dump (offset 0x%04X) =====\n", offset);
    printf("\n");

    for (i = 0; i < (int)(dumpsize / DUMP_LINE); i++){
        // Print address
        printf("%X: ", (unsigned int)(mem + offset + i * DUMP_LINE));
        // Print hex bytes
        for (j = 0; j < DUMP_LINE; j++){
            printout = (unsigned char)(*(mem + offset + i * DUMP_LINE + j));
            printf("%02X ", printout);
        }
        printf(" --- ");
        // Print ASCII representation
        for (j = 0; j < DUMP_LINE; j++){
            printout = (unsigned char)(*(mem + offset + i * DUMP_LINE + j));
            if (printout > 0x20 && printout < 0x7E)
                printf("%c ", printout);
            else
                printf(". ");
        }
        printf("\n");
    }
    printf("====================================================\n");
}
