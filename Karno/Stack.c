// stack.c

#include "stack.h"

// Вспомогательная функция для инициализации stack
struct stack* newStack(int capacity)
{
    struct stack* pt = (struct stack*)malloc(sizeof(struct stack));

    pt->maxsize = capacity;
    pt->top = -1;
    pt->items = (IndexPair*)malloc(sizeof(IndexPair) * capacity);

    return pt;
}

// Вспомогательная функция для возврата размера stack
int size(struct stack* pt) {
    return pt->top + 1;
}

// Вспомогательная функция для проверки, пуст stack или нет
int isEmpty(struct stack* pt) {
    return pt->top == -1; // или return size(pt) == 0;
}

// Вспомогательная функция для проверки, заполнен ли stack или нет
int isFull(struct stack* pt) {
    return pt->top == pt->maxsize - 1; // or return size(pt) == pt->maxsize;
}

// Вспомогательная функция для добавления элемента 'x' в stack
void push(struct stack* pt, IndexPair pos)
{
    // проверяем, не заполнен ли уже stack. Тогда вставка элемента
    // приведёт к переполнению stack
    if (isFull(pt))
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Inserting (%d, %d)\n", pos.row, pos.col);

    // добавляем элемент и увеличиваем индекс вершины
    pt->items[++pt->top] = pos;
}

// Вспомогательная функция для возврата верхнего элемента stack
IndexPair  peek(struct stack* pt)
{
    // проверка на пустой stack
    if (!isEmpty(pt)) {
        return pt->items[pt->top];
    }
    else {
        exit(EXIT_FAILURE);
    }
}

// Вспомогательная функция для извлечения верхнего элемента из stack
IndexPair  pop(struct stack* pt)
{
    // проверка на опустошение stack
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    IndexPair topItem = peek(pt);
    printf("Removing (%d, %d)\n", topItem.row, topItem.col);

    // уменьшаем размер stack на 1 и возвращаем извлеченный элемент
    pt->top--;
    return topItem;
}

void clear(struct stack* pt)
{
    pt->top = -1;
}

// Вспомогательная функция для проверки наличия элемента в stack
bool include(struct stack* pt, IndexPair pos)
{
    for (int i = 0; i <= pt->top; i++) {
        if (pt->items[i].row == pos.row && pt->items[i].col == pos.col) {
            return true;
        }
    }
    return false;
}
