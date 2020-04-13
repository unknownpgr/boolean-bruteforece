# Boolean-bruteforece
This is a C++ program that finds an boolean expression that satisfies given truth table.
## How it works?
1. The main idea is to represent truth table of a 4-value boolean expression as 16bit integer(unsigned short). below is 2-value example.
> Let a = 1100, b = 1010 then
>- a&b = 1000
>- a|b = 1110
>- a^b = 0110
2. Define a expression structrue that represents a expression. It has three essntial property.
- Truth table(16-bit integer)
- Left operand(Pointer of expression structure)
- Right operand(NULL for NOT operation)
- Operatior(char. e.g. ~, &, |, ^...)
3. Define 2^(2^4)=65536 length array of pointers of expression structures. It is lookup table. for example, array[0b0000111100001111] = Pointer of expression structure whose truth table is 0b0000111100001111.
4. Fill empty elements of lookup table by combining existing expressions with operation.
5. It expression dupicates, use shorter one. for example, use (a&b) rather than (a&b)|(a^a)
6. Repeat 4~5 until expression for given truth table is found.
## Sourc code
- See [Boolean Bruteforce.cpp](./Boolean%20Bruteforce/Boolean%20Bruteforce.cpp) for more details.