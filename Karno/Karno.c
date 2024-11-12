// Karno.c

#include "Stack.h"


#include <locale.h>

// ������� ������������ ����� (����� �������)
void TestStack();

unsigned char* ones;      // ���������� ���������� ������� ������
unsigned char  size_ones; // ���������� ���������� ������� ������� ������
unsigned char A, B;       // ����������� ����� �����

// ��������� ��� �������� ���� �������� ��� ����� �����
typedef struct {
    unsigned char** kmap;     // ����� �����
    bool** kmap_bool;         // ������ false
} KarnaughMaps;

// ������� ��� �������� ������� ������ ������� �������
unsigned char* parse_numbers(const char* input, unsigned char* size) {
    // ������������ ���������� ����� � ������
    *size = 0;
    for (unsigned char i = 0; input[i] != '\0'; i++) {
        if (input[i] == ' ') (*size)++;
    }
    (*size)++; // ���������� ����� = ���������� �������� + 1

    // ������� ������ ��� �����
    unsigned char* numbers = (unsigned char*)malloc(*size * sizeof(unsigned char));
    if (!numbers) {
        printf("������ ��������� ������\n");
        return NULL;
    }

    // ��������� ������ � ��������� ������ �����
    unsigned char index = 0;
    char* endptr;
    for (const char* token = input; *token != '\0'; token = endptr) {
        numbers[index++] = strtol(token, &endptr, 10);
        if (*endptr == ' ') endptr++;
    }

    return numbers;
}

// ������� ��� �������� ���������� �������
KarnaughMaps create_karnaugh_map(int N) {
    // �������� ������ ��� ���������� �������
    unsigned char** map = (unsigned char**)malloc(A * sizeof(unsigned char*));
    for (int i = 0; i < A; i++) {
        map[i] = (unsigned char*)malloc(B * sizeof(unsigned char));
    }

    // �������� ������ ��� ������� false
    bool** map_false = (bool**)malloc(A * sizeof(bool*));
    for (int i = 0; i < A; i++) {
        map_false[i] = (bool*)malloc(B * sizeof(bool));
    }

    // ��������� ������� ������ � false
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            map[i][j] = 0;
            map_false[i][j] = false;
        }
    }

    // ������������� ������� �� �������� �� ������� ones
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

//---------------------------------------- ���������� ������� ----------------------------------------
void printOnes(unsigned char* ones, unsigned char size) // ����� ones
{
    printf("������ �� ������: ");
    for (unsigned char i = 0; i < size; i++) {
        printf("%d ", ones[i]);
    }
    printf("\n");
}

// ������� ��� ������ ����� ����� (��� �������)
void print_karnaugh_map(unsigned char** map, unsigned char A, int B) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
}

// ������� ��� ������ ���������� ����� ����� (��� �������)
void print_karnaugh_map_bool(unsigned char** map, unsigned char A, int B) {
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            printf("%5s ", map[i][j] ? "true" : "false");
        }
        printf("\n");
    }
}


unsigned char code[4] = { 0b00, 0b01, 0b11, 0b10 }; // ������������������ �������� ��� ����� �����

int main()
{
    setlocale(LC_ALL, "Rus");

    // �������� ������������� ������
    const char* str = "1 3 5 7 9 15";                     
    unsigned char N = 4;                      // ���������� ����������

    printf("�������� ������: %s\n", str);       // ���������� ����� ������

    ones = parse_numbers(str, &size_ones);    // �������� ������ ����� � ��� ������
    printOnes(ones, size_ones);               // ���������� �����

    A = (N / 2) * 2;                          // ���������� ������ ������� ��� N ����������
    B = (N - (N / 2)) * 2;                    // ���������� �������� ������� ��� N ����������

    KarnaughMaps maps = create_karnaugh_map(N);

    if (maps.kmap != NULL) {
        printf("����� ����� (� ���� �������� �������):\n");
        print_karnaugh_map(maps.kmap, A, B);
    }    
    
    if (maps.kmap_bool != NULL) {
        printf("����� ����� (� ���� �������� ���������� �������):\n");
        print_karnaugh_map_bool(maps.kmap, A, B);
    }



    // ������������ ������
    for (int i = 0; i < 4; i++) {
        free(maps.kmap[i]);
        free(maps.kmap_bool[i]);
    }
    free(maps.kmap);
    free(maps.kmap_bool);
    
    if (ones) free(ones) ; // ����������� ������

    // TestStack();

    return 0;
}

// ������� ������������ ����� (����� �������)
void TestStack()
{
    // ������� stack �������� 5
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