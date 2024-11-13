// stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Структура для хранения пары индексов двумерного массива
typedef struct 
{
    int row;
    int col;
} IndexPair;

// Структура данных для представления stack
struct stack
{
    int maxsize;       // определяем максимальную емкость stack
    int top;           // вершина(размер) stack
    IndexPair* items;  // элементы stack
};

// Прототипы функций, работающих со стеком
struct stack* newStack(int capacity);
int size(struct stack* pt);
int isEmpty(struct stack* pt);
int isFull(struct stack* pt);
void push(struct stack* pt, IndexPair pos);
IndexPair peek(struct stack* pt);
IndexPair pop(struct stack* pt);
bool include(struct stack* pt, IndexPair pos);

#endif // STACK_H
