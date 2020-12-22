#ifndef MNEMONICS_H
#define MNEMONICS_H

#include "symbol.h"
enum argtype {none, gp_register, byte, address, string};

typedef struct _mnemonic 
{
	char name[5];
	int opsize;
	int type;
	unsigned char opbase;
	int isshortbranch;
} mnemonic;

mnemonic *get_mnemonic_data(const char* name);

int parse_register(const char *str, int *reg);
int parse_byte(const char *str, int *byte, symbol *symbol_table);
int parse_address(const char *str, int *address, symbol *symbol_table);
int parse_string(const char *str, char *buffer, int len);

extern mnemonic mnemonics[];
#endif
