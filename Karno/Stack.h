// stack.h

#ifndef STACK_H
#define STACK_H

#define true  1
#define false 0

// #include <stdio.h>
#include "Uart.h" // ���������

// ������������ ������ �����
#define STACK_CAPACITY 8

// ��������� ��� �������� ���� �������� ���������� �������
typedef struct 
{
    int row;
    int col;
} IndexPair;

// ��������� ������ ��� ������������� stack
typedef struct stack
{
    int top;                          // �������(������) stack
    IndexPair items[STACK_CAPACITY];  // �������� stack
} Stack;

// ��������� �������, ���������� �� ������
void initStack(Stack* pt);
int size(const Stack* pt);
void push(Stack* pt, IndexPair pos);
IndexPair peek(Stack* pt);
IndexPair pop(Stack* pt);
void clear(Stack* pt);
_Bool include(const Stack* pt, IndexPair pos);

#endif // STACK_H
