// макросы
#define MAX_ONES 16                              // максимальное количество единиц функции
#define MAX_SIZE 4                               // максимальный размер карты Карно
#define INVALID_VALUE 255                        // флаг для ненужных элементов
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // проверка символа на соответствие цифре

#include <locale.h>  // ОТЛАДКА
#include "Stack.h"

unsigned char ones[MAX_ONES];                          // глобальный массив для хранения позиций единиц
unsigned char N;                                       // количество переменных
unsigned char neighbors[MAX_ONES][MAX_SIZE];           // глобальное объявление матрицы соседних единиц
unsigned char A, B;                                    // размерность карты Карно
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];         // глобальная матрица кодов Грея
unsigned char kmap[MAX_SIZE][MAX_SIZE];                // глобальная матрица для карты Карно
unsigned char kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} }; // глобальная логическая матрица для карты Карно
unsigned char values[4];                               // Глобальный массив для хранения совпадающих бит
Stack pt;                                              // глобальное объявление указателя стека

// функция для создания массива единиц булевой функции
_Bool parse_numbers(char input[]) {
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
    if (N < 2 || N > 4) {
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
void generate_gray_code_matrix() {
    // Массив для генерации кода Грея
    const unsigned char code_gray[] = { 0b00, 0b01, 0b11, 0b10 };

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
            kmap_bool[i][j] = 0;
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

        _Bool all_match = true;

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

void generate_implicant_string(char result[9]) {
    // Инициализация строки как пустой
    for (unsigned char i = 0; i < 8; i++) {
        result[i] = '\0';
    }

    // Помечаем единицы как склеенные
    for (unsigned char i = 0; i <= pt.top; i++) {
        unsigned char row = pt.items[i].row;
        unsigned char col = pt.items[i].col;
        kmap_bool[row][col] = true;
    }

    // Получаем код Грея из первого элемента стека
    unsigned char gray_value = gray_matrix[pt.items[0].row][pt.items[0].col];

    // Индекс для записи в строку
    unsigned char index = 0;

    // Формируем строку на основе values и gray_value
    for (unsigned char i = 0; i < N; i++) {
        if (values[i] == 1) {
            result[index++] = 'A' + i; // Добавляем переменную
            if (((gray_value >> (N - 1 - i)) & 1) == 0) { // Проверяем бит переменной
                result[index++] = '\''; // Добавляем апостроф, если бит == 0
            }
        }
    }

    // Завершаем строку символом конца строки
    result[index] = '\0';
}

/*
// Функция для поиска импликант
_Bool minimize(unsigned char x, unsigned char y)
{
    // добавление элемента в стек
    push(&pt, (IndexPair) { x, y });

    int bits;                                     // число одинаковых бит
    _Bool match = (size(&pt) == 1) ? true : false;  // флаг, указывающий, что единица подходит для склейки
    _Bool break_flag = false;                      // флаг устанавливается для тупиковой единицы

    bits = (size(&pt) == 1) ? N : count_common_bits();  // проверяем количество совпадаемых бит
    if (bits == 0)
        break_flag = true;         // единица не подходит
    else
        switch (size(&pt))
        {
        case 2: // перенести match для 2 вниз???
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

        // Формирование массива соседних элементов для просмотра
        unsigned char l_ind = 0;
        unsigned char r_ind = 3;
        unsigned char coord[4][2];

        for (int k = 0; k < 4; k++) {
            int nx = (x + dx[k] + A) % A;  // Новая координата по строке
            int ny = (y + dy[k] + B) % B;  // Новая координата по столбцу

            if (kmap_bool[nx][ny] == false) { // приоритетная непомеченная единица
                coord[l_ind][0] = nx;
                coord[l_ind][1] = ny;
                l_ind++;
            }
            else {
                coord[r_ind][0] = nx;
                coord[r_ind][1] = ny;
                r_ind -= 1;
            }
        }

        // Проходим по каждому направлению
        for (int k = 0; k < 4; k++) {
            unsigned char nx = coord[k][0];
            unsigned char ny = coord[k][1];
            if ((kmap[nx][ny] == 1) && (!include(&pt, (IndexPair) { nx, ny }))) {
                _Bool res = minimize(nx, ny);
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
*/

/*

*/
// цикл 
_Bool minimize(unsigned char x, unsigned char y)
{
    IndexPair coord;              // координаты единицы
    _Bool added_flag = false;     // флаг, что элемент уже в стеке
    _Bool twoOnes_flag = false;   // флаг разрешения склеики двух единиц

    while (1) {
        // добавление элемента в стек
        if (!added_flag) {
            push(&pt, (IndexPair) { x, y });
        }

        added_flag = false;

        int bits;                                       // число одинаковых бит
        _Bool match = (size(&pt) == 1) ? true : false;  // флаг, указывающий, что единица подходит для склейки
        _Bool break_flag = false;                       // флаг устанавливается для тупиковой единицы
        _Bool next_flag = false;                        // флаг для продолжения поиска
        unsigned char ind = x * 4 + y;                  // Индекс для массива соседей

        bits = (size(&pt) == 1) ? N : count_common_bits();  // проверяем количество совпадаемых бит
        if (bits == 0)
            break_flag = true;         // единица не подходит
        else
            switch (size(&pt))
            {
            case 2:
                if (twoOnes_flag) {
                    match = true;      
                    break_flag = true;
                }
                    
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
            _Bool priority_flag;
            // Смещения для четырех направлений: вправо, вниз, влево, вверх
            unsigned char dx[4] = { 0, 1, 0, -1 };  // Смещение по строкам
            unsigned char dy[4] = { 1, 0, -1, 0 };  // Смещение по столбцам

            // Проходим по направлению несклеенных единиц
    two_ones:
            priority_flag = true;                  // если включен, то в приоритете несклееные единицы
            for (int k = 0; k < 4; k++) {
                unsigned char nx = (x + dx[k] + A) % A;  // Новая координата по строке
                unsigned char ny = (y + dy[k] + B) % B;  // Новая координата по столбцу

                if ((kmap[nx][ny] == 1) && (!kmap_bool[nx][ny] + !priority_flag) && (!include(&pt, (IndexPair) { nx, ny })) && (neighbors[ind][k] == 0)) {

                    // запоминаем соседей
                    neighbors[ind][k] = 1;
                    x = nx;
                    y = ny;
                    next_flag = true; // продолжить поиск для соседней единицы
                    break;
                }
                if (priority_flag && k == 3) { // попытка поиска без приоритета
                    priority_flag = false;
                    k = -1;
                }
            }

            if (next_flag) continue;

            // склейка для двух единиц
            if ((size(&pt)) == 1 && !twoOnes_flag) {
                twoOnes_flag = true;
                // удаление просмотренных соседей
                for (unsigned char i = 0; i < 4; i++) {
                    neighbors[ind][i] = 0;
                }
                goto two_ones;
            }
        }  

        // найдена наибольшая импликанта
        if (match) {
            return true;      // является импликантой   
        }

        // удаление просмотренных соседей
        for (unsigned char i = 0; i < 4; i++) {
            neighbors[ind][i] = 0;
        }
        pop(&pt);                  // удаление неподходящего элемента

        coord = peek(&pt);         // вовзращаемся к предыдущей единице
        x = coord.row;
        y = coord.col;
        added_flag = true;
    }
}

void init_neighbors() {
    // Обнуление матрицы соседей
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            neighbors[i][j] = 0;
        }
    }
}

/*
// Функция для создания динамической матрицы neighbors
IndexPair** create_neighbors() {
    // Выделение памяти для массива указателей на строки
    IndexPair** neighbors = (IndexPair**)malloc(n_ones * sizeof(IndexPair*));

    // Выделение памяти для каждой строки (по 5 элементов)
    for (unsigned char i = 0; i < n_ones; i++) {
        neighbors[i] = (IndexPair*)malloc(5 * sizeof(IndexPair));
    }

    
    // Заполнение матрицы
    unsigned char ind = 0;
    for (unsigned char i = 0; i < A; i++)
        for (unsigned char j = 0; j < B; j++)
            if (kmap[i][j] == 1) {
                neighbors[ind][0].row = i;
                neighbors[ind][0].col = j;
                ind++;
            }

    return neighbors;
}

// Функция для освобождения памяти, выделенной для neighbors
void free_neighbors() {
    for (unsigned char i = 0; i < n_ones; i++) {
        free(neighbors[i]); // Освобождаем каждую строку
    }
    free(neighbors); // Освобождаем массив указателей
}
*/

//---------------------------------------- ОТЛАДОЧНЫЕ ФУНКЦИИ ----------------------------------------
void TestStack();                                                          // функция тестирования стека
void printOnes();                                                          // Фукнция для вывода массива единиц
void print_karnaugh_map();                                                 // Функция для печати карты Карно
void print_karnaugh_map_bool(unsigned char A, unsigned char  B);           // Функция для печати логической карты Карно
void print_gray_matrix();                                                  // Функция для печати матрицы кода Грея
void TestBitCountFunc();                                                   // функция тестирования подсчета одинаковых бит

int main()
{
    setlocale(LC_ALL, "Rus");

    // вводимая пользователем строка
    char str[10][40] = {
    "4 0 2 8 10 1 5",
    "4 3 7 11 15", // 3 столбец
    "4 2 6 10 14", // 4 столбец
    "4 1 5 9 13",  // 2 столбец
    "4 0 4 8 12"   // 1 столбец
    };


    for (int i = 0; i < 1; i++)
    {

        printf("\nВведённая строка: %s\n", str[i]);

        if (!parse_numbers(str[i])) {   // получена некорректная строка
            // ОТЛАДКА
            printf("Ошибка в строке:\n");

            continue;
        }

        // печать единиц
        printOnes();

        // Размеры карты Карно
        A = (N / 2) * 2;         // количество строк массива для N переменных
        B = (N - (N / 2)) * 2;   // количество столбцов массива для N переменных

        generate_gray_code_matrix();  // Генерация и вывод матрицы кода Грея

        // Заполнение карты Карно
        fill_karnaugh_map();

        _Bool f_const = true;

        // Проверка на функцию-константу
        for (int i = 0; i < A; i++)
            for (int j = 0; j < B; j++)
                if (kmap[i][j] == 0) {
                    f_const = false;  // значит, f != C
                    break;
                }

        // ОТЛАДКА
        print_gray_matrix();
        print_karnaugh_map();

        // ОСНОВНОЙ АЛГОРИТМ
        if (!f_const) {
            initStack(&pt); // инициализация стека
            init_neighbors();

            char implicant[9]; // массив для результата


            int num = 1; // ОТЛАДКА

            // основной алгоритм
            for (int i = 0; i < A; i++) {
                for (int j = 0; j < B; j++) {
                    if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
                        printf("Вызвана функция минимизации:\n");
                        if (minimize(i, j)) {

                            count_common_bits();
                            generate_implicant_string(implicant);


                            // ОТЛАДКА
                            printf("Получена импликанта %d: %s\n", num++, implicant);;
                            // ОТЛАДОЧНЫЙ ВЫВОД СТЕКА
                            printf("Stack: \n");
                            for (unsigned char i = 0; i <= pt.top; i++) {
                                printf("%d", pt.items[i].row);
                                printf("%d\n", pt.items[i].col);
                            }
                            printf("Массив совпадений: ");
                            for (unsigned char i = 0; i < N; i++) {
                                printf("%d ", values[i]);
                            }
                            printf("\n\n");

                            init_neighbors();             // очистка матрицы соседних единиц
                            clear(&pt);                   // очистка стека
                        }
                    }
                }
            }
        }
        else
        {
            printf("Введённая функция f = 1:\n");
        }

        // ОТЛАДКА
        if (kmap_bool != NULL) {
            printf("Карта Карно (в виде бинарной логической матрицы):\n");
            print_karnaugh_map_bool(A, B);
        }
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
void print_gray_matrix() {
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
    push(&sp, (IndexPair) { 1, 0 });
    push(&sp, (IndexPair) { 2, 0 });
    push(&sp, (IndexPair) { 3, 0 });

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
    pop(&sp);
    pop(&sp);

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
    _Bool res = include(&pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "найден" : "не найден");

    printf("The stack size is %d\n", size(&pt));

    // Удаление элементов из стека
    pop(&pt);
    pop(&pt);
    pop(&pt);
}

/*
clear(&pt);
push(&pt, (IndexPair) { 0, 0 });
push(&pt, (IndexPair) { 1, 0 });
push(&pt, (IndexPair) { 2, 0 });
push(&pt, (IndexPair) { 3, 0 });

printf("\n-----ТЕСТИРОВАНИЕ подсчёта бит-----\n");

// Проверка функции подсчёта
printf("одинаковых бит: %d\n", count_common_bits());
printf("Массив совпадений: ");
for (unsigned char i = 0; i < 4; i++) {
    printf("%d ", values[i]);
}
printf("\n");
*/
// ТЕСТ

// линейный поиск импликант

            /*
            if (N == 4) {
                _Bool full_line;
                unsigned char find[4] = { {0} };

                // по строкам
            printf("-----------------------ПОИСК ПО СТРОКАМ\n");
            for (unsigned char row = 0; row < 4; row++) {
                full_line = true;
                for (unsigned char col = 0; col < 4; col++) {
                    if (kmap[row][col] != 1) {
                        full_line = false;
                        break;
                    }
                }
                if (full_line) {
                    find[row] = 1;
                }
            }

            // отмечаем импликанты
            for (unsigned char i = 0; i < 4; i++) {
                if (find[i] == 1 && find[(i + 1) % 4] != 1 && find[(i + 3) % 4] != 1) {
                    for (unsigned char j = 0; j < 4; j++) {
                        kmap_bool[i][j] = true;
                        push(&pt, (IndexPair) { i, j });
                    }

                    count_common_bits();
                    generate_implicant_string(implicant);

                    // ОТЛАДКА
                    printf("Получена импликанта %d: %s\n", num++, implicant);;
                    // ОТЛАДОЧНЫЙ ВЫВОД СТЕКА
                    printf("Stack: \n");
                    for (unsigned char i = 0; i <= pt.top; i++) {
                        printf("%d", pt.items[i].row);
                        printf("%d\n", pt.items[i].col);
                    }
                    printf("Массив совпадений: ");
                    for (unsigned char i = 0; i < N; i++) {
                        printf("%d ", values[i]);
                    }
                    printf("\n");

                    clear(&pt);
                }
            }
            // отладка
            printf("Массив подходящих строк: ");
            for (int i = 0; i < 4; i++) {
                printf("%d ", find[i]);
                find[i] = 0;
            }

            // по столбцам

                printf("\n\n-----------------------ПОИСК ПО СТОЛБЦАМ\n");
                for (unsigned char col = 0; col < 4; col++) {
                    full_line = true;
                    for (unsigned char row = 0; row < 4; row++) {
                        if (kmap[row][col] != 1) {
                            full_line = false;
                            break;
                        }
                    }
                    if (full_line) {
                        find[col] = 1;
                    }
                }

                // отмечаем импликанты
                for (unsigned char col = 0; col < 4; col++) {
                    if (find[col] == 1 && find[(col + 1) % 4] != 1 && find[(col + 3) % 4] != 1) {
                        for (unsigned char row = 0; row < 4; row++) {
                            kmap_bool[row][col] = true;
                            push(&pt, (IndexPair) { row, col });
                        }

                        count_common_bits();
                        generate_implicant_string(implicant);

                        // ОТЛАДКА
                        printf("Получена импликанта %d: %s\n", num++, implicant);;
                        // ОТЛАДОЧНЫЙ ВЫВОД СТЕКА
                        printf("Stack: \n");
                        for (unsigned char i = 0; i <= pt.top; i++) {
                            printf("%d", pt.items[i].row);
                            printf("%d\n", pt.items[i].col);
                        }
                        printf("Массив совпадений: ");
                        for (unsigned char i = 0; i < N; i++) {
                            printf("%d ", values[i]);
                        }
                        printf("\n");

                        clear(&pt);
                    }
                }
                printf("Массив подходящих столбцов: ");
                for (int i = 0; i < 4; i++) {
                    printf("%d ", find[i]);
                }

                printf("\n\n-----------------------ПОИСК ПО СТОЛБЦАМ\n\n");
            }
            */