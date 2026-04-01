/**
 * @file mips_asm.h
 *
 * Definitions
 *
 * @author Yooseong Kim, Aug., 2010
 */
#ifndef MIPS_ASM_H
#define MIPS_ASM_H
#include <string.h>

extern int n_Label;
extern char *regNameTab[N_REG];

int findLabel(char *label);
int getRegNum(char *regName);
int addLabel(char *label, int segType, int offset);
int addData(char *token, char *buffer, int offset);
void addInst(char *inst, char *operands, int offset);

#endif