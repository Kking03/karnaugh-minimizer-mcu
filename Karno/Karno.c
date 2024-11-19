// ЗАДАНИЕ
/*
1 Минимизация для f = Const
2 Добавление импликант в список
*/

// Karno.c

// макросы
#define MAX_ONES 16                              // максимальное количество единиц функции
#define MAX_SIZE 4                               // максимальный размер карты Карно
#define INVALID_VALUE 255                        // 
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // проверка символа на соответствие цифре

#include <locale.h>

#include "Stack.h"

unsigned char ones[MAX_ONES];                   // глобальный массив для хранения позиций единиц
unsigned char N;                                // количество переменных
unsigned char A, B;                             // размерность карты Карно
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];  // глобальная матрица кодов Грея
unsigned char kmap[MAX_SIZE][MAX_SIZE];         // глобальная матрица для карты Карно
bool kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} };   // глобальная логическая матрица для карты Карно
unsigned char values[4];                        // Глобальный массив для хранения совпадающих бит
Stack pt;                                      // глобальное объявление указателя стека


// Массив для генерации кода Грея
const unsigned char code_gray[] = { 0b00, 0b01, 0b11, 0b10 };

// функция для создания массива единиц булевой функции
bool parse_numbers(char input[]) {
    unsigned char index = 0; // индекс для массива 'ones'

    // Заполняем массив ones флагами
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        ones[i] = INVALID_VALUE;
    }

    // пропуск пробелов в начале строки
    while (*input == ' ') input++;

    // проверка на цифру
    if (!isdigit(*input)) {
        return false;       // некорректное количество переменных
    }
    N = *input - '0';       // преобразование символа в число
    if (N < 1 || N > 4) {
        return false;       // некорректное количество переменных
    }
    input++; // переходим к следующему символу

    // пропускаем пробелов
    while (*input == ' ') input++;

    // Максимальная позиция для единицы: 2^N - 1
    unsigned char max_position = (1 << N) - 1;

    // Читаем позиции единиц
    while (*input != '\0') {
        // Проверяем, является ли текущий символ цифрой
        if (!isdigit(*input)) {
            return false;
        }

        // Преобразуем число
        unsigned char value = 0;
        while (isdigit(*input)) {
            value = value * 10 + (*input - '0');
            input++;
        }

        // Проверяем, что значение в допустимом диапазоне
        if (value < 0 || value > max_position) {
            printf("Ошибка: значение %d выходит за пределы допустимого диапазона (0-%d)\n", value, max_position);
            return false;
        }

        // Проверяем на дублирование
        for (unsigned char i = 0; i < index; i++) {
            if (ones[i] == value) {
                printf("Ошибка: повторяющееся значение %d\n", value);
                return false;
            }
        }

        // Сохраняем значение в массив 'ones'
        if (index >= MAX_ONES) {
            printf("Ошибка: слишком много единиц (максимум %d)\n", MAX_ONES);
            return false;
        }
        ones[index++] = value;

        // Пропускаем пробелы
        while (*input == ' ') input++;
    }

    return true;
}

// Функция для создания двумерной матрицы кода Грея для карты Карно
void generate_gray_code_matrix(unsigned char N) {
    // Размеры карты Карно
    unsigned char A = (N / 2) * 2;   // количество строк
    unsigned char B = (N - (N / 2)) * 2; // количество столбцов

    // Инициализация всей матрицы нулями
    for (unsigned char i = 0; i < MAX_SIZE; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            gray_matrix[i][j] = 0;
        }
    }

    // Заполнение матрицы в зависимости от количества переменных
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            if (N == 2) {
                // Для 2 переменных используем последовательность 00, 01, 10, 11
                gray_matrix[i][j] = i * 2 + j;
            }
            else {
                // Для 3 и 4 переменных используем `code_gray` для строк и столбцов
                unsigned char row_code = code_gray[i]; // Код строки
                unsigned char col_code = code_gray[j]; // Код столбца

                // Для 3 переменных объединяем 1 бит строки и 2 бита столбца
                if (N == 3) {
                    gray_matrix[i][j] = (row_code << 2) | col_code;
                }

                // Для 4 переменных объединяем 2 бита строки и 2 бита столбца
                if (N == 4) {
                    gray_matrix[i][j] = (row_code << 2) | col_code;
                }
            }
        }
    }
}

// Функция для заполнения карты Карно
void fill_karnaugh_map() {
    // Инициализация всей карты Карно нулями
    for (unsigned char i = 0; i < MAX_SIZE; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            kmap[i][j] = 0;
        }
    }

    // Устанавливаем единицы в соответствии с массивом ones
    for (unsigned char k = 0; k < MAX_ONES; k++) {
        if (ones[k] == INVALID_VALUE) break; // Прекращаем, если достигли конца значимых значений

        unsigned char target_index = ones[k];
        for (unsigned char i = 0; i < A; i++) {
            for (unsigned char j = 0; j < B; j++) {
                if (gray_matrix[i][j] == target_index) {
                    kmap[i][j] = 1; // Устанавливаем единицу
                    break;
                }
            }
        }
    }
}

// Функция для подсчета количества одинаковых битов во всех строках из стека
int count_common_bits() {
    int common_bits_count = 0;

    // Инициализация массива values нулями
    for (unsigned char i = 0; i < 4; i++) {
        values[i] = 0;
    }

    // Проверяем каждый бит (позицию) в строках кода Грея
    for (unsigned char bit = 0; bit < N; bit++) {
        // Получаем значение первого элемента из стека
        unsigned char first_value = gray_matrix[pt.items[0].row][pt.items[0].col];
        unsigned char first_bit = (first_value >> bit) & 1;

        bool all_match = true;

        // Проверяем этот бит у всех элементов стека
        for (int i = 1; i < size(&pt); i++) {
            unsigned char current_value = gray_matrix[pt.items[i].row][pt.items[i].col];
            unsigned char current_bit = (current_value >> bit) & 1;

            if (current_bit != first_bit) {
                all_match = false;
                break;
            }
        }

        // Если на данной позиции биты совпадают у всех, обновляем values и счётчик
        if (all_match) {
            values[N - 1 - bit] = 1;
            common_bits_count++;
        }
    }

    return common_bits_count;
}

// Функция для поиска импликант
bool minimize(unsigned char x, unsigned char y)
{
    // добавление элемента в стек
    push(&pt, (IndexPair) { x, y });

    int bits;                                     // число одинаковых бит
    bool match = (size(&pt) == 1) ? true : false;  // флаг, указывающий, что единица подходит для склейки
    bool break_flag = false;                      // флаг устанавливается для тупиковой единицы

    bits = (size(&pt) == 1) ? N : count_common_bits();  // проверяем количество совпадаемых бит
    if (bits == 0)
        break_flag = true;         // единица не подходит
    else
        switch (size(&pt))
        {
        case 2:
            match = true;
            if (N == 2) break_flag = true;  // для двух переменных
            break;
        case 4:
            if (bits == 1 && N == 3)        // для трёх переменных
            {
                match = true;
                break_flag = true;
            }
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

            if ((kmap[nx][ny] == 1) && (!include(&pt, (IndexPair) { nx, ny }))) {
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
        kmap_bool[x][y] = true; // единица принадлежит импликанте ПРОВЕРИТЬ
        return true; // является импликантой   
    }

    pop(&pt);         // удаляем элемент
    return false;    // соседи отсутствуют
}

//---------------------------------------- ОТЛАДОЧНЫЕ ФУНКЦИИ ----------------------------------------
void TestStack();                                                          // функция тестирования стека
void printOnes();                                                          // Фукнция для вывода массива единиц
void print_karnaugh_map();                                                 // Функция для печати карты Карно
void print_karnaugh_map_bool(unsigned char A, unsigned char  B);           // Функция для печати логической карты Карно
void print_gray_matrix(unsigned char A, unsigned char B);                  // Функция для печати матрицы кода Грея
void TestBitCountFunc();                                                   // функция тестирования подсчета одинаковых бит

int main()
{
    setlocale(LC_ALL, "Rus");

    // вводимая пользователем строка
    char str[] = " 2 0  1 2  ";

    if (parse_numbers(str)) {
        printf("Количество переменных: %d\n", N);
        printOnes();
    }
    else {
        printf("Ошибка в формате строки\n");
    }

    // Размеры карты Карно
    A = (N / 2) * 2;         // количество строк массива для N переменных
    B = (N - (N / 2)) * 2;   // количество столбцов массива для N переменных

    generate_gray_code_matrix(N);  // Генерация и вывод матрицы кода Грея
    print_gray_matrix(A, B);       // ОТЛАДОЧНЫЙ ВЫВОД

    // Заполнение карты Карно
    fill_karnaugh_map();
    // Печать карты Карно
    print_karnaugh_map();


    // TestBitCountFunc(); // ТЕСТИРОВАНИЕ ФУНКЦИИ ПОДСЧЁТА КОЛИЧЕСТВА БИТ

    // ОСНОВНОЙ АЛГОРИТМ
    initStack(&pt); // инициализация стека

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
                printf("Вызвана функция минимизации:\n");
                if (minimize(i, j)) {
                    clear(&pt);                   // очитска стека
                    printf("Массив совпадений: ");
                    for (unsigned char i = 0; i < N; i++) {
                        printf("%d ", values[i]);
                    }
                    printf("\n");
                }
            }
        }
    }

    // Печать карты Карно с помеченными единицами
    if (kmap_bool != NULL) {
        printf("Карта Карно (в виде бинарной логической матрицы):\n");
        print_karnaugh_map_bool(A, B);
    }

    return 0;
}

//------------------------------------------реализация------------------------------------------
// Фукнция для вывода массива единиц
void printOnes()
{
    printf("Массив единиц: ");
    for (int i = 0; i < MAX_ONES; i++) {
        if (ones[i] != INVALID_VALUE) {
            printf("%d ", ones[i]);
        }
    }
    printf("\n");
}

// Функция для печати матрицы кода Грея
void print_gray_matrix(unsigned char A, unsigned char B) {
    printf("Матрица кода Грея:\n");
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            // Преобразуем значение в двоичный вид вручную
            for (int bit = N - 1; bit >= 0; bit--) {
                putchar((gray_matrix[i][j] & (1 << bit)) ? '1' : '0');
            }
            printf(" "); // Разделяем элементы пробелом
        }
        printf("\n");
    }
}


// Функция для печати карты Карно
void print_karnaugh_map() {
    printf("Карта Карно:\n");
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            printf("%d ", kmap[i][j]);
        }
        printf("\n");
    }
}

// Функция для печати логической карты Карно (для отладки)
void print_karnaugh_map_bool(unsigned char A, int B) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            printf("%5s ", kmap_bool[i][j] ? "true" : "false");
        }
        printf("\n");
    }
}

void TestBitCountFunc()
{
    printf("\n-----ТЕСТИРОВАНИЕ подсчёта бит-----\n");

    Stack sp;

    initStack(&sp);

    // Добавления координат в стек
    push(&sp, (IndexPair) { 0, 0 });
    push(&sp, (IndexPair) { 0, 1 });
    //push(pt, (IndexPair) { 1, 0 });
    //push(pt, (IndexPair) { 1, 1 });

    // Проверка функции подсчёта
    printf("одинаковых бит: %d\n", count_common_bits());
    printf("Массив совпадений: ");
    for (unsigned char i = 0; i < 4; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    // Удаление элементов из стека
    pop(&sp);
    pop(&sp);
    //pop(pt);
    //pop(pt);

    // Освобождение памяти
}

// функция тестирования стека (можно удалить)
void TestStack()
{
    printf("-----ТЕСТИРОВАНИЕ стека-----\n");
    Stack sp;

    initStack(&sp);

    // Пример добавления координат в стек
    push(&sp, (IndexPair) { 1, 2 });
    push(&sp, (IndexPair) { 3, 4 });
    push(&sp, (IndexPair) { 5, 6 });

    // Проверка, содержит ли стек определенную позицию
    bool res = include(&pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "найден" : "не найден");

    printf("The stack size is %d\n", size(&pt));

    // Удаление элементов из стека
    pop(&pt);
    pop(&pt);
    pop(&pt);
}

