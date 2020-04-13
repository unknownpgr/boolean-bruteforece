#include "pch.h"
#include <iostream>

/*

	Boolean bruteforce

	This program finds optimal 4-variable boolean equation that generates given truth table.
	For example, when given truth table is:

				 < Truth table>

	|   a   |   b   |   c   |   d   |  out  |
	├───────┼───────┼───────┼───────┼───────┤
	|   0   |   0   |   0   |   0   |   0   |
	|   0   |   0   |   0   |   1   |   1   |
	|   0   |   0   |   1   |   0   |   0   |
	|   0   |   0   |   1   |   1   |   1   |
	|   0   |   1   |   0   |   0   |   1   |
	|   0   |   1   |   0   |   1   |   1   |
	|   0   |   1   |   1   |   0   |   0   |
	|   0   |   1   |   1   |   1   |   0   |
	|   1   |   0   |   0   |   0   |   0   |
	|   1   |   0   |   0   |   1   |   0   |
	|   1   |   0   |   1   |   0   |   0   |
	|   1   |   0   |   1   |   1   |   1   |
	|   1   |   1   |   0   |   0   |   1   |
	|   1   |   1   |   0   |   1   |   1   |
	|   1   |   1   |   1   |   0   |   0   |
	|   1   |   1   |   1   |   1   |   1   |

	The input of program is 0b1011100000111010 which is [out].
	Then, this program return (((b^c)|(a&d))^((c^d)&~b)) that satisfy given truth table with minimum operation.
*/

//#define DEBUG // print log. program will be dramatically slower.
//#define NAND	// Use only NAND operation.

/*
EXP is an expression of boolean operation.
op : operation. for example, |, &, ^, !, n(NAND).
left : left side operand
right : right side operand

for NOT operation, op=~ and left=operand, right=NULL=zero.
zero is special expression only for not.

if EXP is a variable, then op is var name(a,b,c,d) and left=right.

Depth means how many operations used for given expression.
** It is different from depth of tree. **
for example, if (a|b)^(c&d), depth of [|]=1,&=[1],^=[3].

EXP is stored in two way.
linked list for iteration and lookup table for quick existency check.
next is a indicator for linked list structre.
*/
typedef struct EXP {
	unsigned short value = 0; // 16bit.
	EXP* left = 0;
	EXP* right = 0;
	EXP* next = 0;
	char op = 0;
	char depth = 0;
};

// Print given expression. for example, (((b^c)|(a&d))^((c^d)&~b))
void printExp(EXP* e) {
	// Initial value
	if (!(e->left||e->right)) {
		printf("%c", e->op);
	}
	else {
		if (e->op == '~') {
			printf("~");
			printExp(e->left);
		}
		else {
			printf("(");
			printExp(e->left);
			printf("%c", e->op);
			printExp(e->right);
			printf(")");
		}
	}
}

// Print short in binary form.
void printShort(unsigned short b) {
	for (int i = 15;i >= 0;i--) {
		printf("%d", !!(b & 0x01 << i));
	}
}

// Print expression with its truth table. for example, 1011100000111010 : (((b^c)|(a&d))^((c^d)&~b))
void print(EXP* e) {
	if (e != 0) {
		printShort(e->value);
		printf(" : depth = %d :\t",e->depth);
		printExp(e);
		printf("\n");
	}
	else {
		printf("Given expression is NULL.\n");
	}
}


EXP* lookupTable[65536] = { 0 };	// Lookup table for quick existency check. 
EXP* header;						// Header node for linked list
EXP* zero;							// Zero expression for not righthand-side operand

// Append new expression that has given value.
EXP* addExp(unsigned short value) {
	EXP* exp = (EXP*)malloc(sizeof(EXP));

	// Initialize expression
	exp->value = value;
	exp->next = header;
	exp->left = 0;
	exp->right = 0;
	exp->depth = 0;

	// Append to linkedlist
	header = exp;

	// Append to lookup table
	lookupTable[value] = exp;

	return exp;
}

// Initialize basic expression. which are:
//a = 1111111100000000
//b = 1111000011110000
//c = 1100110011001100
//d = 1010101010101010
void initExp(EXP** es) {
	for (char j = 0;j < 4;j++) {
		unsigned short value = 0;
		char m = 0x01 << (3 - j);
		for (char i = 0;i < 16;i++) if (i&m)value |= (0x01 << i);
		es[j] = addExp(value);
		es[j]->op = 'a'+j;
	}
}

// To consider operation duplication, recursivly check expresion.
// For example, depth of (a|b)&(c&(a|b)) is 3, because operation (a|b) is duplicated.
void calcDepth(char* lookupTable, char* depth, EXP* exp) {
	if (!exp)return;
	if (exp == zero)return;							// Depth of zero is definitly zero.
	if ('a' <= exp->op && exp->op <= 'd')return;	// Depth of variable is zero.
	if (!lookupTable[exp->value]) {
		lookupTable[exp->value] = 1;
		*depth += 1;
	}
	calcDepth(lookupTable, depth, exp->left);
	calcDepth(lookupTable, depth, exp->right);
}

// If there is no given expression or given expression is more efficient than existing one, add given expression.
// therefore using this function during a iteration would not pollute the iteration.
void checkAndAdd(unsigned short value, EXP* left, EXP* right, char op) {
	EXP* exp = lookupTable[value];

	char test[65535] = { 0 };
	char newDepth = 0;

	// If given expression does not exsits
	if (!exp) {
		exp = addExp(value);
		exp->left = left;
		exp->right = right;
		exp->op = op;
		calcDepth(test, &newDepth, exp);
		exp->depth = newDepth;
#ifdef DEBUG
		printf("Assign ");
		print(exp);
#endif DEBUG // DEBUG
	}
	else {
		EXP temp;
		temp.value = value;
		temp.left = left;
		temp.right = right;
		temp.op = op;
		calcDepth(test, &newDepth, &temp);
		temp.depth = newDepth;

		if (newDepth < exp->depth) {
#ifdef DEBUG
			print(exp);
			printf("=>\n");
			print(&temp);
			printf("\n");
#endif // DEBUG
			exp->left = left;
			exp->right = right;
			exp->op = op;
			exp->depth = newDepth;
		}
	}
}

// Generate new expression by combining existing expressions with operation.
void combine() {
	EXP* e1 = header;
	EXP* e2 = header;
	while (e1->next) {
#ifndef NAND
		checkAndAdd(~e1->value, e1, zero, '~');						// Not
#endif // !NAND
		e2 = e1;
		while (e2->next) {
#ifndef NAND
			if (e1->value != e2->value) {
				//a ? a = a operations.
				checkAndAdd(e1->value&e2->value, e1, e2, '&');		// And
				checkAndAdd(e1->value | e2->value, e1, e2, '|');	// Or
			}
			checkAndAdd(e1->value^e2->value, e1, e2, '^');			// Xor
#endif // !NAND
			checkAndAdd(~(e1->value&e2->value), e1, e2, 'n');		// Nand
			e2 = e2->next;
		}
		e1 = e1->next;
	}
}

int main()
{
	// Initialize header and zero
	EXP h,z;
	header = &h;
	zero = &z;
	header->next = 0;
	zero->depth = 0;

	// Input variable
	EXP* es[4] = { 0 };
	initExp(es);
	unsigned short a = es[0]->value;
	unsigned short b = es[1]->value;
	unsigned short c = es[2]->value;
	unsigned short d = es[3]->value;

	// Truth table that I want to find the expression.
	// This program finds shortest boolean expression that generates this truth table.
	unsigned short e = 0b1011100000111010;

	for (int i = 0;i < 4;i++) {
		printf("%c = ", es[i]->op);
		printShort(es[i]->value);
		printf("\n");
	}
	
	EXP* ans;
	while (!(ans = lookupTable[e])) {
		printf("Combine\n");
		combine();
	}
	print(ans);

	return 0;
}