/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lab8header.h
 * Author:
 *
 */

#ifndef LAB8HEADER_H
#define LAB8HEADER_H

unsigned int CPU_fetchCode(char *mem, int codeOffset);
unsigned char CPU_Decode(unsigned int machineCode);
void CPU_Execution(unsigned char opcode, unsigned int machineCode, char *mem);
void printRegisterFiles();
void printDataMemoryDump(char *mem);  /* FIX: added char *mem parameter to match CPU() call */

/* PCRegister defined once in lab8_9.c, declared extern everywhere else */
extern unsigned int PCRegister;

#endif /* LAB8HEADER_H */
