// stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdbool.h>

// Максимальный размер стека
#define STACK_CAPACITY 8

// Структура для хранения пары индексов двумерного массива
typedef struct 
{
    int row;
    int col;
} IndexPair;

// Структура данных для представления stack
typedef struct stack
{
    int top;                          // вершина(размер) stack
    IndexPair items[STACK_CAPACITY];  // элементы stack
} Stack;

// Прототипы функций, работающих со стеком
void initStack(Stack* pt);
int size(const Stack* pt);
void push(Stack* pt, IndexPair pos);
IndexPair pop(Stack* pt);
void clear(Stack* pt);
bool include(const Stack* pt, IndexPair pos);

#endif // STACK_H
