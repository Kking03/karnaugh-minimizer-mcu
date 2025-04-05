#include "stack.h"

// ДЛЯ ВЫВОДА 
// char buffer[5]; // Буфер для строки (максимум 4 символа + '\0' для uint8_t)



// вспомогательная функция для инициализации stack
void initStack(Stack* pt) {
    pt->top = -1; // указывает на пустой стек
}

// вспомогательная функция для возврата размера stack
int size(const Stack* pt) {
    return pt->top + 1;
}

// вспомогательная функция для добавления элемента 'pos' в stack
void push(Stack* pt, IndexPair pos)
{
	// ОТЛАДОЧНЫЙ ВЫВОД
	// USARTTransmitString("Inserting: ");

	// itoa(pos.row, buffer, 10);
	// USARTTransmitString(buffer); // Отправляем строку по UART

	// itoa(pos.col, buffer, 10);
	// USARTTransmitStringLn(buffer); // Отправляем строку по UART
	
    // printf("Inserting (%d, %d)\n", pos.row, pos.col);

    // добавляем элемент и увеличиваем индекс вершины
    pt->items[++pt->top] = pos;
}

// вспомогательная функция для получения верхнего элемента из stack
IndexPair peek(Stack* pt)
{
    //printf("Removing (%d, %d)\n", pt->items[pt->top].row, pt->items[pt->top].col);  // ОТЛАДОЧНЫЙ ВЫВОД

    // уменьшаем размер stack на 1 и возвращаем извлеченный элемент
    return pt->items[pt->top];
}

// вспомогательная функция для извлечения верхнего элемента из stack
IndexPair pop(Stack* pt)
{
	// ОТЛАДОЧНЫЙ ВЫВОД
	// USARTTransmitString("Removing: ");

	// itoa(pt->items[pt->top].row, buffer, 10);
	// USARTTransmitString(buffer); // Отправляем строку по UART

	// itoa(pt->items[pt->top].col, buffer, 10);
	// USARTTransmitStringLn(buffer); // Отправляем строку по UART

    // printf("Removing (%d, %d)\n", pt->items[pt->top].row, pt->items[pt->top].col);  // ОТЛАДОЧНЫЙ ВЫВОД

    // уменьшаем размер stack на 1 и возвращаем извлеченный элемент
    return pt->items[pt->top--];
}

void clear(Stack* pt)
{
    pt->top = -1;
}

// Вспомогательная функция для проверки наличия элемента в stack
_Bool include(const Stack* pt, IndexPair pos)
{
    for (int i = 0; i <= pt->top; i++) {
        if (pt->items[i].row == pos.row && pt->items[i].col == pos.col) {
            return true;
        }
    }
    return false;
}
