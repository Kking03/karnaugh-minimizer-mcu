// Karno.c

#include "Stack.h"


#include <locale.h>

// функция тестирования стека (можно удалить)
void TestStack();

unsigned char* ones;      // глобальное объявление массива единиц
unsigned char  size_ones; // глобальное объявление размера массива единиц
unsigned char A, B;       // размерность карты Карно

// Структура для хранения двух массивов для карты Карно
typedef struct {
    unsigned char** kmap;     // Карта Карно
    bool** kmap_bool;         // Массив false
} KarnaughMaps;

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


unsigned char code[4] = { 0b00, 0b01, 0b11, 0b10 }; // последовательность значений для карты Карно

int main()
{
    setlocale(LC_ALL, "Rus");

    // вводимая пользователем строка
    const char* str = "1 3 5 7 9 15";                     
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



    // Освобождение памяти
    for (int i = 0; i < 4; i++) {
        free(maps.kmap[i]);
        free(maps.kmap_bool[i]);
    }
    free(maps.kmap);
    free(maps.kmap_bool);
    
    if (ones) free(ones) ; // Освобождаем память

    // TestStack();

    return 0;
}

// функция тестирования стека (можно удалить)
void TestStack()
{
    // создаем stack емкостью 5
    struct stack* pt = newStack(5);

    push(pt, 1);
    push(pt, 2);
    push(pt, 3);

    printf("The top element is %d\n", peek(pt));
    printf("The stack size is %d\n", size(pt));

    pop(pt);
    printf("The top element is %d\n", peek(pt));

    pop(pt);
    pop(pt);

    if (isEmpty(pt)) {
        printf("The stack is empty");
    }
    else {
        printf("The stack is not empty");
    }

}