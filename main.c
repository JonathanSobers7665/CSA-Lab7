/*
 ============================================================================
 Name        : main.c
 Author      : Austin Tian
 Revised by  :
 Version     :
 Copyright   : Copyright 2023
 Description : main code in C
 ============================================================================
 */
#include "header.h"

int main (int argc, char *argv[]) {
        // Don't touch the commented code until told so in lab 7

        char *mem = NULL;  //  memory space pointer.
        FILE *fp = NULL;
        int stopChar;
        // Step 1: Parse the ASM code.
        if (argc < 2) {
            puts("\nIncorrect number of arguments.");
            puts("Usage: ProgramName.exe MIPSCode.asm \n");
            stopChar = getchar();
            return EXIT_FAILURE;
        }
        if ((fp = fopen(argv[1], "r"))== NULL) {
                printf("Input file could not be opened.");
                stopChar = getchar();
                return EXIT_FAILURE;
        }
        parse_MIPS(fp);  // parse the ASM into Data_storage, Instruction_storage, labelTab

        // Step 2: Setup the memory (lab 6)
        mem = init_memory();  // initialize memory (filled with 0s)
        stopChar = getchar();

        // Step 3: Load the code into memory (lab 7)
        puts("----Lab 7 Code Starts to Parse the ASM Code----");
        loadCodeToMem(mem);
        stopChar = getchar();

        // Step 4 & 5: CPU Fetch and Decode (lab 8)
        puts("----Lab 8 Code Starts to Fetch and Decode the Code ----");
        CPU(mem);   // Fetch + Decode every instruction, print register file + data dump
        stopChar = getchar();

        // Step 6: Execute (lab 9 - uncomment when ready)
        // puts("----Lab 9 Code Starts to Execute the Code ----");

        free_memory(mem);  // free allocated memory
        return 0;
}
