// stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ��������� ��� �������� ���� �������� ���������� �������
typedef struct 
{
    int row;
    int col;
} IndexPair;

// ��������� ������ ��� ������������� stack
struct stack
{
    int maxsize;       // ���������� ������������ ������� stack
    int top;           // �������(������) stack
    IndexPair* items;  // �������� stack
};

// ��������� �������, ���������� �� ������
struct stack* newStack(int capacity);
int size(struct stack* pt);
int isEmpty(struct stack* pt);
int isFull(struct stack* pt);
void push(struct stack* pt, IndexPair pos);
IndexPair peek(struct stack* pt);
IndexPair pop(struct stack* pt);
bool include(struct stack* pt, IndexPair pos);

#endif // STACK_H
