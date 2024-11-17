// stack.c

#include "stack.h"

// ��������������� ������� ��� ������������� stack
struct stack* newStack(int capacity)
{
    struct stack* pt = (struct stack*)malloc(sizeof(struct stack));

    pt->maxsize = capacity;
    pt->top = -1;
    pt->items = (IndexPair*)malloc(sizeof(IndexPair) * capacity);

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
void push(struct stack* pt, IndexPair pos)
{
    // ���������, �� �������� �� ��� stack. ����� ������� ��������
    // ������� � ������������ stack
    if (isFull(pt))
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Inserting (%d, %d)\n", pos.row, pos.col);

    // ��������� ������� � ����������� ������ �������
    pt->items[++pt->top] = pos;
}

// ��������������� ������� ��� �������� �������� �������� stack
IndexPair  peek(struct stack* pt)
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
IndexPair  pop(struct stack* pt)
{
    // �������� �� ����������� stack
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    IndexPair topItem = peek(pt);
    printf("Removing (%d, %d)\n", topItem.row, topItem.col);

    // ��������� ������ stack �� 1 � ���������� ����������� �������
    pt->top--;
    return topItem;
}

void clear(struct stack* pt)
{
    pt->top = -1;
}

// ��������������� ������� ��� �������� ������� �������� � stack
bool include(struct stack* pt, IndexPair pos)
{
    for (int i = 0; i <= pt->top; i++) {
        if (pt->items[i].row == pos.row && pt->items[i].col == pos.col) {
            return true;
        }
    }
    return false;
}
