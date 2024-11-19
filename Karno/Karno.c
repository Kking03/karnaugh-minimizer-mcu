// �������
/*
1 ����������� ��� f = Const
2 ���������� ��������� � ������
*/

// Karno.c

// �������
#define MAX_ONES 16                              // ������������ ���������� ������ �������
#define MAX_SIZE 4                               // ������������ ������ ����� �����
#define INVALID_VALUE 255                        // 
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // �������� ������� �� ������������ �����

#include <locale.h>

#include "Stack.h"

unsigned char ones[MAX_ONES];                   // ���������� ������ ��� �������� ������� ������
unsigned char N;                                // ���������� ����������
unsigned char A, B;                             // ����������� ����� �����
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];  // ���������� ������� ����� ����
unsigned char kmap[MAX_SIZE][MAX_SIZE];         // ���������� ������� ��� ����� �����
bool kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} };   // ���������� ���������� ������� ��� ����� �����
unsigned char values[4];                        // ���������� ������ ��� �������� ����������� ���
Stack pt;                                      // ���������� ���������� ��������� �����


// ������ ��� ��������� ���� ����
const unsigned char code_gray[] = { 0b00, 0b01, 0b11, 0b10 };

// ������� ��� �������� ������� ������ ������� �������
bool parse_numbers(char input[]) {
    unsigned char index = 0; // ������ ��� ������� 'ones'

    // ��������� ������ ones �������
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        ones[i] = INVALID_VALUE;
    }

    // ������� �������� � ������ ������
    while (*input == ' ') input++;

    // �������� �� �����
    if (!isdigit(*input)) {
        return false;       // ������������ ���������� ����������
    }
    N = *input - '0';       // �������������� ������� � �����
    if (N < 1 || N > 4) {
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
            printf("������: �������� %d ������� �� ������� ����������� ��������� (0-%d)\n", value, max_position);
            return false;
        }

        // ��������� �� ������������
        for (unsigned char i = 0; i < index; i++) {
            if (ones[i] == value) {
                printf("������: ������������� �������� %d\n", value);
                return false;
            }
        }

        // ��������� �������� � ������ 'ones'
        if (index >= MAX_ONES) {
            printf("������: ������� ����� ������ (�������� %d)\n", MAX_ONES);
            return false;
        }
        ones[index++] = value;

        // ���������� �������
        while (*input == ' ') input++;
    }

    return true;
}

// ������� ��� �������� ��������� ������� ���� ���� ��� ����� �����
void generate_gray_code_matrix(unsigned char N) {
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

// ������� ��� �������� ���������� ���������� ����� �� ���� ������� �� �����
int count_common_bits() {
    int common_bits_count = 0;

    // ������������� ������� values ������
    for (unsigned char i = 0; i < 4; i++) {
        values[i] = 0;
    }

    // ��������� ������ ��� (�������) � ������� ���� ����
    for (unsigned char bit = 0; bit < N; bit++) {
        // �������� �������� ������� �������� �� �����
        unsigned char first_value = gray_matrix[pt.items[0].row][pt.items[0].col];
        unsigned char first_bit = (first_value >> bit) & 1;

        bool all_match = true;

        // ��������� ���� ��� � ���� ��������� �����
        for (int i = 1; i < size(&pt); i++) {
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

// ������� ��� ������ ���������
bool minimize(unsigned char x, unsigned char y)
{
    // ���������� �������� � ����
    push(&pt, (IndexPair) { x, y });

    int bits;                                     // ����� ���������� ���
    bool match = (size(&pt) == 1) ? true : false;  // ����, �����������, ��� ������� �������� ��� �������
    bool break_flag = false;                      // ���� ��������������� ��� ��������� �������

    bits = (size(&pt) == 1) ? N : count_common_bits();  // ��������� ���������� ����������� ���
    if (bits == 0)
        break_flag = true;         // ������� �� ��������
    else
        switch (size(&pt))
        {
        case 2:
            match = true;
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
        // �������� ��� ������� �����������: ������, ����, �����, �����
        int dx[4] = { 0, 1, 0, -1 };  // �������� �� �������
        int dy[4] = { 1, 0, -1, 0 };  // �������� �� ��������

        // �������� �� ������� �����������
        for (int k = 0; k < 4; k++) {
            int nx = (x + dx[k] + A) % A;  // ����� ���������� �� ������
            int ny = (y + dy[k] + B) % B;  // ����� ���������� �� �������

            if ((kmap[nx][ny] == 1) && (!include(&pt, (IndexPair) { nx, ny }))) {
                bool res = minimize(nx, ny);
                if (res)  // ������� ���������� ����������
                {
                    match = true;    // ������� �������� ��� ����������                  
                    break;           // ��������� �����
                }
            }

        }
    }

    if (match) {
        kmap_bool[x][y] = true; // ������� ����������� ���������� ���������
        return true; // �������� �����������   
    }

    pop(&pt);         // ������� �������
    return false;    // ������ �����������
}

//---------------------------------------- ���������� ������� ----------------------------------------
void TestStack();                                                          // ������� ������������ �����
void printOnes();                                                          // ������� ��� ������ ������� ������
void print_karnaugh_map();                                                 // ������� ��� ������ ����� �����
void print_karnaugh_map_bool(unsigned char A, unsigned char  B);           // ������� ��� ������ ���������� ����� �����
void print_gray_matrix(unsigned char A, unsigned char B);                  // ������� ��� ������ ������� ���� ����
void TestBitCountFunc();                                                   // ������� ������������ �������� ���������� ���

int main()
{
    setlocale(LC_ALL, "Rus");

    // �������� ������������� ������
    char str[] = " 2 0  1 2  ";

    if (parse_numbers(str)) {
        printf("���������� ����������: %d\n", N);
        printOnes();
    }
    else {
        printf("������ � ������� ������\n");
    }

    // ������� ����� �����
    A = (N / 2) * 2;         // ���������� ����� ������� ��� N ����������
    B = (N - (N / 2)) * 2;   // ���������� �������� ������� ��� N ����������

    generate_gray_code_matrix(N);  // ��������� � ����� ������� ���� ����
    print_gray_matrix(A, B);       // ���������� �����

    // ���������� ����� �����
    fill_karnaugh_map();
    // ������ ����� �����
    print_karnaugh_map();


    // TestBitCountFunc(); // ������������ ������� ����ר�� ���������� ���

    // �������� ��������
    initStack(&pt); // ������������� �����

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
                printf("������� ������� �����������:\n");
                if (minimize(i, j)) {
                    clear(&pt);                   // ������� �����
                    printf("������ ����������: ");
                    for (unsigned char i = 0; i < N; i++) {
                        printf("%d ", values[i]);
                    }
                    printf("\n");
                }
            }
        }
    }

    // ������ ����� ����� � ����������� ���������
    if (kmap_bool != NULL) {
        printf("����� ����� (� ���� �������� ���������� �������):\n");
        print_karnaugh_map_bool(A, B);
    }

    return 0;
}

//------------------------------------------����������------------------------------------------
// ������� ��� ������ ������� ������
void printOnes()
{
    printf("������ ������: ");
    for (int i = 0; i < MAX_ONES; i++) {
        if (ones[i] != INVALID_VALUE) {
            printf("%d ", ones[i]);
        }
    }
    printf("\n");
}

// ������� ��� ������ ������� ���� ����
void print_gray_matrix(unsigned char A, unsigned char B) {
    printf("������� ���� ����:\n");
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            // ����������� �������� � �������� ��� �������
            for (int bit = N - 1; bit >= 0; bit--) {
                putchar((gray_matrix[i][j] & (1 << bit)) ? '1' : '0');
            }
            printf(" "); // ��������� �������� ��������
        }
        printf("\n");
    }
}


// ������� ��� ������ ����� �����
void print_karnaugh_map() {
    printf("����� �����:\n");
    for (unsigned char i = 0; i < A; i++) {
        for (unsigned char j = 0; j < B; j++) {
            printf("%d ", kmap[i][j]);
        }
        printf("\n");
    }
}

// ������� ��� ������ ���������� ����� ����� (��� �������)
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
    printf("\n-----������������ �������� ���-----\n");

    Stack sp;

    initStack(&sp);

    // ���������� ��������� � ����
    push(&sp, (IndexPair) { 0, 0 });
    push(&sp, (IndexPair) { 0, 1 });
    //push(pt, (IndexPair) { 1, 0 });
    //push(pt, (IndexPair) { 1, 1 });

    // �������� ������� ��������
    printf("���������� ���: %d\n", count_common_bits());
    printf("������ ����������: ");
    for (unsigned char i = 0; i < 4; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    // �������� ��������� �� �����
    pop(&sp);
    pop(&sp);
    //pop(pt);
    //pop(pt);

    // ������������ ������
}

// ������� ������������ ����� (����� �������)
void TestStack()
{
    printf("-----������������ �����-----\n");
    Stack sp;

    initStack(&sp);

    // ������ ���������� ��������� � ����
    push(&sp, (IndexPair) { 1, 2 });
    push(&sp, (IndexPair) { 3, 4 });
    push(&sp, (IndexPair) { 5, 6 });

    // ��������, �������� �� ���� ������������ �������
    bool res = include(&pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "������" : "�� ������");

    printf("The stack size is %d\n", size(&pt));

    // �������� ��������� �� �����
    pop(&pt);
    pop(&pt);
    pop(&pt);
}

