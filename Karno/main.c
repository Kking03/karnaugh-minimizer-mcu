#include "Stack.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// макросы для минимизации
#define MAX_ONES 16                              // максимальное количество единиц функции
#define MAX_SIZE 4                               // максимальный размер карты Карно
#define INVALID_VALUE 255                        // флаг для ненужных элементов
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // проверка символа на соответствие цифре

// макросы для дисплея
#define DATA_PORT PORTA // Порт данных (DB0–DB7)
#define DATA_DDR DDRA   // Направление порта данных

#define RS PC0          // RS - Команда/Данные
#define RW PC1          // RW - Чтение/Запись
#define E  PC2          // Enable (строб)
#define CS1 PC3         // Выбор первого чипа
#define CS2 PC4         // Выбор второго чипа

#define CONTROL_PORT PORTC // Порт управления
#define CONTROL_DDR DDRC   // Направление порта управления

#define min(a, b) ((a) < (b) ? (a) : (b)) // макрос для вычисления границ

// глобальные переменные для минимизации
unsigned char ones[MAX_ONES];                          // глобальный массив для хранения позиций единиц
unsigned char N;                                       // количество переменных
unsigned char A, B;                                    // размерность карты Карно
unsigned char neighbors[MAX_ONES][MAX_SIZE];           // глобальное объявление матрицы соседних единиц
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];         // глобальная матрица кодов Грея
unsigned char kmap[MAX_SIZE][MAX_SIZE];                // глобальная матрица для карты Карно
unsigned char kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} }; // глобальная логическая матрица для карты Карно
unsigned char values[4];                               // Глобальный массив для хранения совпадающих бит
Stack pt;                                              // глобальное объявление указателя стека

// -------------------------------------- Реализация алгоритма минимизации --------------------------------------
// функция для создания массива единиц булевой функции
_Bool parse_numbers(char input[]) {

	// USARTTransmitStringLn("Вызвана функция для строки: ");
	// USARTTransmitString(input);

    unsigned char index = 0; // индекс для массива 'ones'

    // Заполняем массив ones флагами
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        ones[i] = INVALID_VALUE;
    }

    // пропуск пробелов в начале строки
    while (*input == ' ') input++;

    // проверка на цифру
    if (!isdigit(*input)) {
		// USARTTransmitStringLn("Выход 1");   // ОТЛАДКА
        return false;       // некорректное количество переменных
    }
    N = *input - '0';       // преобразование символа в число
    if (N < 2 || N > 4) {
		// USARTTransmitStringLn("Выход 2"); // ОТЛАДКА
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
			// USARTTransmitStringLn("Выход 3"); // ОТЛАДКА
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
            // printf("Ошибка: значение %d выходит за пределы допустимого диапазона (0-%d)\n", value, max_position);
			// USARTTransmitStringLn("Выход 4"); // ОТЛАДКА
            return false;
        }

        // Проверяем на дублирование
        for (unsigned char i = 0; i < index; i++) {
            if (ones[i] == value) {
                // printf("Ошибка: повторяющееся значение %d\n", value);
				// USARTTransmitStringLn("Выход 5"); // ОТЛАДКА
                return false;
            }
        }

        // Сохраняем значение в массив 'ones'
        if (index >= MAX_ONES) {
            // printf("Ошибка: слишком много единиц (максимум %d)\n", MAX_ONES);
			// USARTTransmitStringLn("Выход 6"); // ОТЛАДКА
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

// инициализации матрицы соседних единиц
void init_neighbors() {
    // Обнуление матрицы соседей
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            neighbors[i][j] = 0;
        }
    }
}

// Функция для подсчета количества одинаковых битов во всех строках из стека
uint8_t count_common_bits() {
     uint8_t common_bits_count = 0;

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
        for (unsigned char i = 1; i < size(&pt); i++) {
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
        result[i] = '0';
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

// Функция для поиска импликант (цикл)
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

        unsigned char bits;                             // число одинаковых бит
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
            	if (twoOnes_flag) { // разрешаем скелить две единицы
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
			 _Bool priority_flag;	// если включен, то в приоритете несклееные единицы

            // Смещения для четырех направлений: вправо, вниз, влево, вверх
            unsigned char dx[4] = { 0, 1, 0, -1 };  // Смещение по строкам
            unsigned char dy[4] = { 1, 0, -1, 0 };  // Смещение по столбцам

            // Проходим по направлению несклеенных единиц
	two_ones:
            priority_flag = true;                  // в приоритете несклееные единицы
            for (unsigned char k = 0; k < 4; k++) {
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
            return true; // является импликантой   
        }

        // push(&invalid, pop(&pt));  // помечаем как неподходящий и удаляем элемент
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

// Функция для перевода цифру в строку
void digit_to_string(char* digit, uint8_t num) {
    digit[0] = num + '0';
    digit[1] = '\0';
}

// -------------------------------------- Реализация работы с дисплеем ------------------------------------------
// глобальные переменные для работы с дисплеем
uint8_t displayBuffer[8][64] = {{0}};  // буфер для отображени карты Карно

// Шрифт для символов (5x7 пикселей)
const uint8_t customFont[][5] PROGMEM = {
    {0x00, 0x3E, 0x41, 0x41, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
	{0x00, 0x00, 0x7E, 0x00, 0x00}, // '1' без основания
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x00, 0x06, 0x00, 0x00, 0x00}  // `'` (апостроф)
};

// Маппинг символов к индексу массива
const char validChars[] = {'0', '1', '2', 'A', 'B', 'C', 'D', '\''}; // выводимые символы

// Отправка команды
void KS0108_Command(uint8_t cmd, uint8_t chip) {
    DATA_PORT = cmd;             // Отправляем команду
    CONTROL_PORT &= ~(1 << RS);  // RS = 0 (команда), исп. побитовую маску
    CONTROL_PORT &= ~(1 << RW);  // RW = 0 (запись)

    // Выбираем чип
    if (chip == 1) {
        CONTROL_PORT |= (1 << CS1);
        CONTROL_PORT &= ~(1 << CS2);
    } else {
        CONTROL_PORT |= (1 << CS2);
        CONTROL_PORT &= ~(1 << CS1);
    }

    // Строб для записи готовой команды
    CONTROL_PORT |= (1 << E);
    _delay_us(1);
    CONTROL_PORT &= ~(1 << E);

    // Задержка для выполнения команды контроллером дисплея
    _delay_ms(1);
}

// Отправка данных
void KS0108_Data(uint8_t data, uint8_t chip) {
    DATA_PORT = data;            // Отправляем данные
    CONTROL_PORT |= (1 << RS);   // RS = 1 (данные)
    CONTROL_PORT &= ~(1 << RW);  // RW = 0 (запись)

    // Выбираем чип
    if (chip == 1) {
        CONTROL_PORT |= (1 << CS1);
        CONTROL_PORT &= ~(1 << CS2);
    } else {
        CONTROL_PORT |= (1 << CS2);
        CONTROL_PORT &= ~(1 << CS1);
    }

    // Строб
    CONTROL_PORT |= (1 << E);
    _delay_us(1);
    CONTROL_PORT &= ~(1 << E);

    // Задержка
    _delay_ms(1);
}

// Инициализация дисплея
void KS0108_Init() {
	// Порт D и C на выход
    DATA_DDR = 0xFF;
    CONTROL_DDR |= (1 << RS) | (1 << RW) | (1 << E) | (1 << CS1) | (1 << CS2);

    KS0108_Command(0x3F, 1); // Включение первого чипа
    KS0108_Command(0x3F, 2); // Включение второго чипа
}

// Очистка дисплея
void KS0108_Clear() {
    for (uint8_t page = 0; page < 8; page++) {
        KS0108_Command(0xB8 + page, 1); // Устанавливаем страницу на первом чипе
        KS0108_Command(0x40, 1);       // Адрес столбца 0
        for (uint8_t col = 0; col < 64; col++) {
            KS0108_Data(0x00, 1);
        }
        KS0108_Command(0xB8 + page, 2); // Устанавливаем страницу на втором чипе
        KS0108_Command(0x40, 2);       // Адрес столбца 0
        for (uint8_t col = 0; col < 64; col++) {
            KS0108_Data(0x00, 2);
        }
    }
}

// Установка курсора
void KS0108_SetCursor(uint8_t x, uint8_t line) {
    uint8_t chip = (x < 64) ? 1 : 2;
    uint8_t address = (x < 64) ? x : (x - 64);

    KS0108_Command(0xB8 + line, chip);
    KS0108_Command(0x40 + address, chip);
}

// Функция для вывода буфера на дисплей
void RefreshDisplay() {
    for (uint8_t page = 0; page < 8; page++) {        // Проходим по всем страницам
        KS0108_Command(0xB8 + page, 1);              // Устанавливаем страницу
        KS0108_Command(0x40, 1);                     // Устанавливаем столбец 0

        for (uint8_t column = 0; column < 64; column++) {
            KS0108_Data(displayBuffer[page][column], 1); // Отправляем данные столбца
        }
    }
}

// Вывод символа
void KS0108_WriteChar(char c, uint8_t x, uint8_t y) {
    // Проверяем, является ли символ допустимым
    uint8_t found = 0xFF;
    for (uint8_t i = 0; i < sizeof(validChars); i++) {
        if (c == validChars[i]) {
            found = i;
            break;
        }
    }

    KS0108_SetCursor(x, y); // Устанавливаем курсор

    // Выводим символ из массива customFont
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t line = pgm_read_byte(&customFont[found][i]);
        KS0108_Data(line, (x < 64) ? 1 : 2); // Отправляем данные
    }

    KS0108_Data(0x00, (x < 64) ? 1 : 2); // Пробел между символами
}

// вывод строки с импликантой
void KS0108_WriteString(const char* str, uint8_t x, uint8_t y) {
    while (*str) {
        char c = *str++;
        if (c == '\'') {
            // Выводим апостроф чуть правее предыдущего символа
            KS0108_WriteChar(c, x - 1, y); // Апостроф ближе к букве
			x += 2;
        } else {
            // Выводим обычный символ
            KS0108_WriteChar(c, x, y);
            x += 6; // Сдвигаем позицию вправо для следующего символа
        }
    }
}

// Добавление пикселя (точки) на дисплей
void DrawPoint(uint8_t x, uint8_t y, _Bool clear) {
    uint8_t page = y / 8;       // Определяем страницу
    uint8_t bit = y % 8;        // Определяем бит в байте

    if (clear) {
        displayBuffer[page][x] &= ~(1 << bit); // Сбрасываем бит в буфере
    } else {
        displayBuffer[page][x] |= (1 << bit); // Устанавливаем бит в буфере
    }
}

// Рисование горищонтальной линии
void DrawHorizontalLine(uint8_t x1, uint8_t x2, uint8_t y, _Bool clear) {
    for (uint8_t x = x1; x <= x2; x++) {
        DrawPoint(x, y, clear); // Рисуем каждую точку на линии
    }
}

// Рисование вертикальной линии
void DrawVerticalLine(uint8_t x, uint8_t y1, uint8_t y2) {
    for (uint8_t y = y1; y <= y2; y++) {
        DrawPoint(x, y, false); // Рисуем каждую точку на линии
    }
}


// Функция для вывода символа
void DrawChar(char c, uint8_t x, uint8_t y) {
    if (c < '0' || c > '9') return; // Поддерживаются только цифры '0'-'9'

    uint8_t basePage = y / 8;             // Нижняя страница
    uint8_t bitOffset = y % 8;            // Смещение битов для нижней страницы

    for (uint8_t i = 0; i < 5; i++) {     // Проходим по каждому столбцу символа
        uint8_t colData = pgm_read_byte(&customFont[c - '0'][i]);

        // Разделяем данные для нижней и верхней страницы
        uint8_t lowerData = colData << bitOffset;       // Нижняя страница
        uint8_t upperData = colData >> (8 - bitOffset); // Верхняя страница

        // Обновляем буфер для нижней страницы
        if (basePage < 8) { // Проверка на выход за пределы
            displayBuffer[basePage][x + i] |= lowerData;
        }

        // Обновляем буфер для верхней страницы, если данные выходят за пределы
        if (bitOffset > 0 && (basePage + 1) < 8) { // Проверка на существование верхней страницы
            displayBuffer[basePage + 1][x + i] |= upperData;
        }
    }
}

// Рисование карты Карно
void DrawKarno(uint8_t N) {
	uint8_t A = (N / 2) * 2;                     // количество строк массива для N переменных
	uint8_t B = (N - (N / 2)) * 2;               // количество столбцов массива для N переменных
	uint8_t end_line = min((B + 1) * 13, 63);    // координата конца горизонтальной линии
	uint8_t end_column = min((A + 1) * 13, 63);  // координата конца вертикальной линии
	
	uint8_t code_gray[4] = { 0b00, 0b01, 0b11, 0b10 };

	// рисуем линии строк
	for (uint8_t line = 1; line <= A; line++) {
		DrawHorizontalLine(0, end_line, line * 13, false);
		// заполнение заголовков для строк для 4 переменных
		if (N == 4) {
			DrawChar(((code_gray[line-1] >> 1) & 1) + '0', 0, line * 13 + 4);
	    	DrawChar((code_gray[line-1] & 1) + '0', 5, line * 13 + 4);
		} else {
		    DrawChar((line-1) + '0', 3, line * 13 + 4);
		}

	}
	 // рисуем линии столбцов
	for (uint8_t column = 1; column <= B; column++) {
		DrawVerticalLine(column * 13, 0, end_column);
		// заполнение заголовков для столбцов для 3 и 4 переменных
		if (N == 3 || N == 4) {
			DrawChar(((code_gray[column-1] >> 1) & 1) + '0', column * 13 + 2, 4);
		    DrawChar((code_gray[column-1] & 1) + '0', column * 13 + 7, 4);
		} else {
			DrawChar((column-1) + '0', column * 13 + 4, 4);
		}
	}
	// рисуем диагональ
	for (uint8_t i = 0; i < 13; i++) {
        DrawPoint(i, i, false);
	}

	// заполнение карты единицами
	for (uint8_t row = 0; row < A; row++) {
		for (uint8_t col = 0; col < B; col++) {
			if (kmap[row][col] == 1) {
				uint8_t x = 17 + 13 * col;
				uint8_t y = 16 + 13 * row;
				DrawChar('2', x, y); // добавляем единицу на карту
			}
		}
	}
}

// Выделение склеенных единиц
void DrawImplicant() {
  	// отмечаем склеенные единицы
	for (unsigned char i = 0; i <= pt.top; i++) {
        uint8_t row = pt.items[i].row; // кооридната строки
        uint8_t col = pt.items[i].col; // координата столбца
    
	    uint8_t x1 = 16 + 13 * col;    // начало линии
	    uint8_t x2 = 23 + 13 * col;    // конец линии
	    uint8_t y = 24 + 13 * row;     // координата по высоте линии
	    DrawHorizontalLine(x1, x2, y, false);  
    }
	
	RefreshDisplay();     // Обновляем дисплей из буфера

	// убираем отмеченные склеенные единицы
	for (unsigned char i = 0; i <= pt.top; i++) {
        uint8_t row = pt.items[i].row; // кооридната строки
        uint8_t col = pt.items[i].col; // координата столбца
    
	    uint8_t x1 = 16 + 13 * col;    // начало линии
	    uint8_t x2 = 23 + 13 * col;    // конец линии
	    uint8_t y = 24 + 13 * row;     // координата по высоте линии
	    DrawHorizontalLine(x1, x2, y, true);  
    }

}

// Функция для очистки буфера
void ClearDisplayBuffer() {
    for (uint8_t page = 0; page < 8; page++) {
        for (uint8_t column = 0; column < 64; column++) {
            displayBuffer[page][column] = 0x00; // Обнуляем каждый байт буфера
        }
    }
}

// -------------------------------------- Реализация работы с кнопками ------------------------------------------


// проверка нажатия кнопки STEP(PD4)
_Bool stepButtonPressed() {
    return !(PIND & (1 << PIND4)); // Кнопка подключена к PIND4
}

void buttonInit() {
    DDRD &= ~(1 << PD4);  // Устанавливаем PD4 как вход
    PORTD |= (1 << PD4);  // Включаем внутренний подтягивающий резистор
}

volatile _Bool step_flag = false; // Переменная-флаг

void setup_interrupts() {
    // Настройка INT0 и INT1 на низкий уровень
    MCUCR |= (1 << ISC01) | (1 << ISC11); // INT0 и INT1 реагируют на спад
    GICR |= (1 << INT0) | (1 << INT1);   // Разрешаем прерывания INT0 и INT1

    // Настройка PD2 и PD3 как входы
    DDRD &= ~((1 << PD2) | (1 << PD3));  // Устанавливаем PD2 и PD3 как входы
    PORTD |= (1 << PD2) | (1 << PD3);    // Включаем подтяжку к Vcc
}

// прерывание по сигналу 0 на PD2
ISR(INT0_vect) {
    step_flag = true;  // Устанавливаем флаг
	USARTTransmitStringLn("Пошаговый режим: Включен\r");
}

// прерывание по сигналу 0 на PD3
ISR(INT1_vect) {
    step_flag = false; // Сбрасываем флаг
	USARTTransmitStringLn("Пошаговый режим: Выключен\r");
}

int main(void) {
	// Инициализация UART
  	USARTInit(MYUBRR);
  	USARTTransmitStringLn("Это программа минимизации БФ\r");

	KS0108_Init();       // Инициализация дисплея
    KS0108_Clear();      // Очистка дисплея


	buttonInit();        // Инициализация кнопки PD4
	setup_interrupts();  // Настройка прерываний
    sei(); // Глобальное разрешение прерываний

	uint8_t strSize = 50;  // Макс. размер строки
	char str[strSize];     // Массив для принимаемой строки
	char digit[2];		   // Буфер для вывода цифр

	while (1) {
		USARTTransmitStringLn("Введите функцию для минимизации\r");
    	USARTReceiveString(str, strSize); // Получение строки по UART

		USARTTransmitString("Введенная строка: ");
    	USARTTransmitStringLn(str);

		if (!parse_numbers(str)) {   // получена некорректная строка
    		USARTTransmitStringLn("Строка с ошибками. Повторите ввод!");
			USARTTransmitStringLn("");
			continue;
		}

		digit_to_string(digit, N);

		USARTTransmitString("Запущен процесс минимизации функции от ");
		USARTTransmitString(digit);
		USARTTransmitStringLn(" переменных");

	    // Размеры карты Карно
	    A = (N / 2) * 2;         // количество строк массива для N переменных
	    B = (N - (N / 2)) * 2;   // количество столбцов массива для N переменных

	    _Bool f_const = true;

		generate_gray_code_matrix(); // Генерация и вывод матрицы кода Грея
		fill_karnaugh_map();         // Заполнение карты Карно

		// Работа с дисплеем
		KS0108_Clear();       // Очистка дисплея
		ClearDisplayBuffer(); // очистка буфера дисплея
		DrawKarno(N);      	  // вызов функции для рисования карты Карно
		// DrawRectangle(15, 15, 24, 24);     // ОТЛАДКА
		RefreshDisplay();     // Обновляем дисплей из буфера	

	    // Проверка функции на константу
	    for (uint8_t i = 0; i < A; i++) {
	        for (uint8_t j = 0; j < B; j++)
	            if (kmap[i][j] == 0) {
	                f_const = false;  // значит, f != C
	                break;
	            }
		}	

	    // ОСНОВНОЙ АЛГОРИТМ
	    if (!f_const) {
	        initStack(&pt); 	         // инициализация стека
			init_neighbors();			 // инициализация матрицы соседних единиц
			uint8_t line = 0;			

	        char implicant[9]; // массив для результата	

	        for (uint8_t i = 0; i < A; i++) {
	            for (uint8_t j = 0; j < B; j++) {
	                if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
	                    // USARTTransmitStringLn("Вызвана функция минимизации:\n");
	                    if (minimize(i, j)) {
							count_common_bits();
							generate_implicant_string(implicant);

							// пошаговый вывод на дисплей
							if (step_flag) {
								// Ждём, пока кнопка будет отпущена
	            				while (stepButtonPressed()) {
	                				// Небольшая задержка для уменьшения нагрузки
	                				_delay_ms(10);
	            				}
							
								// Ждём, пока кнопка будет отпущена
								while (!stepButtonPressed()) {
	                				// Небольшая задержка для уменьшения нагрузки
	                				_delay_ms(10);
	            				}
								_delay_ms(100); // Задержка перед выводом

								
								// рисование импликанты на карте
								DrawImplicant();
							}

							KS0108_WriteString(implicant, 80, line++); // вывод выражения на дисплей

							// ОТЛАДКА
							USARTTransmitString("\rПолучена");
							//digit_to_string(digit, ++num);
							//USARTTransmitString(digit);
							USARTTransmitString(" импликанта ");
							USARTTransmitStringLn(implicant);
							
							// Вывод стека
							USARTTransmitString("Stack: ");
						    for (uint8_t i = 0; i <= pt.top; i++) {

								digit_to_string(digit, pt.items[i].row);
								USARTTransmitString(digit); // Отправляем строку по UART

								digit_to_string(digit, pt.items[i].col);
								USARTTransmitString(digit); // Отправляем строку по UART
								USARTTransmitString(" ");
							}	
							USARTTransmitString("\r");						

							init_neighbors();             // очистка матрицы соседних единиц
	                        clear(&pt);                   // очистка стека
	                    }
	                }
	            }
	        }
			USARTTransmitStringLn("\rМинимизация завершена\r");
	    }
	    else
	    {
	        USARTTransmitStringLn("\rВведённая функция f = 1\r"); // ОТЛАДКА
	    }	
    }

	return 0;
}
