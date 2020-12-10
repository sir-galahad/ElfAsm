#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mnemonics.h"
#include "symbol.h"
#include "ctype.h"
int get_argint(const char *arg, int type, int addr, symbol *symbol_table)
{
	
	int output = 0;
	int count;
	char *endptr;
	symbol *sym;
	switch(type) {
		case none:
			output = -1;
			break;

		case gp_register:
			if(strlen(arg)!=2) {	
				fprintf(stdout,"argument not register\n");
				output = -1;
				break;
			}
			
			if(arg[0]!='r' && arg[0]!='R'){
				fprintf(stderr,"argument not register\n");
				output = -1;
				break;
			}
			
			if(arg[1] >= '0' && arg[1] <='9') output = arg[1] - '0';
			else if(arg[1] >= 'a' && arg[1] <='f') output = arg[1] - 'a' + 10;
			else if(arg[1] >= 'A' && arg[1] <='F') output = arg[1] - 'A' + 10;
			else {
				fprintf(stderr,"argument not register\n");
				output = -1;
				break;
			}

			break;	
		case byte:

			if(isalpha(arg[0])) {
				fprintf(stderr, "ERROR: 16 bit symbol passed as 8 bit argument\n");
				return -1;
			} else if(arg[0] == '^' || arg[0] == '_') {
				sym = symbol_search(symbol_table, &arg[1]);
				if(sym == NULL) {
					fprintf(stderr,"ERROR: symbol %s doesn't exist",&arg[1]);
					output = -1;
				} else {
					if(arg[0] == '^') output = (sym->address >> 8);
					if(arg[0] == '_') output = (sym->address & 0xff);
				}
				break;
			}
			
			output = strtol(arg, &endptr, 0);

			
			if(*endptr != '\0'){
				output = -1;
				fprintf(stderr,"argument not byte\n");
				break;
			}

			if(output > 255 || output < 0) {
				output = -1;
				fprintf(stderr,"argument not byte (out of range) \n");
			}
			break;

		case address:
			if(isalpha(arg[0])) {
				sym = symbol_search(symbol_table, arg);
				if(sym == NULL) {
					fprintf(stderr,"ERROR: symbol %s doesn't exist", arg);
					output = -1;
				} else {
					output = sym->address;
				}
				break;
			}

			output = strtol(arg, &endptr, 0);
			
			if(*endptr != '\0'){
				output = -1;
				fprintf(stderr,"argument not address\n");
			}
			
			if(output > 0xffff || output < 0) {
				output = -1;
				fprintf(stderr,"argument not address (out of range)\n");
			}
			break;
	}

	return output;
}

//we asssume the buffer has at least 3 bytes
int mnemonic_getbytes(mnemonic *mne, unsigned char *buffer, char *arg, int addr, symbol *symbol_table)
{
	int argint;
	
	if(mne->type!=none) {
		argint = get_argint(arg,mne->type,addr,symbol_table);
		if(argint < 0) return -1;
	}
	
	switch(mne->type) {
		case none:
			buffer[0] = mne->opbase;
			break;
		case gp_register:
			buffer[0] = (unsigned char)(mne->opbase | argint);
			break;
		case byte:
			buffer[0] = mne->opbase;
			buffer[1] = (unsigned char)(argint);
			break;
		case address:
			buffer[0] = mne->opbase;
			//masking the high byte shouldn't be necessary
			buffer[1] = (unsigned char)((argint>>8) & 0xff);
			buffer[2] = (unsigned char)(argint & 0xff);
	}
	return mne->opsize;
}


