#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "symbol.h"

//this is my code to evaluate static expressions,i started with a plan of 
//doing recursive descent, but found trying to parse the string and evaluate
//it at the same time was breaking my brain. So here we are. This code works
//by first converting the text expression into a linked list of operators
//and values. The only recursion here is done when parentheses are 
//encountered. In that case the parenthetical expression is evaluated 
//recursively and the result added as a value to the current linked list.
//Once the list is constructed the list is iterated over left to right once 
//for each order of precendence. When an operation is found in that order the 
//result is placed in the element on the left of the operator. Other elements
//used in the operation are removed from the list and freed. Thus after all
//orders of precedence have been tested. there will be only one element in
//the head of the list with the result of the expression.

enum {none, number, operation};
enum {nop, multiply, divide, add, subtract, nul};

typedef struct _expression_element
{
	int isoperator;
	int value;
	struct _expression_element *next;
} expel;

expel *expel_new(int isoperator, int value)
{
	expel *el;

	el = malloc(sizeof(expel));
	if(el == NULL) return NULL;
	el->isoperator = isoperator;
	el->value = value;
	el->next = NULL;
}

int get_operator(char c)
{
	if(c == '*') return multiply;
	if(c == '/') return divide;
	if(c == '+') return add;
	if(c == '-') return subtract;

	return nop;
}



int evaluate(const char *exp, symbol *symbol_table) 
{
	expel *head = NULL;
	expel *tail = NULL;
	expel *el, *tmp;
	int i,num, valuechanged;
	char buffer[200];
	int bufpos = 0;
	int parencount;
	int operator;
	char *endptr;
	symbol *sym;
	for(i = 0; exp[i] != '\0'; i++) {
		if( exp[i] == '\t' || exp[i]  == ' ' || exp[i] == '\n') continue;
		if( exp[i] == ')' ) {
			fprintf(stderr, "ERROR unmatched closing parenthesis\n");
			exit(-1);
		}
		
		//parse parenthetical
		if( exp[i] == '(' ) {
			i++;
			parencount = 1;
			for(; exp[i] != 0; i++) {
				if( exp[i] == '(') parencount++;
				if( exp[i] == ')') parencount--;
				if( parencount == 0) {
					printf("%s\n",buffer);
					num = evaluate(buffer,symbol_table);
					break;
				}
				buffer[bufpos++] = exp[i];
				buffer[bufpos] = 0;
			}
			bufpos = 0;
			if(parencount != 0) {
				fprintf(stderr, "ERROR unmatched parenthesis\n");
				exit(-1);
			}
			el=expel_new(0,num);
			if(head == NULL) head=el;
			if(tail == NULL) tail=el;
			else {
				tail->next=el;
				tail=el;
			}
			continue;
		} 

		if((operator = get_operator(exp[i])) != none) {
			el=expel_new(1,operator);
			if(head == NULL) head=el;
			if(tail == NULL) tail=el;
			else {
				tail->next=el;
				tail=el;
			}
			continue;
		}

		if(exp[i] == '^' || exp[i] == '_') {
			sym = symbol_search(symbol_table, &exp[i+1], 1);
			if(sym == NULL) {
				fprintf(stderr, "ERROR: byte specifier used without symbol\n");
				exit(1);
			} else {
				if(exp[i] == '^') num = (sym->address >> 8);
				if(exp[i] == '_') num = (sym->address & 0xff);
				el=expel_new(0,num);
				if(head == NULL) head=el;
				if(tail == NULL) tail=el;
				else {
					tail->next=el;
					tail=el;
				}
				i += sym->length;
			}
			continue;
		}
		
		if( isalpha(exp[i] )) {
			sym = symbol_search(symbol_table, &exp[i], 1);
			if(sym == NULL) {
				fprintf(stderr, "ERROR: alphabeta characters not a symbol\n");
				exit(1);
			} else {
				num = sym->address;
				el=expel_new(0,num);
				if(head == NULL) head=el;
				if(tail == NULL) tail=el;
				else {
					tail->next=el;
					tail=el;
				}
				i += sym->length - 1;
			}
			continue;
		}


			
		
		//if not whitespace paren or operator that leaves only values
		
		num = strtol(&exp[i], &endptr,0);
		if(endptr != &exp[i]){
			el=expel_new(0,num);
			if(head == NULL) head=el;
			if(tail == NULL) tail=el;
			else {
				tail->next=el;
				tail=el;
			}
			//do something with num here
			i = endptr -1 - exp;
			if(*endptr == 0) break;
			continue;
		}
	}

	for(i=0; i<2; i++) {
		el=head;
		while(el!=NULL){
			if(el->next!=NULL && el->next->isoperator){
				valuechanged = 0;
				switch(i) {
					case 0:
						if(el->next->value == multiply){
							el->value *= el->next->next->value;
							valuechanged = 1;
						}
						if(el->next->value == divide){
							el->value /= el->next->next->value;
							valuechanged = 1;
						}
						if(valuechanged) {
							tmp=el->next;
							el->next = el->next->next->next;
							free(tmp->next);
							free(tmp);
							continue;
						}
						break;
					case 1:
						if(el->next->value == add){
							el->value += el->next->next->value;
							valuechanged = 1;
						}
						if(el->next->value == subtract){
							el->value -= el->next->next->value;
							valuechanged = 1;
						}
						if(valuechanged) {
							tmp=el->next;
							el->next = el->next->next->next;
							free(tmp->next);
							free(tmp);
							continue;
						}
						break;
				}
			}
			el=el->next;
		}
	}
	i = head->value;
	free(head);
	return i;
}


