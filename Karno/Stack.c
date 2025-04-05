#include "stack.h"

// ��� ������ 
// char buffer[5]; // ����� ��� ������ (�������� 4 ������� + '\0' ��� uint8_t)



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
	// ���������� �����
	// USARTTransmitString("Inserting: ");

	// itoa(pos.row, buffer, 10);
	// USARTTransmitString(buffer); // ���������� ������ �� UART

	// itoa(pos.col, buffer, 10);
	// USARTTransmitStringLn(buffer); // ���������� ������ �� UART
	
    // printf("Inserting (%d, %d)\n", pos.row, pos.col);

    // ��������� ������� � ����������� ������ �������
    pt->items[++pt->top] = pos;
}

// ��������������� ������� ��� ��������� �������� �������� �� stack
IndexPair peek(Stack* pt)
{
    //printf("Removing (%d, %d)\n", pt->items[pt->top].row, pt->items[pt->top].col);  // ���������� �����

    // ��������� ������ stack �� 1 � ���������� ����������� �������
    return pt->items[pt->top];
}

// ��������������� ������� ��� ���������� �������� �������� �� stack
IndexPair pop(Stack* pt)
{
	// ���������� �����
	// USARTTransmitString("Removing: ");

	// itoa(pt->items[pt->top].row, buffer, 10);
	// USARTTransmitString(buffer); // ���������� ������ �� UART

	// itoa(pt->items[pt->top].col, buffer, 10);
	// USARTTransmitStringLn(buffer); // ���������� ������ �� UART

    // printf("Removing (%d, %d)\n", pt->items[pt->top].row, pt->items[pt->top].col);  // ���������� �����

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
