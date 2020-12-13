#include "symbol.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
symbol *symbol_new(const char *name, unsigned int address)
{
	symbol *sym;
	int name_len = strlen(name);

	if(name_len >= SYMBOL_NAME_LEN) {
		fprintf(stderr,"symbol name too long: %s\n",name);
		return NULL;
	}

	sym = malloc(sizeof(symbol));
	if(sym == NULL) {
		fprintf(stderr, "unable to allocate memory for symbol\n");
		return NULL;
	}

	sym->address = address;
	sym->next = NULL;
	sym->length = name_len;
	strncpy(sym->name, name, name_len);

	return sym;
}

void symbol_destroy(symbol *sym) 
{
	free(sym);
}

symbol *symbol_search(symbol *first, const char *name)
{
	symbol *sym;
	int name_len = strlen(name);

	if(name_len >= SYMBOL_NAME_LEN) {
		fprintf(stderr,"searching symbol name too long: %s\n",name);
		return NULL;
	}
	
	for(sym=first; sym!=NULL; sym=sym->next) {
		if(strncmp(sym->name, name, sym->length) == 0){
			return sym;
		}
	}

	return NULL;
}
