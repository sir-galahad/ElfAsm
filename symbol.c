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

void symbol_table_insert(symbol **symbol_table, symbol *sym)
{	
	symbol *iter;
	if(*symbol_table==NULL){
		*symbol_table=sym;
		return;
	}

	if((*symbol_table)->length < sym->length) {
		sym->next=*symbol_table;
		*symbol_table=sym;
		return;
	}
	for(iter=*symbol_table; iter!=NULL; iter = iter->next) {
		if(iter->next == NULL || iter->next->length < sym->length) {
			sym->next=iter->next;
			iter->next=sym;
			return;
		}
	}
}

symbol *symbol_search(symbol *first, const char *name, int lazy)
{
	symbol *sym;
	int name_len = strlen(name);
	int cmp_len;
	if(name_len >= SYMBOL_NAME_LEN) {
		fprintf(stderr,"searching symbol name too long: %s\n",name);
		return NULL;
	}
	
	for(sym=first; sym!=NULL; sym=sym->next) {
		if(!lazy) {
			cmp_len = name_len > SYMBOL_NAME_LEN ? name_len : SYMBOL_NAME_LEN;
		} else {
			cmp_len = sym->length;
		}

		if(strncmp(sym->name, name, cmp_len) == 0){
			return sym;
		}
	}
	return NULL;
}

void *symbol_dump(symbol *symbol_table)
{
	symbol *sym;
	for(sym=symbol_table; sym!=NULL; sym=sym->next) {
		printf("%s=%X\n",sym->name,sym->address);
	}
}

