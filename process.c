#include <stdio.h>
#include <string.h>
#include "mnemonics.h"
#include <ctype.h>
#include "symbol.h"
#include "assemble.h"
typedef struct _linedata
{
	char *label;
	char *mnemonic;
	char *arg;
} linedata;

int split_line(char *line, linedata *data)
{
	int len,i,endline=0;
	char *start;
	int inword = 0;
	len = strlen(line);
	for(i = 0; i < len; i++) {
		
		if(inword == 0){
			start=&line[i];
		}
		
		if(isalnum(line[i]) || line[i]=='^' || line[i]=='_'){
			inword = 1;
		}

		if(line[i] == '\0' || line[i] == ';') {
			line[i]='\0';
			endline = 1;
		}

		if(inword && !isalnum(line[i]) && line[i] != '_' && line[i] != '^'){
			if(data->mnemonic == NULL && line[i] == ':'){
				data->label = start;
				line[i]='\0';
			} else if(data->mnemonic == NULL) {
				data->mnemonic = start;
				line[i]='\0';
			} else if(data->arg[0] == '\0') {
				data->arg = start;
				line[i]='\0';
			} else {
				fprintf(stderr,"Unable to parse line: too many elements\n");
				return -1;
			}

			inword = 0;
		}
		
		if(endline == 1) break;
	}

	return 1;
}

symbol *symbol_table = NULL;
int process(const char* inputfile, const char* outputfile)
{	
	FILE *output = fopen(outputfile,"w");
	char empty[] = "";
	char buffer[10];
	unsigned char line[512];
	int i;
	symbol *sym;

	if(output == NULL) {
		fprintf(stderr,"could not open file: %s\n",outputfile);
		return -1;
	}

	for(i = 0; i < 2; i++) {
		FILE *input = fopen(inputfile,"r");
		if(input == NULL) {
			fprintf(stderr,"could not open file: %s\n", inputfile);
			return -1;
		}
		int linenum = 0;
		int address = 0;
		while(fgets(line,512,input)!=NULL) {
			linenum++;
			linedata data;
			data.label=NULL;
			data.mnemonic=NULL;
			data.arg=empty;
	
			split_line(line,&data);
	
			mnemonic *mne = get_mnemonic_data(data.mnemonic);
			if(mne == NULL) {
				fprintf(stderr, "%s:%d : no such mnemonic: %s\n", 
					inputfile, 
					linenum, 
					data.mnemonic
				);
				return -1;
			}
			
			if(i == 0) {
				if(data.label != NULL) {
					if(symbol_search(symbol_table, data.label) != NULL) {
						fprintf(
							stderr, 
							"line %d symbol '%s' defined more than once\n",
							linenum,
							data.label
						);
						return -1;
					}

					sym=symbol_new(data.label, address);
					sym->next = symbol_table;
					symbol_table = sym;
				}
			}
			
			if(i == 1){
				printf(
					"%s:%d %s %s\n",
					inputfile,
					linenum,
					data.mnemonic,
					data.arg
				);
				if(mnemonic_getbytes(mne, buffer, data.arg, address, symbol_table) < 0) {
					fprintf(stderr,"ERROR assembling %s:%d\n",inputfile,linenum);
					return -1;
				}
				fwrite(buffer, mne->opsize, 1, output);
			}
			
			address += mne->opsize;
		}
		fclose(input);
	}
	
	fclose(output);
	return 0;
}

int main(int argc, char *argv[])
{
	char *inputfile = argv[1];
	char outputfile[256];
	int i;
	
	strncpy(outputfile,inputfile,256);
	
	for(i = 0; outputfile[i]!='\0' && outputfile[i] != '.'; i++);

	outputfile[i] = '.';
	outputfile[i + 1] = 'b';
	outputfile[i + 2] = 'i';
	outputfile[i + 3] = 'n';

	return process(inputfile, outputfile);
}
