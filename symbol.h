#ifndef _SYMBOL_H
#define _SYMBOL_H

#define SYMBOL_NAME_LEN 30

typedef struct _symbol
{
	char name[SYMBOL_NAME_LEN]; //more than plenty imo
	unsigned int length;
	unsigned int address;
	struct _symbol *next;
} symbol;

symbol *symbol_new(const char* name, unsigned int address);
void symbol_destroy(symbol *sym);
symbol *symbol_search(symbol *first, const char *name, int lazy);
void symbol_table_insert(symbol **symbol_table, symbol *sym);
#endif
