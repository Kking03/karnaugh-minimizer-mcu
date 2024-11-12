// stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Структура данных для представления stack
struct stack
{
    int maxsize;    // определяем максимальную емкость stack
    int top;        // вершина(размер) stack
    int* items;     // элементы stack
};

// Прототипы функций, работающих со стеком
struct stack* newStack(int capacity);
int size(struct stack* pt);
int isEmpty(struct stack* pt);
int isFull(struct stack* pt);
void push(struct stack* pt, int x);
int peek(struct stack* pt);
int pop(struct stack* pt);
bool include(struct stack* pt, int x);

#endif // STACK_H