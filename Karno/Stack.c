// stack.c

#include "stack.h"

// ��������������� ������� ��� ������������� stack
struct stack* newStack(int capacity)
{
    struct stack* pt = (struct stack*)malloc(sizeof(struct stack));

    pt->maxsize = capacity;
    pt->top = -1;
    pt->items = (int*)malloc(sizeof(int) * capacity);

    return pt;
}

// ��������������� ������� ��� �������� ������� stack
int size(struct stack* pt) {
    return pt->top + 1;
}

// ��������������� ������� ��� ��������, ���� stack ��� ���
int isEmpty(struct stack* pt) {
    return pt->top == -1; // ��� return size(pt) == 0;
}

// ��������������� ������� ��� ��������, �������� �� stack ��� ���
int isFull(struct stack* pt) {
    return pt->top == pt->maxsize - 1; // or return size(pt) == pt->maxsize;
}

// ��������������� ������� ��� ���������� �������� 'x' � stack
void push(struct stack* pt, int x)
{
    // ���������, �� �������� �� ��� stack. ����� ������� ��������
    // ������� � ������������ stack
    if (isFull(pt))
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Inserting %d\n", x);

    // ��������� ������� � ����������� ������ �������
    pt->items[++pt->top] = x;
}

// ��������������� ������� ��� �������� �������� �������� stack
int peek(struct stack* pt)
{
    // �������� �� ������ stack
    if (!isEmpty(pt)) {
        return pt->items[pt->top];
    }
    else {
        exit(EXIT_FAILURE);
    }
}

// ��������������� ������� ��� ���������� �������� �������� �� stack
int pop(struct stack* pt)
{
    // �������� �� ����������� stack
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Removing %d\n", peek(pt));

    // ��������� ������ stack �� 1 � (�������������) ���������� ����������� �������
    return pt->items[pt->top--];
}
