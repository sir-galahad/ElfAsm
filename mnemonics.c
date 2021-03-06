#include "mnemonics.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "evaluate.h"

mnemonic *get_mnemonic_data(const char* name)
{
	int i;
	mnemonic *mne = NULL;
	for(i=0; mnemonics[i].name[0] != '\0'; i++) {
		if(strncmp(mnemonics[i].name, name, 4) == 0) {
			mne=&mnemonics[i];
				break;
		}
	}

	return mne;
}


mnemonic mnemonics[] = 
{	//NOP!
	{"NOP",1,none,0xc4,0},

	//register and memory loading/storing
	{"INC",1,gp_register,0x10,0},
	{"DEC",1,gp_register,0x20,0},
	{"IRX",1,none,0x60,0},
	{"GLO",1,gp_register,0x80,0},
	{"GHI",1,gp_register,0x90,0},
	{"PLO",1,gp_register,0xa0,0},
	{"PHI",1,gp_register,0xb0,0},
	{"LDN",1,gp_register,0x00,0},
	{"LDA",1,gp_register,0x40,0},
	{"LDX",1,none,0xf0,0},
	{"LDXA",1,none,0x72,0},
	{"LDI",2,byte,0xf8,0},
	{"STR",1,gp_register,0x50,0},
	{"STXD",1,none,0x73,0},

	//short branches
	{"BR",2,byte,0x30,1},
	{"BQ",2,byte,0x31,1},
	{"BZ",2,byte,0x32,1},
	{"BDF",2,byte,0x33,1},
	{"BPZ",2,byte,0x33,1},
	{"BGE",2,byte,0x33,1},
	{"B1",2,byte,0x34,1},
	{"B2",2,byte,0x35,1},
	{"B3",2,byte,0x36,1},
	{"B4",2,byte,0x37,1},
	{"NBR",2,byte,0x38,1},
	{"BNQ",2,byte,0x39,1},
	{"BNZ",2,byte,0x3a,1},
	{"BNF",2,byte,0x3b,1},
	{"BM",2,byte,0x3b,1},
	{"BL",2,byte,0x3b,1},
	{"BN1",2,byte,0x3c,1},
	{"BN2",2,byte,0x3d,1},
	{"BN3",2,byte,0x3e,1},
	{"BN4",2,byte,0x3f,1},
	
	//long branches
	{"LBR",3,address,0xc0,0},
	{"LBQ",3,address,0xc1,0},
	{"LBZ",3,address,0xc2,0},
	{"LBDF",3,address,0xc3,0},
	{"LBPZ",3,address,0xc3,0},
	{"LBGE",3,address,0xc3,0},
	{"NLBR",3,address,0xc8,0},
	{"LBNQ",3,address,0xc9,0},
	{"LBNZ",3,address,0xca,0},
	{"LBNF",3,address,0xcb,0},

	//skips
	{"LSNQ",1,none,0xc5,0},
	{"LSNZ",1,none,0xc6,0},
	{"LSNF",1,none,0xc7,0},
	{"LSIE",1,none,0xcc,0},
	{"LSQ",1,none,0xcd,0},
	{"LSZ",1,none,0xce,0},
	{"LSDF",1,none,0xcf,0},

	//ALU operations
	{"OR",1,none,0xf1,0},
	{"AND",1,none,0xf2,0},
	{"XOR",1,none,0xf3,0},
	{"ADD",1,none,0xf4,0},
	{"ADC",1,none,0x74,0},
	{"SD",1,none,0xf5,0},
	{"SDB",1,none,0x75,0},
	{"SM",1,none,0xf7,0},
	{"SMB",1,none,0x77,0},
	{"ORI",2,byte,0xf9,0},
	{"ANI",2,byte,0xfa,0},
	{"XRI",2,byte,0xfb,0},
	{"ADI",2,byte,0xfc,0},
	{"ADCI",2,byte,0x7c,0},
	{"SDI",2,byte,0xfd,0},
	{"SDBI",2,byte,0x7d,0},
	{"SMI",2,byte,0xff,0},
	{"SMBI",2,byte,0x7f,0},
	{"SHR",1,none,0xf6,0},
	{"SHL",1,none,0xfe,0},
	{"SHRC",1,none,0x76,0},
	{"RSHR",1,none,0x76,0},
	{"SHLC",1,none,0x7e,0},
	{"RSHL",1,none,0x7e,0},
	
	//CPU control	
	{"IDL",1,none,0x00,0},
	{"RET",1,none,0x70,0},
	{"DIS",1,none,0x71,0},
	{"MARK",1,none,0x79,0},
	{"SAV",1,none,0x78,0},
	{"REQ",1,none,0x7a,0},
	{"SEQ",1,none,0x7b,0},
	{"SEP",1,gp_register,0xd0,0},
	{"SEX",1,gp_register,0xe0,0},

	//IO	
	{"OUT1",1,none,0x61,0},
	{"OUT2",1,none,0x62,0},
	{"OUT3",1,none,0x63,0},
	{"OUT4",1,none,0x64,0},
	{"OUT5",1,none,0x65,0},
	{"OUT6",1,none,0x66,0},
	{"OUT7",1,none,0x67,0},
	{"IN1",1,none,0x69,0},
	{"IN2",1,none,0x6a,0},
	{"IN3",1,none,0x6b,0},
	{"IN4",1,none,0x6c,0},
	{"IN5",1,none,0x6d,0},
	{"IN6",1,none,0x6e,0},
	{"IN7",1,none,0x6f,0},
	
	//Pseudo OPs
	
	{"ORG",0,address,0,0},
	{"DB",1,byte,0,0},
	{"DD",2,address,0,0},
	{"DS",-1,string,0,0},
	//Mark the end of the array
	{"\0",0,none,0,0}

};

int parse_register(const char *str, int *reg)
{
	int i, regnum;
	int stlen = strlen(str);
	
	for(i = 0; i < stlen; i++) {
		if( str[i] != ' ' && str[i] != '\t') { //there are other whitespace symbols but...
			break;
		}
	}
	
	//check first non-whitespace char is r or R
	if(i >= stlen || (str[i]!='r' && str[i]!='R')){
		fprintf(stderr, "ERROR: register expected\n");
		return -1;
	}

	i++;
	regnum = toupper(str[i]);

	if( (regnum >= '0' && regnum <='9') || (regnum >= 'A' && regnum <= 'F') ) {
		if(regnum >= '0' && regnum <='9') {
			regnum -= '0';
		} else if(regnum >= 'A' && regnum <= 'F') {
			regnum = (regnum - 'A') + 10;
		}
	} else {
		fprintf(stderr, "ERROR: register expected\n");
	}
	
	for(i++ ;i < stlen; i++) {
		if( str[i] == ';') break; //if we hit the start of a comment it's over
		if( str[i] != ' ' && str[i] != '\t' && str[i] != '\n') { //there are other whitespace symbols but...
			fprintf(stderr, "ERROR: extra character %c after register found\n", str[i]);
			return -1;
		}
	}
	*reg = regnum;
	return regnum;
}

int parse_byte(const char *str, int *byte, symbol *symbol_table)
{
	return evaluate(str,byte,symbol_table);
}

int parse_address(const char *str, int *address, symbol *symbol_table)
{
	return evaluate(str,address,symbol_table);
}

int parse_string(const char *str, char *buffer, int len) 
{
	int i, bi, closed = 0;
	int stlen = strlen(str);

	for(i = 0; i < stlen; i++) {
		if( str[i] != ' ' && str[i] != '\t') { //there are other whitespace symbols but...
			break;
		}
	}
	
	//check first non-whitespace char is r or R
	if(i >= stlen || str[i]!='"'){
		fprintf(stderr, "ERROR: string expected\n");
		return -1;
	}

	for( i++, bi = 0; i< stlen; i++, bi++) {
		if( bi==len){
			fprintf(stderr, "ERROR: string exceeds buffer length\n");
			return -1;
		}
		if(str[i] == '"') {
			closed = 1;
			break;
		}

		if(str[i] == '\\' && buffer != NULL) {
			i++;
			if(str[i] == '"') buffer[bi] = '"';
			if(str[i] == '\\') buffer[bi] = '\\';
			if(str[i] == 't') buffer[bi] = '\t';
			if(str[i] == 'n') buffer[bi] = '\n';
			//probably other escapes i should add, but i can't be bothered right now.
		}else{

			if(buffer != NULL) buffer[bi] = str[i];
		}
	}
	if(!closed) {
		fprintf(stderr, "ERROR: string unclosed at end of line\n");
		return -1;
	}

	for( i++; i < stlen; i++) {
		if( str[i] == ';') break; //if we hit the start of a comment it's over
		if( str[i] != ' ' && str[i] != '\t' && str[i] != '\n') { //there are other whitespace symbols but...
			fprintf(stderr, "ERROR: extra characters after string\n");
			return -1;
		}
	}
	return bi;
}
