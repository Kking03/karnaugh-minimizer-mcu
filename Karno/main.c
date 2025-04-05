// �������
#define MAX_ONES 16                              // ������������ ���������� ������ �������
#define MAX_SIZE 4                               // ������������ ������ ����� �����
#define INVALID_VALUE 255                        // ���� ��� �������� ���������
#define isdigit(c) ((c) >= '0' && (c) <= '9')    // �������� ������� �� ������������ �����

#include <locale.h>  // �������
#include "Stack.h"

unsigned char ones[MAX_ONES];                          // ���������� ������ ��� �������� ������� ������
unsigned char N;                                       // ���������� ����������
unsigned char neighbors[MAX_ONES][MAX_SIZE];           // ���������� ���������� ������� �������� ������
unsigned char A, B;                                    // ����������� ����� �����
unsigned char gray_matrix[MAX_SIZE][MAX_SIZE];         // ���������� ������� ����� ����
unsigned char kmap[MAX_SIZE][MAX_SIZE];                // ���������� ������� ��� ����� �����
unsigned char kmap_bool[MAX_SIZE][MAX_SIZE] = { {0} }; // ���������� ���������� ������� ��� ����� �����
unsigned char values[4];                               // ���������� ������ ��� �������� ����������� ���
Stack pt;                                              // ���������� ���������� ��������� �����

// ������� ��� �������� ������� ������ ������� �������
_Bool parse_numbers(char input[]) {
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
    if (N < 2 || N > 4) {
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

        _Bool all_match = true;

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

void generate_implicant_string(char result[9]) {
    // ������������� ������ ��� ������
    for (unsigned char i = 0; i < 8; i++) {
        result[i] = '\0';
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

/*
// ������� ��� ������ ���������
_Bool minimize(unsigned char x, unsigned char y)
{
    // ���������� �������� � ����
    push(&pt, (IndexPair) { x, y });

    int bits;                                     // ����� ���������� ���
    _Bool match = (size(&pt) == 1) ? true : false;  // ����, �����������, ��� ������� �������� ��� �������
    _Bool break_flag = false;                      // ���� ��������������� ��� ��������� �������

    bits = (size(&pt) == 1) ? N : count_common_bits();  // ��������� ���������� ����������� ���
    if (bits == 0)
        break_flag = true;         // ������� �� ��������
    else
        switch (size(&pt))
        {
        case 2: // ��������� match ��� 2 ����???
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

        // ������������ ������� �������� ��������� ��� ���������
        unsigned char l_ind = 0;
        unsigned char r_ind = 3;
        unsigned char coord[4][2];

        for (int k = 0; k < 4; k++) {
            int nx = (x + dx[k] + A) % A;  // ����� ���������� �� ������
            int ny = (y + dy[k] + B) % B;  // ����� ���������� �� �������

            if (kmap_bool[nx][ny] == false) { // ������������ ������������ �������
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

        // �������� �� ������� �����������
        for (int k = 0; k < 4; k++) {
            unsigned char nx = coord[k][0];
            unsigned char ny = coord[k][1];
            if ((kmap[nx][ny] == 1) && (!include(&pt, (IndexPair) { nx, ny }))) {
                _Bool res = minimize(nx, ny);
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
*/

/*

*/
// ���� 
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

        int bits;                                       // ����� ���������� ���
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
                if (twoOnes_flag) {
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
            _Bool priority_flag;
            // �������� ��� ������� �����������: ������, ����, �����, �����
            unsigned char dx[4] = { 0, 1, 0, -1 };  // �������� �� �������
            unsigned char dy[4] = { 1, 0, -1, 0 };  // �������� �� ��������

            // �������� �� ����������� ����������� ������
    two_ones:
            priority_flag = true;                  // ���� �������, �� � ���������� ���������� �������
            for (int k = 0; k < 4; k++) {
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
            return true;      // �������� �����������   
        }

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

void init_neighbors() {
    // ��������� ������� �������
    for (unsigned char i = 0; i < MAX_ONES; i++) {
        for (unsigned char j = 0; j < MAX_SIZE; j++) {
            neighbors[i][j] = 0;
        }
    }
}

/*
// ������� ��� �������� ������������ ������� neighbors
IndexPair** create_neighbors() {
    // ��������� ������ ��� ������� ���������� �� ������
    IndexPair** neighbors = (IndexPair**)malloc(n_ones * sizeof(IndexPair*));

    // ��������� ������ ��� ������ ������ (�� 5 ���������)
    for (unsigned char i = 0; i < n_ones; i++) {
        neighbors[i] = (IndexPair*)malloc(5 * sizeof(IndexPair));
    }

    
    // ���������� �������
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

// ������� ��� ������������ ������, ���������� ��� neighbors
void free_neighbors() {
    for (unsigned char i = 0; i < n_ones; i++) {
        free(neighbors[i]); // ����������� ������ ������
    }
    free(neighbors); // ����������� ������ ����������
}
*/

//---------------------------------------- ���������� ������� ----------------------------------------
void TestStack();                                                          // ������� ������������ �����
void printOnes();                                                          // ������� ��� ������ ������� ������
void print_karnaugh_map();                                                 // ������� ��� ������ ����� �����
void print_karnaugh_map_bool(unsigned char A, unsigned char  B);           // ������� ��� ������ ���������� ����� �����
void print_gray_matrix();                                                  // ������� ��� ������ ������� ���� ����
void TestBitCountFunc();                                                   // ������� ������������ �������� ���������� ���

int main()
{
    setlocale(LC_ALL, "Rus");

    // �������� ������������� ������
    char str[10][40] = {
    "4 0 2 8 10 1 5",
    "4 3 7 11 15", // 3 �������
    "4 2 6 10 14", // 4 �������
    "4 1 5 9 13",  // 2 �������
    "4 0 4 8 12"   // 1 �������
    };


    for (int i = 0; i < 1; i++)
    {

        printf("\n�������� ������: %s\n", str[i]);

        if (!parse_numbers(str[i])) {   // �������� ������������ ������
            // �������
            printf("������ � ������:\n");

            continue;
        }

        // ������ ������
        printOnes();

        // ������� ����� �����
        A = (N / 2) * 2;         // ���������� ����� ������� ��� N ����������
        B = (N - (N / 2)) * 2;   // ���������� �������� ������� ��� N ����������

        generate_gray_code_matrix();  // ��������� � ����� ������� ���� ����

        // ���������� ����� �����
        fill_karnaugh_map();

        _Bool f_const = true;

        // �������� �� �������-���������
        for (int i = 0; i < A; i++)
            for (int j = 0; j < B; j++)
                if (kmap[i][j] == 0) {
                    f_const = false;  // ������, f != C
                    break;
                }

        // �������
        print_gray_matrix();
        print_karnaugh_map();

        // �������� ��������
        if (!f_const) {
            initStack(&pt); // ������������� �����
            init_neighbors();

            char implicant[9]; // ������ ��� ����������


            int num = 1; // �������

            // �������� ��������
            for (int i = 0; i < A; i++) {
                for (int j = 0; j < B; j++) {
                    if ((kmap[i][j] == 1) && (!kmap_bool[i][j])) {
                        printf("������� ������� �����������:\n");
                        if (minimize(i, j)) {

                            count_common_bits();
                            generate_implicant_string(implicant);


                            // �������
                            printf("�������� ���������� %d: %s\n", num++, implicant);;
                            // ���������� ����� �����
                            printf("Stack: \n");
                            for (unsigned char i = 0; i <= pt.top; i++) {
                                printf("%d", pt.items[i].row);
                                printf("%d\n", pt.items[i].col);
                            }
                            printf("������ ����������: ");
                            for (unsigned char i = 0; i < N; i++) {
                                printf("%d ", values[i]);
                            }
                            printf("\n\n");

                            init_neighbors();             // ������� ������� �������� ������
                            clear(&pt);                   // ������� �����
                        }
                    }
                }
            }
        }
        else
        {
            printf("�������� ������� f = 1:\n");
        }

        // �������
        if (kmap_bool != NULL) {
            printf("����� ����� (� ���� �������� ���������� �������):\n");
            print_karnaugh_map_bool(A, B);
        }
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
void print_gray_matrix() {
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
    push(&sp, (IndexPair) { 1, 0 });
    push(&sp, (IndexPair) { 2, 0 });
    push(&sp, (IndexPair) { 3, 0 });

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
    pop(&sp);
    pop(&sp);

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
    _Bool res = include(&pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "������" : "�� ������");

    printf("The stack size is %d\n", size(&pt));

    // �������� ��������� �� �����
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

printf("\n-----������������ �������� ���-----\n");

// �������� ������� ��������
printf("���������� ���: %d\n", count_common_bits());
printf("������ ����������: ");
for (unsigned char i = 0; i < 4; i++) {
    printf("%d ", values[i]);
}
printf("\n");
*/
// ����

// �������� ����� ���������

            /*
            if (N == 4) {
                _Bool full_line;
                unsigned char find[4] = { {0} };

                // �� �������
            printf("-----------------------����� �� �������\n");
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

            // �������� ����������
            for (unsigned char i = 0; i < 4; i++) {
                if (find[i] == 1 && find[(i + 1) % 4] != 1 && find[(i + 3) % 4] != 1) {
                    for (unsigned char j = 0; j < 4; j++) {
                        kmap_bool[i][j] = true;
                        push(&pt, (IndexPair) { i, j });
                    }

                    count_common_bits();
                    generate_implicant_string(implicant);

                    // �������
                    printf("�������� ���������� %d: %s\n", num++, implicant);;
                    // ���������� ����� �����
                    printf("Stack: \n");
                    for (unsigned char i = 0; i <= pt.top; i++) {
                        printf("%d", pt.items[i].row);
                        printf("%d\n", pt.items[i].col);
                    }
                    printf("������ ����������: ");
                    for (unsigned char i = 0; i < N; i++) {
                        printf("%d ", values[i]);
                    }
                    printf("\n");

                    clear(&pt);
                }
            }
            // �������
            printf("������ ���������� �����: ");
            for (int i = 0; i < 4; i++) {
                printf("%d ", find[i]);
                find[i] = 0;
            }

            // �� ��������

                printf("\n\n-----------------------����� �� ��������\n");
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

                // �������� ����������
                for (unsigned char col = 0; col < 4; col++) {
                    if (find[col] == 1 && find[(col + 1) % 4] != 1 && find[(col + 3) % 4] != 1) {
                        for (unsigned char row = 0; row < 4; row++) {
                            kmap_bool[row][col] = true;
                            push(&pt, (IndexPair) { row, col });
                        }

                        count_common_bits();
                        generate_implicant_string(implicant);

                        // �������
                        printf("�������� ���������� %d: %s\n", num++, implicant);;
                        // ���������� ����� �����
                        printf("Stack: \n");
                        for (unsigned char i = 0; i <= pt.top; i++) {
                            printf("%d", pt.items[i].row);
                            printf("%d\n", pt.items[i].col);
                        }
                        printf("������ ����������: ");
                        for (unsigned char i = 0; i < N; i++) {
                            printf("%d ", values[i]);
                        }
                        printf("\n");

                        clear(&pt);
                    }
                }
                printf("������ ���������� ��������: ");
                for (int i = 0; i < 4; i++) {
                    printf("%d ", find[i]);
                }

                printf("\n\n-----------------------����� �� ��������\n\n");
            }
            */