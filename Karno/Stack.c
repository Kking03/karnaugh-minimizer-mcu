#include "stack.h"



// ��������������� ������� ��� ������������� stack
void initStack(Stack* pt) {
    pt->top = -1; // ��������� �� ������ ����
}

// ��������������� ������� ��� �������� ������� stack
int size(const Stack* pt) {
    return pt->top + 1;
}

// ��������������� ������� ��� ���������� �������� 'pos' � stack
void push(Stack* pt, IndexPair pos)
{
    printf("Inserting (%d, %d)\n", pos.row, pos.col);  // ���������� �����

    // ��������� ������� � ����������� ������ �������
    pt->items[++pt->top] = pos;
}

// ��������������� ������� ��� ���������� �������� �������� �� stack
IndexPair pop(Stack* pt)
{
    printf("Removing (%d, %d)\n", pt->items[pt->top].row, pt->items[pt->top].col);  // ���������� �����

    // ��������� ������ stack �� 1 � ���������� ����������� �������
    return pt->items[pt->top--];
}

void clear(Stack* pt)
{
    pt->top = -1;
}

// ��������������� ������� ��� �������� ������� �������� � stack
_Bool include(const Stack* pt, IndexPair pos)
{
    for (int i = 0; i <= pt->top; i++) {
        if (pt->items[i].row == pos.row && pt->items[i].col == pos.col) {
            return true;
        }
    }
    return false;
}
