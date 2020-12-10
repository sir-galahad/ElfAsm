#ifndef MNEMONICS_H
#define MNEMONICS_H

enum argtype {none, gp_register, byte, address};

typedef struct _mnemonic 
{
	char name[5];
	int opsize;
	int type;
	unsigned char opbase;
	int isshortbranch;
} mnemonic;

mnemonic *get_mnemonic_data(const char* name);


extern mnemonic mnemonics[];
#endif
