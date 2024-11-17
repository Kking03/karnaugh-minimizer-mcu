// ЗАДАНИЕ
/*
1 Минимизация для f = Const
2 Добавление импликант в список
*/

// Karno.c
#include <locale.h>

#include "Stack.h"

unsigned char* ones;      // глобальное объявление массива единиц
unsigned char  size_ones; // глобальное объявление размера массива единиц
unsigned char  A, B;      // размерность карты Карно
char*** gray_matrix;      // матрица кодов Грея
unsigned char N;          // количество переменных

const char* code_gray[] = { "00", "01", "11", "10" };  // Код Грея для 2 бит (используем как базовый)
// ВРЕМЕННО
const char* code_gray2[2][2] = {
    {"00", "01"},
    {"10", "11"}
}; // Код Грея для 2 переменных

// Структура для хранения двух массивов для карты Карно
typedef struct {
    unsigned char** kmap;     // Карта Карно
    bool** kmap_bool;         // Массив false
} KarnaughMaps;

KarnaughMaps maps;

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
        int target_index = ones[i];

        // Поиск позиции (row, col) для target_index в gray_matrix
        bool found = false;
        for (int row = 0; row < A && !found; row++) {
            for (int col = 0; col < B && !found; col++) {
                // Преобразуем строку кода Грея в число для сравнения с target_index
                int gray_value = (int)strtol(gray_matrix[row][col], NULL, 2);

                if (gray_value == target_index) {
                    map[row][col] = 1;
                    // found = true;
                }
            }
        }
    }

    KarnaughMaps maps = { map, map_false };
    return maps;
}

// Функция для создания двумерной матрицы кода Грея для карты Карно
char*** generate_gray_code_matrix(int N, int* rows, int* cols) {
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

    // Создаём матрицу для хранения комбинаций кода Грея
    char*** gray_matrix = (char***)malloc(*rows * sizeof(char**));
    for (int i = 0; i < *rows; i++) {
        gray_matrix[i] = (char**)malloc(*cols * sizeof(char*));
        for (int j = 0; j < *cols; j++) {
            gray_matrix[i][j] = (char*)malloc((N + 1) * sizeof(char)); // N бит + '\0'

            // Объединяем код Грея для строки и столбца, создавая строку длиной N
            if (N == 2) {
                // ИСПРАВИТЬ ГЕНЕРАЦИЮ
                // Для 2 переменных используем строки и столбцы напрямую из code_gray
                gray_matrix[i][j] = code_gray2[i][j];
            }
            else if (N == 3) {
                // Для 3 переменных используем один бит для строки и два бита для столбца
                gray_matrix[i][j][0] = (i == 0) ? '0' : '1'; // 1 бит для строки
                gray_matrix[i][j][1] = code_gray[j][0];
                gray_matrix[i][j][2] = code_gray[j][1];
                gray_matrix[i][j][3] = '\0';
            }
            else if (N == 4) {
                // Для 4 переменных используем два бита для строки и два бита для столбца
                gray_matrix[i][j][0] = code_gray[i][0];
                gray_matrix[i][j][1] = code_gray[i][1];
                gray_matrix[i][j][2] = code_gray[j][0];
                gray_matrix[i][j][3] = code_gray[j][1];
                gray_matrix[i][j][4] = '\0';
            }
        }
    }

    return gray_matrix;
}

struct stack* pt;

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

// Функция для подсчета количества одинаковых битов на всех строках из стека
int count_common_bits(struct stack* pt) {
    if (isEmpty(pt)) {
        return 0; // Если стек пустой, возвращаем 0
    }

    int common_bits_count = 0;
    int bit_length = strlen(gray_matrix[0][0]); // Длина строки кода Грея (N бит)

    // Проверяем каждый бит (позицию) в строках
    for (int bit = 0; bit < bit_length; bit++) {
        // Получаем бит из первой строки в стеке
        char first_bit = gray_matrix[pt->items[0].row][pt->items[0].col][bit];
        bool all_match = true;

        // Сравниваем бит с аналогичными битами во всех строках из стека
        for (int i = 1; i < size(pt); i++) {
            if (gray_matrix[pt->items[i].row][pt->items[i].col][bit] != first_bit) {
                all_match = false;
                break;
            }
        }

        // Если на данной позиции биты совпадают у всех строк, увеличиваем счетчик
        if (all_match) {
            common_bits_count++;
        }
    }

    return common_bits_count;
}

// СДЕЛАТЬ ПОИСК БОЛЕЕ ЭФФЕКТИВНЫМ (ЛИНЕЙНО)
// Функция для поиска импликант
bool minimize(unsigned char x, unsigned char y)
{
    // добавление элемента в стек
    push(pt, (IndexPair) { x, y });

    int bits;                                     // число одинаковых бит
    bool match = (size(pt) == 1) ? true : false;  // флаг, указывающий, что единица подходит для склейки
    bool break_flag = false;                      // флаг устанавливается для тупиковой единицы
    
    bits = (size(pt) == 1) ? N : count_common_bits(pt);  // проверяем количество совпадаемых бит
    if (bits == 0)
        break_flag = true;         // единица не подходит
    else
        switch (size(pt))
        {
        case 2:
            match = true;
            if (N == 2) break_flag = true;  // для двух переменных
            break;
        case 4:
            if (bits == 1 && N == 3)        // для трёх переменных
                match = true;
            if (bits == 2 && N == 4)        // для четырёх переменных
                match = true;
            break;
        case 8:
            if (bits == 1) 
                match = true;
            break_flag = true;    // тупик, дальше 8 единиц не ищем
            break;
        }

    if (!break_flag) // можно продолжить поиск
    {
        // Смещения для четырех направлений: вправо, вниз, влево, вверх
        int dx[4] = { 0, 1, 0, -1 };  // Смещение по строкам
        int dy[4] = { 1, 0, -1, 0 };  // Смещение по столбцам

        // Проходим по каждому направлению
        for (int k = 0; k < 4; k++) {
            int nx = (x + dx[k] + A) % A;  // Новая координата по строке
            int ny = (y + dy[k] + B) % B;  // Новая координата по столбцу

            if ((maps.kmap[nx][ny] == 1) && (!include(pt, (IndexPair) { nx, ny }))) {
                bool res = minimize(nx, ny);
                if (res)  // найдена наибольшая импликанта
                {
                    match = true;    // единица подходит для импликанты                  
                    break;           // завершаем поиск
                }
            }

        }
    }

    if (match) {
        maps.kmap_bool[x][y] = true; // единица принадлежит импликанте ПРОВЕРИТЬ
        return true; // является импликантой   
    }

    pop(pt);         // удаляем элемент
    return false;    // соседи отсутствуют
}


//---------------------------------------- ОТЛАДОЧНЫЕ ФУНКЦИИ ----------------------------------------
void TestStack();                                                          // функция тестирования стека
void printOnes(unsigned char* ones, unsigned char size);                   // Фукнция для вывода массива единиц
void print_karnaugh_map(unsigned char** map, unsigned char A, int B);      // Функция для печати карты Карно
void print_karnaugh_map_bool(unsigned char** map, unsigned char A, int B); // Функция для печати логической карты Карно
void print_gray_matrix(char*** gray_matrix, int rows, int cols);           // Функция для печати матрицы кода Грея
void printList(ListNode* head);                                            // Отладочная функция для вывода содержимого списка
void TestList();                                                           // функция тестирования списка
void TestBitCountFunc();                                                   // функция тестирования подсчета одинаковых бит

int main()
{
    setlocale(LC_ALL, "Rus");

    // вводимая пользователем строка
    const char* str = "0 1 2 3";                     
    N = 2;                                      // количество переменных

    printf("Исходная строка: %s\n", str);       // ОТЛАДОЧНЫЙ ВЫВОД СТРОКИ

    ones = parse_numbers(str, &size_ones);    // получаем массив чисел и его размер
    printOnes(ones, size_ones);               // ОТЛАДОЧНЫЙ ВЫВОД

    A = (N / 2) * 2;                          // количество строки массива для N переменных
    B = (N - (N / 2)) * 2;                    // количество столбцов массива для N переменных

    int rows, cols;

    // Генерация и вывод матрицы кода Грея
    gray_matrix = generate_gray_code_matrix(N, &rows, &cols);
    if (gray_matrix) {
        print_gray_matrix(gray_matrix, rows, cols);

        // УДАЛИТЬ! ТЕСТИРОВАНИЕ ПОДСЧЁТА БИТ
        // TestBitCountFunc(); // ДЛЯ ФУНКЦИИ ПОИСКА БИТ СТРОКИ ДОЛЖНЫ СУЩЕСТВОВАТЬ
    }

    maps = create_karnaugh_map(N);

    if (maps.kmap != NULL) {
        printf("Карта Карно (в виде бинарной матрицы):\n");
        print_karnaugh_map(maps.kmap, A, B);
    }

    // ОСНОВНОЙ АЛГОРИТМ
    int size = 1;                      // максимальный размер искомой импликанты
    for (int i = 0; i < N-1; i++)
        size *= 2;
    pt = newStack(size);

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if ((maps.kmap[i][j] == 1) && (!maps.kmap_bool[i][j])) {
                printf("Вызвана функция минимизации:\n");
                if (minimize(i, j))
                    clear(pt);                   // очитска стека
                    maps.kmap_bool[i][j] = true; // единица принадлежит импликанте
            }
        }
    }

    // Печать карты Карно с помеченными единицами
    if (maps.kmap_bool != NULL) {
        printf("Карта Карно (в виде бинарной логической матрицы):\n");
        print_karnaugh_map_bool(maps.kmap_bool, A, B);
    }

    // Освобождение памяти
    if (gray_matrix && N != 2) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                free(gray_matrix[i][j]);
            }
            free(gray_matrix[i]);
        }
        free(gray_matrix);
    }

    for (int i = 0; i < A; i++) {
        free(maps.kmap[i]);
        free(maps.kmap_bool[i]);
    }
    free(maps.kmap);
    free(maps.kmap_bool);
    
    if (ones) free(ones) ; // Освобождаем память

    // TestStack();
    // TestList();

    return 0;
}

//------------------------------------------реализация------------------------------------------
// Фукнция для вывода массива единиц
void printOnes(unsigned char* ones, unsigned char size)
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
void print_gray_matrix(char*** gray_matrix, int rows, int cols) {
    printf("Матрица кода Грея:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s ", gray_matrix[i][j]);
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

// функция тестирования стека (можно удалить)
void TestStack()
{
    printf("-----ТЕСТИРОВАНИЕ стека-----\n");

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
    printf("-----ТЕСТИРОВАНИЕ списка-----\n");
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

void TestBitCountFunc()
{
    printf("\n-----ТЕСТИРОВАНИЕ подсчёта бит-----\n");

    struct stack* pt = newStack(4);

    // Добавления координат в стек
    push(pt, (IndexPair) { 0, 0 });
    push(pt, (IndexPair) { 0, 1 });
    push(pt, (IndexPair) { 1, 0 });
    push(pt, (IndexPair) { 1, 1 });

    // Проверка функции подсчёта
    printf("одинаковых бит: %d\n", count_common_bits(pt));

    // Удаление элементов из стека
    pop(pt);
    pop(pt);

    // Освобождение памяти
    free(pt->items);
    free(pt);
}
