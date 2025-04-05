#include "Stack.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// ������� ��� �����������
#define MAX_ONES 16                              // ������������ ���������� ������ �������
#define MAX_SIZE 4                               // ������������ ������ ����� �����
#define INVALID_VALUE 255                        // ���� ��� �������� ���������
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // �������� ������� �� ������������ �����

// ������� ��� �������
#define DATA_PORT PORTA // ���� ������ (DB0�DB7)
#define DATA_DDR DDRA   // ����������� ����� ������

#define RS PC0          // RS - �������/������
#define RW PC1          // RW - ������/������
#define E  PC2          // Enable (�����)
#define CS1 PC3         // ����� ������� ����
#define CS2 PC4         // ����� ������� ����

#define CONTROL_PORT PORTC // ���� ����������
#define CONTROL_DDR DDRC   // ����������� ����� ����������

#define min(a, b) ((a) < (b) ? (a) : (b)) // ������ ��� ���������� ������

// ���������� ���������� ��� �����������
unsigned char ones[MAX_ONES];                          // ���������� ������ ��� �������� ������� ������
unsigned char N;                                       // ���������� ����������
unsigned char A, B;                                    // ����������� ����� �����
unsigned char neighbors[MAX_ONES][MAX_SIZE];           // ���������� ���������� ������� �������� ������
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];         // ���������� ������� ����� ����
unsigned char kmap[MAX_SIZE][MAX_SIZE];                // ���������� ������� ��� ����� �����
unsigned char kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} }; // ���������� ���������� ������� ��� ����� �����
unsigned char values[4];                               // ���������� ������ ��� �������� ����������� ���
Stack pt;                                              // ���������� ���������� ��������� �����

// -------------------------------------- ���������� ��������� ����������� --------------------------------------
// ������� ��� �������� ������� ������ ������� �������
_Bool parse_numbers(char input[]) {

	// USARTTransmitStringLn("������� ������� ��� ������: ");
	// USARTTransmitString(input);

    unsigned char index = 0; // ������ ��� ������� 'ones'

    // ��������� ������ ones �������
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        ones[i] = INVALID_VALUE;
    }

    // ������� �������� � ������ ������
    while (*input == ' ') input++;

    // �������� �� �����
    if (!isdigit(*input)) {
		// USARTTransmitStringLn("����� 1");   // �������
        return false;       // ������������ ���������� ����������
    }
    N = *input - '0';       // �������������� ������� � �����
    if (N < 2 || N > 4) {
		// USARTTransmitStringLn("����� 2"); // �������
        return false;       // ������������ ���������� ����������
    }
    input++; // ��������� � ���������� �������

    // ���������� ��������
    while (*input == ' ') input++;

    // ������������ ������� ��� �������: 2^N - 1
    unsigned char max_position = (1 << N) - 1;

    // ������ ������� ������
    while (*input != '\0') {
        // ���������, �������� �� ������� ������ ������
        if (!isdigit(*input)) {
			// USARTTransmitStringLn("����� 3"); // �������
            return false;
        }

        // ����������� �����
        unsigned char value = 0;
        while (isdigit(*input)) {
            value = value * 10 + (*input - '0');
            input++;
        }

        // ���������, ��� �������� � ���������� ���������
        if (value < 0 || value > max_position) {
            // printf("������: �������� %d ������� �� ������� ����������� ��������� (0-%d)\n", value, max_position);
			// USARTTransmitStringLn("����� 4"); // �������
            return false;
        }

        // ��������� �� ������������
        for (unsigned char i = 0; i < index; i++) {
            if (ones[i] == value) {
                // printf("������: ������������� �������� %d\n", value);
				// USARTTransmitStringLn("����� 5"); // �������
                return false;
            }
        }

        // ��������� �������� � ������ 'ones'
        if (index >= MAX_ONES) {
            // printf("������: ������� ����� ������ (�������� %d)\n", MAX_ONES);
			// USARTTransmitStringLn("����� 6"); // �������
            return false;
        }
        ones[index++] = value;

        // ���������� �������
        while (*input == ' ') input++;
    }

    return true;
}

// ������� ��� �������� ��������� ������� ���� ���� ��� ����� �����
void generate_gray_code_matrix() {
    // ������ ��� ��������� ���� ����
    const unsigned char code_gray[] = { 0b00, 0b01, 0b11, 0b10 };

    // ������� ����� �����
    unsigned char A = (N / 2) * 2;   // ���������� �����
    unsigned char B = (N - (N / 2)) * 2; // ���������� ��������

    // ������������� ���� ������� ������
    for (unsigned char i = 0; i < MAX_SIZE; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            gray_matrix[i][j] = 0;
        }
    }

    // ���������� ������� � ����������� �� ���������� ����������
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            if (N == 2) {
                // ��� 2 ���������� ���������� ������������������ 00, 01, 10, 11
                gray_matrix[i][j] = i * 2 + j;
            }
            else {
                // ��� 3 � 4 ���������� ���������� `code_gray` ��� ����� � ��������
                unsigned char row_code = code_gray[i]; // ��� ������
                unsigned char col_code = code_gray[j]; // ��� �������

                // ��� 3 ���������� ���������� 1 ��� ������ � 2 ���� �������
                if (N == 3) {
                    gray_matrix[i][j] = (row_code << 2) | col_code;
                }

                // ��� 4 ���������� ���������� 2 ���� ������ � 2 ���� �������
                if (N == 4) {
                    gray_matrix[i][j] = (row_code << 2) | col_code;
                }
            }
        }
    }
}

// ������� ��� ���������� ����� �����
void fill_karnaugh_map() {
    // ������������� ���� ����� ����� ������
    for (unsigned char i = 0; i < MAX_SIZE; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            kmap[i][j] = 0;
			kmap_bool[i][j] = 0;
        }
    }

    // ������������� ������� � ������������ � �������� ones
    for (unsigned char k = 0; k < MAX_ONES; k++) {
        if (ones[k] == INVALID_VALUE) break; // ����������, ���� �������� ����� �������� ��������

        unsigned char target_index = ones[k];
        for (unsigned char i = 0; i < A; i++) {
            for (unsigned char j = 0; j < B; j++) {
                if (gray_matrix[i][j] == target_index) {
                    kmap[i][j] = 1; // ������������� �������
                    break;
                }
            }
        }
    }
}

// ������������� ������� �������� ������
void init_neighbors() {
    // ��������� ������� �������
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            neighbors[i][j] = 0;
        }
    }
}

// ������� ��� �������� ���������� ���������� ����� �� ���� ������� �� �����
uint8_t count_common_bits() {
     uint8_t common_bits_count = 0;

    // ������������� ������� values ������
    for (unsigned char i = 0; i < 4; i++) {
        values[i] = 0;
    }

    // ��������� ������ ��� (�������) � ������� ���� ����
    for (unsigned char bit = 0; bit < N; bit++) {
        // �������� �������� ������� �������� �� �����
        unsigned char first_value = gray_matrix[pt.items[0].row][pt.items[0].col];
        unsigned char first_bit = (first_value >> bit) & 1;

        _Bool all_match = true;

        // ��������� ���� ��� � ���� ��������� �����
        for (unsigned char i = 1; i < size(&pt); i++) {
            unsigned char current_value = gray_matrix[pt.items[i].row][pt.items[i].col];
            unsigned char current_bit = (current_value >> bit) & 1;

            if (current_bit != first_bit) {
                all_match = false;
                break;
            }
        }

        // ���� �� ������ ������� ���� ��������� � ����, ��������� values � �������
        if (all_match) {
            values[N - 1 - bit] = 1;
            common_bits_count++;
        }
    }

    return common_bits_count;
}

void generate_implicant_string(char result[9]) {
    // ������������� ������ ��� ������
    for (unsigned char i = 0; i < 8; i++) {
        result[i] = '0';
    }

	// �������� ������� ��� ���������
    for (unsigned char i = 0; i <= pt.top; i++) {
        unsigned char row = pt.items[i].row;
        unsigned char col = pt.items[i].col;
        kmap_bool[row][col] = true;
    }

    // �������� ��� ���� �� ������� �������� �����
    unsigned char gray_value = gray_matrix[pt.items[0].row][pt.items[0].col];

    // ������ ��� ������ � ������
    unsigned char index = 0;

    // ��������� ������ �� ������ values � gray_value
    for (unsigned char i = 0; i < N; i++) {
        if (values[i] == 1) {
            result[index++] = 'A' + i; // ��������� ����������
            if (((gray_value >> (N - 1 - i)) & 1) == 0) { // ��������� ��� ����������
                result[index++] = '\''; // ��������� ��������, ���� ��� == 0
            }
        }
    }

    // ��������� ������ �������� ����� ������
    result[index] = '\0';
}

// ������� ��� ������ ��������� (����)
_Bool minimize(unsigned char x, unsigned char y)
{
    IndexPair coord;              // ���������� �������
    _Bool added_flag = false;     // ����, ��� ������� ��� � �����
	_Bool twoOnes_flag = false;   // ���� ���������� ������� ���� ������

    while (1) {
        // ���������� �������� � ����
        if (!added_flag) {
            push(&pt, (IndexPair) { x, y });
        }

        added_flag = false;

        unsigned char bits;                             // ����� ���������� ���
        _Bool match = (size(&pt) == 1) ? true : false;  // ����, �����������, ��� ������� �������� ��� �������
        _Bool break_flag = false;                       // ���� ��������������� ��� ��������� �������
        _Bool next_flag = false;                        // ���� ��� ����������� ������
        unsigned char ind = x * 4 + y;                  // ������ ��� ������� �������

        bits = (size(&pt) == 1) ? N : count_common_bits();  // ��������� ���������� ����������� ���
        if (bits == 0)
            break_flag = true;         // ������� �� ��������
        else
            switch (size(&pt))
            {
            case 2:
            	if (twoOnes_flag) { // ��������� ������� ��� �������
                    match = true;      
                    break_flag = true;
                }
                if (N == 2) break_flag = true;  // ��� ���� ����������
                break;
            case 4:
                if (bits == 1 && N == 3)        // ��� ��� ����������
                {
                    match = true;
                    break_flag = true;
                }
                if (bits == 2 && N == 4)        // ��� ������ ����������
                    match = true;
                break;
            case 8:
                if (bits == 1)
                    match = true;
                break_flag = true;    // �����, ������ 8 ������ �� ����
                break;
            }

        if (!break_flag) // ����� ���������� �����
        {
			 _Bool priority_flag;	// ���� �������, �� � ���������� ���������� �������

            // �������� ��� ������� �����������: ������, ����, �����, �����
            unsigned char dx[4] = { 0, 1, 0, -1 };  // �������� �� �������
            unsigned char dy[4] = { 1, 0, -1, 0 };  // �������� �� ��������

            // �������� �� ����������� ����������� ������
	two_ones:
            priority_flag = true;                  // � ���������� ���������� �������
            for (unsigned char k = 0; k < 4; k++) {
                unsigned char nx = (x + dx[k] + A) % A;  // ����� ���������� �� ������
                unsigned char ny = (y + dy[k] + B) % B;  // ����� ���������� �� �������

                if ((kmap[nx][ny] == 1) && (!kmap_bool[nx][ny] + !priority_flag) && (!include(&pt, (IndexPair) { nx, ny })) && (neighbors[ind][k] == 0)) {

                    // ���������� �������
                    neighbors[ind][k] = 1;
                    x = nx;
                    y = ny;
                    next_flag = true; // ���������� ����� ��� �������� �������
                    break;
                }
                if (priority_flag && k == 3) { // ������� ������ ��� ����������
                    priority_flag = false;
                    k = -1;
                }
            }

            if (next_flag) continue;

			// ������� ��� ���� ������
            if ((size(&pt)) == 1 && !twoOnes_flag) {
                twoOnes_flag = true;
                // �������� ������������� �������
                for (unsigned char i = 0; i < 4; i++) {
                    neighbors[ind][i] = 0;
                }
                goto two_ones;
            }
        }

        // ������� ���������� ����������
        if (match) {
            return true; // �������� �����������   
        }

        // push(&invalid, pop(&pt));  // �������� ��� ������������ � ������� �������
        // �������� ������������� �������
        for (unsigned char i = 0; i < 4; i++) {
            neighbors[ind][i] = 0;
        }
        pop(&pt);                  // �������� ������������� ��������

        coord = peek(&pt);         // ������������ � ���������� �������
        x = coord.row;
        y = coord.col;
        added_flag = true;
    }
}

// ������� ��� �������� ����� � ������
void digit_to_string(char* digit, uint8_t num) {
    digit[0] = num + '0';
    digit[1] = '\0';
}

// -------------------------------------- ���������� ������ � �������� ------------------------------------------
// ���������� ���������� ��� ������ � ��������
uint8_t displayBuffer[8][64] = {{0}};  // ����� ��� ���������� ����� �����

// ����� ��� �������� (5x7 ��������)
const uint8_t customFont[][5] PROGMEM = {
    {0x00, 0x3E, 0x41, 0x41, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
	{0x00, 0x00, 0x7E, 0x00, 0x00}, // '1' ��� ���������
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x00, 0x06, 0x00, 0x00, 0x00}  // `'` (��������)
};

// ������� �������� � ������� �������
const char validChars[] = {'0', '1', '2', 'A', 'B', 'C', 'D', '\''}; // ��������� �������

// �������� �������
void KS0108_Command(uint8_t cmd, uint8_t chip) {
    DATA_PORT = cmd;             // ���������� �������
    CONTROL_PORT &= ~(1 << RS);  // RS = 0 (�������), ���. ��������� �����
    CONTROL_PORT &= ~(1 << RW);  // RW = 0 (������)

    // �������� ���
    if (chip == 1) {
        CONTROL_PORT |= (1 << CS1);
        CONTROL_PORT &= ~(1 << CS2);
    } else {
        CONTROL_PORT |= (1 << CS2);
        CONTROL_PORT &= ~(1 << CS1);
    }

    // ����� ��� ������ ������� �������
    CONTROL_PORT |= (1 << E);
    _delay_us(1);
    CONTROL_PORT &= ~(1 << E);

    // �������� ��� ���������� ������� ������������ �������
    _delay_ms(1);
}

// �������� ������
void KS0108_Data(uint8_t data, uint8_t chip) {
    DATA_PORT = data;            // ���������� ������
    CONTROL_PORT |= (1 << RS);   // RS = 1 (������)
    CONTROL_PORT &= ~(1 << RW);  // RW = 0 (������)

    // �������� ���
    if (chip == 1) {
        CONTROL_PORT |= (1 << CS1);
        CONTROL_PORT &= ~(1 << CS2);
    } else {
        CONTROL_PORT |= (1 << CS2);
        CONTROL_PORT &= ~(1 << CS1);
    }

    // �����
    CONTROL_PORT |= (1 << E);
    _delay_us(1);
    CONTROL_PORT &= ~(1 << E);

    // ��������
    _delay_ms(1);
}

// ������������� �������
void KS0108_Init() {
	// ���� D � C �� �����
    DATA_DDR = 0xFF;
    CONTROL_DDR |= (1 << RS) | (1 << RW) | (1 << E) | (1 << CS1) | (1 << CS2);

    KS0108_Command(0x3F, 1); // ��������� ������� ����
    KS0108_Command(0x3F, 2); // ��������� ������� ����
}

// ������� �������
void KS0108_Clear() {
    for (uint8_t page = 0; page < 8; page++) {
        KS0108_Command(0xB8 + page, 1); // ������������� �������� �� ������ ����
        KS0108_Command(0x40, 1);       // ����� ������� 0
        for (uint8_t col = 0; col < 64; col++) {
            KS0108_Data(0x00, 1);
        }
        KS0108_Command(0xB8 + page, 2); // ������������� �������� �� ������ ����
        KS0108_Command(0x40, 2);       // ����� ������� 0
        for (uint8_t col = 0; col < 64; col++) {
            KS0108_Data(0x00, 2);
        }
    }
}

// ��������� �������
void KS0108_SetCursor(uint8_t x, uint8_t line) {
    uint8_t chip = (x < 64) ? 1 : 2;
    uint8_t address = (x < 64) ? x : (x - 64);

    KS0108_Command(0xB8 + line, chip);
    KS0108_Command(0x40 + address, chip);
}

// ������� ��� ������ ������ �� �������
void RefreshDisplay() {
    for (uint8_t page = 0; page < 8; page++) {        // �������� �� ���� ���������
        KS0108_Command(0xB8 + page, 1);              // ������������� ��������
        KS0108_Command(0x40, 1);                     // ������������� ������� 0

        for (uint8_t column = 0; column < 64; column++) {
            KS0108_Data(displayBuffer[page][column], 1); // ���������� ������ �������
        }
    }
}

// ����� �������
void KS0108_WriteChar(char c, uint8_t x, uint8_t y) {
    // ���������, �������� �� ������ ����������
    uint8_t found = 0xFF;
    for (uint8_t i = 0; i < sizeof(validChars); i++) {
        if (c == validChars[i]) {
            found = i;
            break;
        }
    }

    KS0108_SetCursor(x, y); // ������������� ������

    // ������� ������ �� ������� customFont
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t line = pgm_read_byte(&customFont[found][i]);
        KS0108_Data(line, (x < 64) ? 1 : 2); // ���������� ������
    }

    KS0108_Data(0x00, (x < 64) ? 1 : 2); // ������ ����� ���������
}

// ����� ������ � �����������
void KS0108_WriteString(const char* str, uint8_t x, uint8_t y) {
    while (*str) {
        char c = *str++;
        if (c == '\'') {
            // ������� �������� ���� ������ ����������� �������
            KS0108_WriteChar(c, x - 1, y); // �������� ����� � �����
			x += 2;
        } else {
            // ������� ������� ������
            KS0108_WriteChar(c, x, y);
            x += 6; // �������� ������� ������ ��� ���������� �������
        }
    }
}

// ���������� ������� (�����) �� �������
void DrawPoint(uint8_t x, uint8_t y, _Bool clear) {
    uint8_t page = y / 8;       // ���������� ��������
    uint8_t bit = y % 8;        // ���������� ��� � �����

    if (clear) {
        displayBuffer[page][x] &= ~(1 << bit); // ���������� ��� � ������
    } else {
        displayBuffer[page][x] |= (1 << bit); // ������������� ��� � ������
    }
}

// ��������� �������������� �����
void DrawHorizontalLine(uint8_t x1, uint8_t x2, uint8_t y, _Bool clear) {
    for (uint8_t x = x1; x <= x2; x++) {
        DrawPoint(x, y, clear); // ������ ������ ����� �� �����
    }
}

// ��������� ������������ �����
void DrawVerticalLine(uint8_t x, uint8_t y1, uint8_t y2) {
    for (uint8_t y = y1; y <= y2; y++) {
        DrawPoint(x, y, false); // ������ ������ ����� �� �����
    }
}


// ������� ��� ������ �������
void DrawChar(char c, uint8_t x, uint8_t y) {
    if (c < '0' || c > '9') return; // �������������� ������ ����� '0'-'9'

    uint8_t basePage = y / 8;             // ������ ��������
    uint8_t bitOffset = y % 8;            // �������� ����� ��� ������ ��������

    for (uint8_t i = 0; i < 5; i++) {     // �������� �� ������� ������� �������
        uint8_t colData = pgm_read_byte(&customFont[c - '0'][i]);

        // ��������� ������ ��� ������ � ������� ��������
        uint8_t lowerData = colData << bitOffset;       // ������ ��������
        uint8_t upperData = colData >> (8 - bitOffset); // ������� ��������

        // ��������� ����� ��� ������ ��������
        if (basePage < 8) { // �������� �� ����� �� �������
            displayBuffer[basePage][x + i] |= lowerData;
        }

        // ��������� ����� ��� ������� ��������, ���� ������ ������� �� �������
        if (bitOffset > 0 && (basePage + 1) < 8) { // �������� �� ������������� ������� ��������
            displayBuffer[basePage + 1][x + i] |= upperData;
        }
    }
}

// ��������� ����� �����
void DrawKarno(uint8_t N) {
	uint8_t A = (N / 2) * 2;                     // ���������� ����� ������� ��� N ����������
	uint8_t B = (N - (N / 2)) * 2;               // ���������� �������� ������� ��� N ����������
	uint8_t end_line = min((B + 1) * 13, 63);    // ���������� ����� �������������� �����
	uint8_t end_column = min((A + 1) * 13, 63);  // ���������� ����� ������������ �����
	
	uint8_t code_gray[4] = { 0b00, 0b01, 0b11, 0b10 };

	// ������ ����� �����
	for (uint8_t line = 1; line <= A; line++) {
		DrawHorizontalLine(0, end_line, line * 13, false);
		// ���������� ���������� ��� ����� ��� 4 ����������
		if (N == 4) {
			DrawChar(((code_gray[line-1] >> 1) & 1) + '0', 0, line * 13 + 4);
	    	DrawChar((code_gray[line-1] & 1) + '0', 5, line * 13 + 4);
		} else {
		    DrawChar((line-1) + '0', 3, line * 13 + 4);
		}

	}
	 // ������ ����� ��������
	for (uint8_t column = 1; column <= B; column++) {
		DrawVerticalLine(column * 13, 0, end_column);
		// ���������� ���������� ��� �������� ��� 3 � 4 ����������
		if (N == 3 || N == 4) {
			DrawChar(((code_gray[column-1] >> 1) & 1) + '0', column * 13 + 2, 4);
		    DrawChar((code_gray[column-1] & 1) + '0', column * 13 + 7, 4);
		} else {
			DrawChar((column-1) + '0', column * 13 + 4, 4);
		}
	}
	// ������ ���������
	for (uint8_t i = 0; i < 13; i++) {
        DrawPoint(i, i, false);
	}

	// ���������� ����� ���������
	for (uint8_t row = 0; row < A; row++) {
		for (uint8_t col = 0; col < B; col++) {
			if (kmap[row][col] == 1) {
				uint8_t x = 17 + 13 * col;
				uint8_t y = 16 + 13 * row;
				DrawChar('2', x, y); // ��������� ������� �� �����
			}
		}
	}
}

// ��������� ��������� ������
void DrawImplicant() {
  	// �������� ��������� �������
	for (unsigned char i = 0; i <= pt.top; i++) {
        uint8_t row = pt.items[i].row; // ���������� ������
        uint8_t col = pt.items[i].col; // ���������� �������
    
	    uint8_t x1 = 16 + 13 * col;    // ������ �����
	    uint8_t x2 = 23 + 13 * col;    // ����� �����
	    uint8_t y = 24 + 13 * row;     // ���������� �� ������ �����
	    DrawHorizontalLine(x1, x2, y, false);  
    }
	
	RefreshDisplay();     // ��������� ������� �� ������

	// ������� ���������� ��������� �������
	for (unsigned char i = 0; i <= pt.top; i++) {
        uint8_t row = pt.items[i].row; // ���������� ������
        uint8_t col = pt.items[i].col; // ���������� �������
    
	    uint8_t x1 = 16 + 13 * col;    // ������ �����
	    uint8_t x2 = 23 + 13 * col;    // ����� �����
	    uint8_t y = 24 + 13 * row;     // ���������� �� ������ �����
	    DrawHorizontalLine(x1, x2, y, true);  
    }

}

// ������� ��� ������� ������
void ClearDisplayBuffer() {
    for (uint8_t page = 0; page < 8; page++) {
        for (uint8_t column = 0; column < 64; column++) {
            displayBuffer[page][column] = 0x00; // �������� ������ ���� ������
        }
    }
}

// -------------------------------------- ���������� ������ � �������� ------------------------------------------


// �������� ������� ������ STEP(PD4)
_Bool stepButtonPressed() {
    return !(PIND & (1 << PIND4)); // ������ ���������� � PIND4
}

void buttonInit() {
    DDRD &= ~(1 << PD4);  // ������������� PD4 ��� ����
    PORTD |= (1 << PD4);  // �������� ���������� ������������� ��������
}

volatile _Bool step_flag = false; // ����������-����

void setup_interrupts() {
    // ��������� INT0 � INT1 �� ������ �������
    MCUCR |= (1 << ISC01) | (1 << ISC11); // INT0 � INT1 ��������� �� ����
    GICR |= (1 << INT0) | (1 << INT1);   // ��������� ���������� INT0 � INT1

    // ��������� PD2 � PD3 ��� �����
    DDRD &= ~((1 << PD2) | (1 << PD3));  // ������������� PD2 � PD3 ��� �����
    PORTD |= (1 << PD2) | (1 << PD3);    // �������� �������� � Vcc
}

// ���������� �� ������� 0 �� PD2
ISR(INT0_vect) {
    step_flag = true;  // ������������� ����
	USARTTransmitStringLn("��������� �����: �������\r");
}

// ���������� �� ������� 0 �� PD3
ISR(INT1_vect) {
    step_flag = false; // ���������� ����
	USARTTransmitStringLn("��������� �����: ��������\r");
}

int main(void) {
	// ������������� UART
  	USARTInit(MYUBRR);
  	USARTTransmitStringLn("��� ��������� ����������� ��\r");

	KS0108_Init();       // ������������� �������
    KS0108_Clear();      // ������� �������


	buttonInit();        // ������������� ������ PD4
	setup_interrupts();  // ��������� ����������
    sei(); // ���������� ���������� ����������

	uint8_t strSize = 50;  // ����. ������ ������
	char str[strSize];     // ������ ��� ����������� ������
	char digit[2];		   // ����� ��� ������ ����

	while (1) {
		USARTTransmitStringLn("������� ������� ��� �����������\r");
    	USARTReceiveString(str, strSize); // ��������� ������ �� UART

		USARTTransmitString("��������� ������: ");
    	USARTTransmitStringLn(str);

		if (!parse_numbers(str)) {   // �������� ������������ ������
    		USARTTransmitStringLn("������ � ��������. ��������� ����!");
			USARTTransmitStringLn("");
			continue;
		}

		digit_to_string(digit, N);

		USARTTransmitString("������� ������� ����������� ������� �� ");
		USARTTransmitString(digit);
		USARTTransmitStringLn(" ����������");

	    // ������� ����� �����
	    A = (N / 2) * 2;         // ���������� ����� ������� ��� N ����������
	    B = (N - (N / 2)) * 2;   // ���������� �������� ������� ��� N ����������

	    _Bool f_const = true;

		generate_gray_code_matrix(); // ��������� � ����� ������� ���� ����
		fill_karnaugh_map();         // ���������� ����� �����

		// ������ � ��������
		KS0108_Clear();       // ������� �������
		ClearDisplayBuffer(); // ������� ������ �������
		DrawKarno(N);      	  // ����� ������� ��� ��������� ����� �����
		// DrawRectangle(15, 15, 24, 24);     // �������
		RefreshDisplay();     // ��������� ������� �� ������	

	    // �������� ������� �� ���������
	    for (uint8_t i = 0; i < A; i++) {
	        for (uint8_t j = 0; j < B; j++)
	            if (kmap[i][j] == 0) {
	                f_const = false;  // ������, f != C
	                break;
	            }
		}	

	    // �������� ��������
	    if (!f_const) {
	        initStack(&pt); 	         // ������������� �����
			init_neighbors();			 // ������������� ������� �������� ������
			uint8_t line = 0;			

	        char implicant[9]; // ������ ��� ����������	

	        for (uint8_t i = 0; i < A; i++) {
	            for (uint8_t j = 0; j < B; j++) {
	                if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
	                    // USARTTransmitStringLn("������� ������� �����������:\n");
	                    if (minimize(i, j)) {
							count_common_bits();
							generate_implicant_string(implicant);

							// ��������� ����� �� �������
							if (step_flag) {
								// ���, ���� ������ ����� ��������
	            				while (stepButtonPressed()) {
	                				// ��������� �������� ��� ���������� ��������
	                				_delay_ms(10);
	            				}
							
								// ���, ���� ������ ����� ��������
								while (!stepButtonPressed()) {
	                				// ��������� �������� ��� ���������� ��������
	                				_delay_ms(10);
	            				}
								_delay_ms(100); // �������� ����� �������

								
								// ��������� ���������� �� �����
								DrawImplicant();
							}

							KS0108_WriteString(implicant, 80, line++); // ����� ��������� �� �������

							// �������
							USARTTransmitString("\r��������");
							//digit_to_string(digit, ++num);
							//USARTTransmitString(digit);
							USARTTransmitString(" ���������� ");
							USARTTransmitStringLn(implicant);
							
							// ����� �����
							USARTTransmitString("Stack: ");
						    for (uint8_t i = 0; i <= pt.top; i++) {

								digit_to_string(digit, pt.items[i].row);
								USARTTransmitString(digit); // ���������� ������ �� UART

								digit_to_string(digit, pt.items[i].col);
								USARTTransmitString(digit); // ���������� ������ �� UART
								USARTTransmitString(" ");
							}	
							USARTTransmitString("\r");						

							init_neighbors();             // ������� ������� �������� ������
	                        clear(&pt);                   // ������� �����
	                    }
	                }
	            }
	        }
			USARTTransmitStringLn("\r����������� ���������\r");
	    }
	    else
	    {
	        USARTTransmitStringLn("\r�������� ������� f = 1\r"); // �������
	    }	
    }

	return 0;
}
