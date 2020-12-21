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
enum {nop, high_byte, low_byte, not, multiply, divide, modulo, add, subtract, and, or, xor, shift_left, shift_right, nul};

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

int get_unary_operator(char c)
{
	if(c == '^') return high_byte;
	if(c == '_') return low_byte;
	if(c == '~') return not;
	return nop;
}

int get_operator(char c)
{
	if(c == '*') return multiply;
	if(c == '/') return divide;
	if(c == '+') return add;
	if(c == '-') return subtract;
	if(c == '%') return modulo;
	if(c == '&') return and;
	if(c == '|') return or;
	if(c == '^') return xor;
	if(c == '<') return shift_left;  //i'm not implementing greater than or less than
	if(c == '>') return shift_right;  //so i'm repurposing for shift left and shift right
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
		
		
		if((tail == NULL || tail->isoperator) && ((operator = get_unary_operator(exp[i])) != nop)) {
			el=expel_new(1,operator);
			if(head == NULL) head=el;
			if(tail == NULL) tail=el;
			else {
				tail->next=el;
				tail=el;
			}
			continue;
		}
	
		if((operator = get_operator(exp[i])) != nop) {
			el=expel_new(1,operator);
			if(head == NULL) head=el;
			if(tail == NULL) tail=el;
			else {
				tail->next=el;
				tail=el;
			}
			continue;
		}

		if( isalpha(exp[i] )) {
			sym = symbol_search(symbol_table, &exp[i], 1);
			if(sym == NULL) {
				fprintf(stderr, "ERROR: alphabetical characters not a symbol\n");
				return  -1;
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

	for(i=0; i<5; i++) {
		el=head;
		while(el!=NULL && el->next!=NULL){
			//if(el->next!=NULL && el->next->isoperator){
			valuechanged = 0;
			switch(i) {
				////UNARY OPERATORS NEED WORK
				case 0: //unary operators
					for(;el->next != NULL && el->next->isoperator; el=el->next);

					if(el->next == NULL) break;
					
					if(el->isoperator && el->value == not){
						el->value = ~el->next->value;
						el->isoperator=0;
						valuechanged = 1;
					}
					if(el->isoperator && el->value == high_byte){
						el->value = (el->next->value >> 8);
						el->isoperator=0;
						valuechanged = 1;
					}
					if(el->isoperator && el->value == low_byte){
						el->value = (el->next->value & 0xff);
						el->isoperator=0;
						valuechanged = 1;
					}
					if(valuechanged) {
						tmp=el->next;
						el->next = el->next->next;
						free(tmp);
						el=head;
						continue;
					}
					break;
			
				case 1:
				    //cases 1 and up should have only binary operators
					if(     el->isoperator || 
							el->next == NULL || !el->next->isoperator || 
							el->next->next==NULL || el->next->next->isoperator) {
						break;
					}

					if(el->next != NULL && el->next->value == multiply){
						el->value *= el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == divide){
						el->value /= el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == modulo){
						el->value %= el->next->next->value;
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

				case 2:
				    //cases 1 and up should have only binary operators
					if(     el->isoperator || 
							el->next == NULL || !el->next->isoperator || 
							el->next->next==NULL || el->next->next->isoperator) {
						break;
					}

					if(el->next != NULL && el->next->value == add){
						el->value += el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == subtract){
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

				case 3:
				    //cases 1 and up should have only binary operators
					if(     el->isoperator || 
							el->next == NULL || !el->next->isoperator || 
							el->next->next==NULL || el->next->next->isoperator) {
						break;
					}

					if(el->next != NULL && el->next->value == shift_left){
						el->value <<= el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == shift_right){
						el->value >>= el->next->next->value;
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

				case 4:
				    //cases 1 and up should have only binary operators
					if(     el->isoperator || 
							el->next == NULL || !el->next->isoperator || 
							el->next->next==NULL || el->next->next->isoperator) {
						break;
					}

					if(el->next != NULL && el->next->value == and){
						el->value &= el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == or){
						el->value |= el->next->next->value;
						valuechanged = 1;
					}
					if(el->next != NULL && el->next->value == xor){
						el->value ^= el->next->next->value;
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
		el=el->next;
		}
	}
	i = head->value;
	free(head);
	return i;
}


