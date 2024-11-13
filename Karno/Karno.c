// Karno.c
#include <locale.h>

#include "Stack.h"

unsigned char* ones;      // ���������� ���������� ������� ������
unsigned char  size_ones; // ���������� ���������� ������� ������� ������
unsigned char  A, B;      // ����������� ����� �����
char*** gray_matrix;      // ������� ����� ����
unsigned char N;          // ���������� ����������

const char* code_gray[] = { "00", "01", "11", "10" }; // ��������������� ��� ���� ��� 2 ���

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
        int target_index = ones[i];

        // ����� ������� (row, col) ��� target_index � gray_matrix
        bool found = false;
        for (int row = 0; row < A && !found; row++) {
            for (int col = 0; col < B && !found; col++) {
                // ����������� ������ ���� ���� � ����� ��� ��������� � target_index
                int gray_value = (int)strtol(gray_matrix[row][col], NULL, 2);

                if (gray_value == target_index) {
                    map[row][col] = 1;
                    found = true;
                }
            }
        }
    }

    KarnaughMaps maps = { map, map_false };
    return maps;
}

// ������� ��� �������� ��������� ������� ���� ���� ��� ����� �����
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
        printf("���������������� �������� N\n");
        return NULL;
    }

    // ������ ������� ��� �������� ���������� ���� ����
    char*** gray_matrix = (char***)malloc(*rows * sizeof(char**));
    for (int i = 0; i < *rows; i++) {
        gray_matrix[i] = (char**)malloc(*cols * sizeof(char*));
        for (int j = 0; j < *cols; j++) {
            gray_matrix[i][j] = (char*)malloc((N + 1) * sizeof(char)); // N ��� + '\0'

            // ������� �������� ������� �� code_gray[i] � code_gray[j]
            gray_matrix[i][j][0] = code_gray[i][0];
            gray_matrix[i][j][1] = code_gray[i][1];
            gray_matrix[i][j][2] = code_gray[j][0];
            gray_matrix[i][j][3] = code_gray[j][1];
            gray_matrix[i][j][4] = '\0'; // ��������� ������
        }
    }

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

// ������� ��� �������� ���������� ���������� ����� �� ���� ������� �� �����
int count_common_bits(struct stack* pt) {
    if (isEmpty(pt)) {
        return 0; // ���� ���� ������, ���������� 0
    }

    int common_bits_count = 0;
    int bit_length = strlen(gray_matrix[0][0]); // ����� ������ ���� ���� (N ���)

    // ��������� ������ ��� (�������) � �������
    for (int bit = 0; bit < bit_length; bit++) {
        // �������� ��� �� ������ ������ � �����
        char first_bit = gray_matrix[pt->items[0].row][pt->items[0].col][bit];
        bool all_match = true;

        // ���������� ��� � ������������ ������ �� ���� ������� �� �����
        for (int i = 1; i < size(pt); i++) {
            if (gray_matrix[pt->items[i].row][pt->items[i].col][bit] != first_bit) {
                all_match = false;
                break;
            }
        }

        // ���� �� ������ ������� ���� ��������� � ���� �����, ����������� �������
        if (all_match) {
            common_bits_count++;
        }
    }

    return common_bits_count;
}


//---------------------------------------- ���������� ������� ----------------------------------------
void TestStack();                                                          // ������� ������������ �����
void printOnes(unsigned char* ones, unsigned char size);                   // ������� ��� ������ ������� ������
void print_karnaugh_map(unsigned char** map, unsigned char A, int B);      // ������� ��� ������ ����� �����
void print_karnaugh_map_bool(unsigned char** map, unsigned char A, int B); // ������� ��� ������ ���������� ����� �����
void print_gray_matrix(char*** gray_matrix, int rows, int cols);           // ������� ��� ������ ������� ���� ����
void printList(ListNode* head);                                            // ���������� ������� ��� ������ ����������� ������
void TestList();                                                           // ������� ������������ ������
void TestBitCountFunc();                                                   // ������� ������������ �������� ���������� ���

int main()
{
    setlocale(LC_ALL, "Rus");

    // �������� ������������� ������
    const char* str = "3";                     
    N = 4;                                      // ���������� ����������

    printf("�������� ������: %s\n", str);       // ���������� ����� ������

    ones = parse_numbers(str, &size_ones);    // �������� ������ ����� � ��� ������
    printOnes(ones, size_ones);               // ���������� �����

    A = (N / 2) * 2;                          // ���������� ������ ������� ��� N ����������
    B = (N - (N / 2)) * 2;                    // ���������� �������� ������� ��� N ����������

    int rows, cols;

    // ��������� � ����� ������� ���� ����
    gray_matrix = generate_gray_code_matrix(N, &rows, &cols);
    if (gray_matrix) {
        print_gray_matrix(gray_matrix, rows, cols);

        // �������! ������������ ����ר�� ���
        TestBitCountFunc();
    }

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
    if (gray_matrix) {
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
    
    if (ones) free(ones) ; // ����������� ������

    TestStack();
    TestList();

    return 0;
}

//------------------------------------------����������------------------------------------------
// ������� ��� ������ ������� ������
void printOnes(unsigned char* ones, unsigned char size)
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
void print_gray_matrix(char*** gray_matrix, int rows, int cols) {
    printf("������� ���� ����:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s ", gray_matrix[i][j]);
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

// ������� ������������ ����� (����� �������)
void TestStack()
{
    printf("-----������������ �����-----\n");

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
    printf("-----������������ ������-----\n");
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

void TestBitCountFunc()
{
    printf("\n-----������������ �������� ���-----\n");

    struct stack* pt = newStack(4);

    // ���������� ��������� � ����
    push(pt, (IndexPair) { 0, 0 });
    push(pt, (IndexPair) { 0, 1 });
    push(pt, (IndexPair) { 1, 0 });
    push(pt, (IndexPair) { 1, 1 });

    // �������� ������� ��������
    printf("���������� ���: %d\n", count_common_bits(pt));

    // �������� ��������� �� �����
    pop(pt);
    pop(pt);

    // ������������ ������
    free(pt->items);
    free(pt);
}
