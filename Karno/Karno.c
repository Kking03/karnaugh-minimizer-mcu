// Karno.c

#include "Stack.h"


#include <locale.h>

// ������� ������������ ����� (����� �������)
void TestStack();

unsigned char* ones;      // ���������� ���������� ������� ������
unsigned char  size_ones; // ���������� ���������� ������� ������� ������
unsigned char  A, B;      // ����������� ����� �����

// ��������� ��� �������� ���� �������� ��� ����� �����
typedef struct {
    unsigned char** kmap;     // ����� �����
    bool** kmap_bool;         // ������ false
} KarnaughMaps;

// ��������� ��� ���� ������
typedef struct ListNode {
    int stackSize;           // ������ ����� �� ������ �������� ����
    IndexPair* stackItems;   // ����� ��������� �����
    struct ListNode* next;   // ��������� �� ��������� ������� ������
} ListNode;

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

// �������� ���������� ������� � ����� ����
char** generate_gray_code(int N, int* rows, int* cols) {
    // ���������� ������ ������� ��� ������� N
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
        printf("���������������� �������� N\n");
        return NULL;
    }

    // ��������� ������������������ ���� ���� ��� N ���
    int total_codes = 1 << N; // 2^N ��������� ��������
    char** gray_code = (char**)malloc(total_codes * sizeof(char*));

    for (int i = 0; i < total_codes; i++) {
        gray_code[i] = (char*)malloc((N + 1) * sizeof(char)); // +1 ��� ������������ '\0'
        int gray_value = i ^ (i >> 1); // ���������� �������� � ���� ����
        for (int j = N - 1; j >= 0; j--) {
            gray_code[i][N - 1 - j] = (gray_value & (1 << j)) ? '1' : '0';
        }
        gray_code[i][N] = '\0'; // ��������� ������
    }

    // ��������� ������� �� �������������������
    char** gray_matrix = (char**)malloc(*rows * sizeof(char*));
    for (int i = 0; i < *rows; i++) {
        gray_matrix[i] = (char*)malloc((*cols) * (N + 1) * sizeof(char)); // �������� ������ ��� ������ ������
        for (int j = 0; j < *cols; j++) {
            // �������� ������� �������, ������ strcpy
            for (int k = 0; k < N + 1; k++) {
                gray_matrix[i][j * (N + 1) + k] = gray_code[i * (*cols) + j][k];
            }
        }
    }

    // ������������ ������ ��� ���������� ������� gray_code
    for (int i = 0; i < total_codes; i++) {
        free(gray_code[i]);
    }
    free(gray_code);

    return gray_matrix;
}

// ������� ��� �������� ������ �������� ������
ListNode* createListNode(struct stack* pt) {
    if (isEmpty(pt)) {
        return NULL;  // ���� ���� ������, ������ �� ������
    }

    // ������� ����� ������� ������
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (!newNode) {
        printf("������ ��������� ������\n");
        exit(EXIT_FAILURE);
    }

    // ������������� ������ �����
    newNode->stackSize = size(pt);

    // �������� ������ ��� ������ � ���������� �����
    newNode->stackItems = (IndexPair*)malloc(newNode->stackSize * sizeof(IndexPair));
    if (!newNode->stackItems) {
        printf("������ ��������� ������\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    // �������� �������� �� ����� � ������ stackItems
    for (int i = 0; i < newNode->stackSize; i++) {
        newNode->stackItems[i] = pt->items[i];
    }

    newNode->next = NULL;  // ���������� ��������� ������� �����������
    return newNode;
}

// ������� ��� ���������� ������ ���� � ������ ������
void addNodeToList(ListNode** head, struct stack* pt) {
    ListNode* newNode = createListNode(pt);
    if (newNode) {
        newNode->next = *head;
        *head = newNode;
    }
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

// ������� ��� ������ ������� ���� ����
void print_gray_matrix(char** gray_matrix, int rows, int cols, int N) {
    printf("������� ���� ���� ��� %d ����������:\n", N);
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

// ���������� ������� ��� ������ ����������� ������
void printList(ListNode* head) {
    ListNode* current = head;
    int nodeIndex = 1;
    while (current != NULL) {
        printf("������� %d:\n", nodeIndex++);
        printf("  ������ �����: %d\n", current->stackSize);
        printf("  �������� �����: ");
        for (int i = 0; i < current->stackSize; i++) {
            printf("(%d, %d) ", current->stackItems[i].row, current->stackItems[i].col);
        }
        printf("\n");
        current = current->next;
    }
    if (nodeIndex == 1) {
        printf("������ ����\n");
    }
}

// ������� ������������ ������ (����� �������)
void TestList();


unsigned char code[4] = { 0b00, 0b01, 0b11, 0b10 }; // ������������������ �������� ��� ����� �����

int main()
{
    setlocale(LC_ALL, "Rus");

    // �������� ������������� ������
    const char* str = "1 3 5 7 9";                     
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

    int rows, cols;

    // ��������� � ����� ������� ���� ����
    char** gray_matrix = generate_gray_code(N, &rows, &cols);
    if (gray_matrix) {
        print_gray_matrix(gray_matrix, rows, cols, N);

        // ������������ ������
        for (int i = 0; i < rows; i++) {
            free(gray_matrix[i]);
        }
        free(gray_matrix);
    }

    // ������������ ������
    for (int i = 0; i < A; i++) {
        free(maps.kmap[i]);
        free(maps.kmap_bool[i]);
    }
    free(maps.kmap);
    free(maps.kmap_bool);
    
    if (ones) free(ones) ; // ����������� ������

    TestStack();
    TestList();

    return 0;
}

// ������� ������������ ����� (����� �������)
void TestStack()
{
    struct stack* pt = newStack(5);

    // ������ ���������� ��������� � ����
    push(pt, (IndexPair) { 1, 2 });
    push(pt, (IndexPair) { 3, 4 });
    push(pt, (IndexPair) { 5, 6 });

    // ��������, �������� �� ���� ������������ �������
    bool res = include(pt, (IndexPair) { 3, 4 });
    printf("%s\n", res ? "������" : "�� ������");

    // ������ �������� ��������
    IndexPair top = peek(pt);
    printf("The top element is (%d, %d)\n", top.row, top.col);
    printf("The stack size is %d\n", size(pt));

    // �������� ��������� �� �����
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

    // ������������ ������
    free(pt->items);
    free(pt);

}

void TestList()
{
    struct stack* pt = newStack(5);

    // ��������� �������� � ����
    push(pt, (IndexPair) { 1, 2 });
    push(pt, (IndexPair) { 3, 4 });
    push(pt, (IndexPair) { 5, 6 });

    // ��������� �� ������ ������
    ListNode* head = NULL;

    // ������ ������� ������ �� ������ �������� ��������� �����
    addNodeToList(&head, pt);
    pop(pt);
    addNodeToList(&head, pt);

    // ����� ������
    printList(head);

    // ������������ ������ (������)
    ListNode* current = head;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        free(temp->stackItems);
        free(temp);
    }

    // ������������ ������ �����
    free(pt->items);
    free(pt);

    return 0;
}
