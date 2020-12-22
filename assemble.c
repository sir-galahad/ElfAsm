#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mnemonics.h"
#include "symbol.h"
#include "ctype.h"
#include "evaluate.h"

int get_argint(const char *arg, mnemonic *mne, int addr, symbol *symbol_table)
{
	
	int output = 0;
	int count;
	char *endptr;
	int argint;
	symbol *sym;
	char reg_buf[20];
	sscanf(arg, "%s", reg_buf);

	switch(mne->type) {
		case none:
			output = -1;
			break;

		case gp_register:
			if(strlen(reg_buf)!=2) {	
				fprintf(stdout,"argument not register\n");
				output = -1;
				break;
			}
			
			if(reg_buf[0]!='r' && reg_buf[0]!='R'){
				fprintf(stderr,"argument not register\n");
				output = -1;
				break;
			}
			
			if(reg_buf[1] >= '0' && reg_buf[1] <='9') output = reg_buf[1] - '0';
			else if(reg_buf[1] >= 'a' && reg_buf[1] <='f') output = reg_buf[1] - 'a' + 10;
			else if(reg_buf[1] >= 'A' && reg_buf[1] <='F') output = reg_buf[1] - 'A' + 10;
			else {
				fprintf(stderr,"argument not register\n");
				output = -1;
			}

			break;	
	
		default:
			if(evaluate(arg, &output, symbol_table) < 0) {
				return -1;
			}
			if(mne->type ==byte){
				if(output > 255 || output < 0) {
					output = -1;
					fprintf(stderr,"argument not byte (out of range) \n");
				}
			}
			if(mne->type == address){
				if(output > 0xffff || output < 0) {
					output = -1;
					fprintf(stderr,"argument not address (out of range)\n");
				}
			}

			break;
	}

	return output;
}

//we asssume the buffer has at least 3 bytes
int mnemonic_getbytes(mnemonic *mne, unsigned char *buffer, char *arg, int addr, symbol *symbol_table)
{
	int argint;
	int index;
	int size;
		
	index = 0;

	switch(mne->type) {
		case none:
			buffer[0] = mne->opbase;
			break;
		case gp_register:
			if(parse_register(arg, &argint) < 0){
				return -1;
			}
			if(buffer != NULL){
				buffer[0] = (unsigned char)(mne->opbase | argint);
			}
			break;

		case byte:
			if( parse_byte(arg, &argint, symbol_table) <0) {
				return -1;
			}
			if(strcmp(mne->name, "DB")) { //if not DB include the base
				buffer[index++] = mne->opbase;
			}
			if(buffer != NULL) {
				buffer[index++] = (unsigned char)(argint);
			}
			break;

		case address:
			if( parse_address(arg, &argint, symbol_table) <0) {
				return -1;
			}
			if(strcmp(mne->name, "DD")) { //if not DD include the base
				buffer[index++] = mne->opbase;
			}
			//masking the high byte shouldn't be necessary
			if(buffer != NULL) {
				buffer[index++] = (unsigned char)((argint>>8) & 0xff);
				buffer[index++] = (unsigned char)(argint & 0xff);
			}
			break;

		case string:
			//only one psuedo-op uses string for an argument currently
			if(!strcmp(mne->name, "DS")) { //if not DD include the base
				if((size = parse_string(arg, buffer, 200)) <0) {
					return -1;
				
				} else return size;
			}
			break;
	}
	return mne->opsize;
}


