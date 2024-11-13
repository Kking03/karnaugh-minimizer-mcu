// Karno.c

#include "Stack.h"


#include <locale.h>

// функция тестирования стека (можно удалить)
void TestStack();

unsigned char* ones;      // глобальное объявление массива единиц
unsigned char  size_ones; // глобальное объявление размера массива единиц
unsigned char  A, B;      // размерность карты Карно

// Структура для хранения двух массивов для карты Карно
typedef struct {
    unsigned char** kmap;     // Карта Карно
    bool** kmap_bool;         // Массив false
} KarnaughMaps;

// Структура для узла списка
typedef struct ListNode {
    int stackSize;           // Размер стека на момент создания узла
    IndexPair* stackItems;   // Копия элементов стека
    struct ListNode* next;   // Указатель на следующий элемент списка
} ListNode;

// функция для создания массива единиц булевой функции
unsigned char* parse_numbers(const char* input, unsigned char* size) {
    // Подсчитываем количество чисел в строке
    *size = 0;
    for (unsigned char i = 0; input[i] != '\0'; i++) {
        if (input[i] == ' ') (*size)++;
    }
    (*size)++; // Количество чисел = количество пробелов + 1

    // Создаем массив для чисел
    unsigned char* numbers = (unsigned char*)malloc(*size * sizeof(unsigned char));
    if (!numbers) {
        printf("Ошибка выделения памяти\n");
        return NULL;
    }

    // Разбираем строку и заполняем массив чисел
    unsigned char index = 0;
    char* endptr;
    for (const char* token = input; *token != '\0'; token = endptr) {
        numbers[index++] = strtol(token, &endptr, 10);
        if (*endptr == ' ') endptr++;
    }

    return numbers;
}

// Функция для создания двумерного массива
KarnaughMaps create_karnaugh_map(int N) {
    // Выделяем память для двумерного массива
    unsigned char** map = (unsigned char**)malloc(A * sizeof(unsigned char*));
    for (int i = 0; i < A; i++) {
        map[i] = (unsigned char*)malloc(B * sizeof(unsigned char));
    }

    // Выделяем память для массива false
    bool** map_false = (bool**)malloc(A * sizeof(bool*));
    for (int i = 0; i < A; i++) {
        map_false[i] = (bool*)malloc(B * sizeof(bool));
    }

    // Заполняем массивы нулями и false
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            map[i][j] = 0;
            map_false[i][j] = false;
        }
    }

    // Устанавливаем единицы по индексам из массива ones
    for (size_t i = 0; i < size_ones; i++) {
        int index = ones[i];
        int row = index / B;
        int col = index % B;
        if (row < A && col < B) {
            map[row][col] = 1;
        }
    }

    KarnaughMaps maps = { map, map_false };
    return maps;
}

// создание двумерного массива с кодом Грея
char** generate_gray_code(int N, int* rows, int* cols) {
    // Определяем размер матрицы для данного N
    if (N == 2) {
        *rows = 2;
        *cols = 2;
    }
    else if (N == 3) {
        *rows = 2;
        *cols = 4;
    }
    else if (N == 4) {
        *rows = 4;
        *cols = 4;
    }
    else {
        printf("Неподдерживаемое значение N\n");
        return NULL;
    }

    // Генерация последовательности кода Грея для N бит
    int total_codes = 1 << N; // 2^N возможных значений
    char** gray_code = (char**)malloc(total_codes * sizeof(char*));

    for (int i = 0; i < total_codes; i++) {
        gray_code[i] = (char*)malloc((N + 1) * sizeof(char)); // +1 для завершающего '\0'
        int gray_value = i ^ (i >> 1); // генерируем значение в коде Грея
        for (int j = N - 1; j >= 0; j--) {
            gray_code[i][N - 1 - j] = (gray_value & (1 << j)) ? '1' : '0';
        }
        gray_code[i][N] = '\0'; // завершаем строку
    }

    // Формируем матрицу из последовательностей
    char** gray_matrix = (char**)malloc(*rows * sizeof(char*));
    for (int i = 0; i < *rows; i++) {
        gray_matrix[i] = (char*)malloc((*cols) * (N + 1) * sizeof(char)); // выделяем память для каждой строки
        for (int j = 0; j < *cols; j++) {
            // Копируем символы вручную, вместо strcpy
            for (int k = 0; k < N + 1; k++) {
                gray_matrix[i][j * (N + 1) + k] = gray_code[i * (*cols) + j][k];
            }
        }
    }

    // Освобождение памяти для временного массива gray_code
    for (int i = 0; i < total_codes; i++) {
        free(gray_code[i]);
    }
    free(gray_code);

    return gray_matrix;
}

// Функция для создания нового элемента списка
ListNode* createListNode(struct stack* pt) {
    if (isEmpty(pt)) {
        return NULL;  // Если стек пустой, ничего не создаём
    }

    // Создаем новый элемент списка
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (!newNode) {
        printf("Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем размер стека
    newNode->stackSize = size(pt);

    // Выделяем память под массив с элементами стека
    newNode->stackItems = (IndexPair*)malloc(newNode->stackSize * sizeof(IndexPair));
    if (!newNode->stackItems) {
        printf("Ошибка выделения памяти\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    // Копируем элементы из стека в массив stackItems
    for (int i = 0; i < newNode->stackSize; i++) {
        newNode->stackItems[i] = pt->items[i];
    }

    newNode->next = NULL;  // Изначально следующий элемент отсутствует
    return newNode;
}

// Функция для добавления нового узла в начало списка
void addNodeToList(ListNode** head, struct stack* pt) {
    ListNode* newNode = createListNode(pt);
    if (newNode) {
        newNode->next = *head;
        *head = newNode;
    }
}

//---------------------------------------- ОТЛАДОЧНЫЕ ФУНКЦИИ ----------------------------------------
void printOnes(unsigned char* ones, unsigned char size) // вывод ones
{
    printf("Массив из строки: ");
    for (unsigned char i = 0; i < size; i++) {
        printf("%d ", ones[i]);
    }
    printf("\n");
}

// Функция для печати карты Карно (для отладки)
void print_karnaugh_map(unsigned char** map, unsigned char A, int B) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
}

// Функция для печати логической карты Карно (для отладки)
void print_karnaugh_map_bool(unsigned char** map, unsigned char A, int B) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            printf("%5s ", map[i][j] ? "true" : "false");
        }
        printf("\n");
    }
}

// Функция для печати матрицы кода Грея
void print_gray_matrix(char** gray_matrix, int rows, int cols, int N) {
    printf("Матрица кода Грея для %d переменных:\n", N);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < N; k++) {
                printf("%c", gray_matrix[i][j * (N + 1) + k]);
            }
            printf(" ");
        }
        printf("\n");
    }
}

// Отладочная функция для вывода содержимого списка
void printList(ListNode* head) {
    ListNode* current = head;
    int nodeIndex = 1;
    while (current != NULL) {
        printf("Элемент %d:\n", nodeIndex++);
        printf("  Размер стека: %d\n", current->stackSize);
        printf("  Элементы стека: ");
        for (int i = 0; i < current->stackSize; i++) {
            printf("(%d, %d) ", current->stackItems[i].row, current->stackItems[i].col);
        }
        printf("\n");
        current = current->next;
    }
    if (nodeIndex == 1) {
        printf("Список пуст\n");
    }
}

// функция тестирования списка (можно удалить)
void TestList();


unsigned char code[4] = { 0b00, 0b01, 0b11, 0b10 }; // последовательность значений для карты Карно

int main()
{
    setlocale(LC_ALL, "Rus");

    // вводимая пользователем строка
    const char* str = "1 3 5 7 9";                     
    unsigned char N = 4;                      // количество переменных

    printf("Исходная строка: %s\n", str);       // ОТЛАДОЧНЫЙ ВЫВОД СТРОКИ

    ones = parse_numbers(str, &size_ones);    // получаем массив чисел и его размер
    printOnes(ones, size_ones);               // ОТЛАДОЧНЫЙ ВЫВОД

    A = (N / 2) * 2;                          // количество строки массива для N переменных
    B = (N - (N / 2)) * 2;                    // количество столбцов массива для N переменных

    KarnaughMaps maps = create_karnaugh_map(N);

    if (maps.kmap != NULL) {
        printf("Карта Карно (в виде бинарной матрицы):\n");
        print_karnaugh_map(maps.kmap, A, B);
    }    
    
    if (maps.kmap_bool != NULL) {
        printf("Карта Карно (в виде бинарной логической матрицы):\n");
        print_karnaugh_map_bool(maps.kmap, A, B);
    }

    int rows, cols;

    // Генерация и вывод матрицы кода Грея
    char** gray_matrix = generate_gray_code(N, &rows, &cols);
    if (gray_matrix) {
        print_gray_matrix(gray_matrix, rows, cols, N);

        // Освобождение памяти
        for (int i = 0; i < rows; i++) {
            free(gray_matrix[i]);
        }
        free(gray_matrix);
    }

    // Освобождение памяти
    for (int i = 0; i < A; i++) {
        free(maps.kmap[i]);
        free(maps.kmap_bool[i]);
    }
    free(maps.kmap);
    free(maps.kmap_bool);
    
    if (ones) free(ones) ; // Освобождаем память

    TestStack();
    TestList();

    return 0;
}

// функция тестирования стека (можно удалить)
void TestStack()
{
    struct stack* pt = newStack(5);

    // Пример добавления координат в стек
    push(pt, (IndexPair) { 1, 2 });
    push(pt, (IndexPair) { 3, 4 });
    push(pt, (IndexPair) { 5, 6 });

    // Проверка, содержит ли стек определенную позицию
    bool res = include(pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "найден" : "не найден");

    // Печать верхнего элемента
    IndexPair top = peek(pt);
    printf("The top element is (%d, %d)\n", top.row, top.col);
    printf("The stack size is %d\n", size(pt));

    // Удаление элементов из стека
    pop(pt);
    top = peek(pt);
    printf("The top element is (%d, %d)\n", top.row, top.col);

    pop(pt);
    pop(pt);

    if (isEmpty(pt)) {
        printf("The stack is empty\n");
    }
    else {
        printf("The stack is not empty\n");
    }

    // Освобождение памяти
    free(pt->items);
    free(pt);

}

void TestList()
{
    struct stack* pt = newStack(5);

    // Добавляем элементы в стек
    push(pt, (IndexPair) { 1, 2 });
    push(pt, (IndexPair) { 3, 4 });
    push(pt, (IndexPair) { 5, 6 });

    // Указатель на начало списка
    ListNode* head = NULL;

    // Создаём элемент списка на основе текущего состояния стека
    addNodeToList(&head, pt);
    pop(pt);
    addNodeToList(&head, pt);

    // Вывод списка
    printList(head);

    // Освобождение памяти (пример)
    ListNode* current = head;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        free(temp->stackItems);
        free(temp);
    }

    // Освобождение памяти стека
    free(pt->items);
    free(pt);

    return 0;
}
