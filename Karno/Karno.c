// �������
/*
1 ����������� ��� f = Const
2 ���������� ��������� � ������
*/

// Karno.c
#include <locale.h>

#include "Stack.h"

unsigned char* ones;      // ���������� ���������� ������� ������
unsigned char  size_ones; // ���������� ���������� ������� ������� ������
unsigned char  A, B;      // ����������� ����� �����
char*** gray_matrix;      // ������� ����� ����
unsigned char N;          // ���������� ����������

const char* code_gray[] = { "00", "01", "11", "10" };  // ��� ���� ��� 2 ��� (���������� ��� �������)
// ��������
const char* code_gray2[2][2] = {
    {"00", "01"},
    {"10", "11"}
}; // ��� ���� ��� 2 ����������

// ��������� ��� �������� ���� �������� ��� ����� �����
typedef struct {
    unsigned char** kmap;     // ����� �����
    bool** kmap_bool;         // ������ false
} KarnaughMaps;

KarnaughMaps maps;

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
                    // found = true;
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

            // ���������� ��� ���� ��� ������ � �������, �������� ������ ������ N
            if (N == 2) {
                // ��������� ���������
                // ��� 2 ���������� ���������� ������ � ������� �������� �� code_gray
                gray_matrix[i][j] = code_gray2[i][j];
            }
            else if (N == 3) {
                // ��� 3 ���������� ���������� ���� ��� ��� ������ � ��� ���� ��� �������
                gray_matrix[i][j][0] = (i == 0) ? '0' : '1'; // 1 ��� ��� ������
                gray_matrix[i][j][1] = code_gray[j][0];
                gray_matrix[i][j][2] = code_gray[j][1];
                gray_matrix[i][j][3] = '\0';
            }
            else if (N == 4) {
                // ��� 4 ���������� ���������� ��� ���� ��� ������ � ��� ���� ��� �������
                gray_matrix[i][j][0] = code_gray[i][0];
                gray_matrix[i][j][1] = code_gray[i][1];
                gray_matrix[i][j][2] = code_gray[j][0];
                gray_matrix[i][j][3] = code_gray[j][1];
                gray_matrix[i][j][4] = '\0';
            }
        }
    }

    return gray_matrix;
}

struct stack* pt;

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

// ������� ����� ����� ����������� (�������)
// ������� ��� ������ ���������
bool minimize(unsigned char x, unsigned char y)
{
    // ���������� �������� � ����
    push(pt, (IndexPair) { x, y });

    int bits;                                     // ����� ���������� ���
    bool match = (size(pt) == 1) ? true : false;  // ����, �����������, ��� ������� �������� ��� �������
    bool break_flag = false;                      // ���� ��������������� ��� ��������� �������
    
    bits = (size(pt) == 1) ? N : count_common_bits(pt);  // ��������� ���������� ����������� ���
    if (bits == 0)
        break_flag = true;         // ������� �� ��������
    else
        switch (size(pt))
        {
        case 2:
            match = true;
            if (N == 2) break_flag = true;  // ��� ���� ����������
            break;
        case 4:
            if (bits == 1 && N == 3)        // ��� ��� ����������
                match = true;
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

            if ((maps.kmap[nx][ny] == 1) && (!include(pt, (IndexPair) { nx, ny }))) {
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
        maps.kmap_bool[x][y] = true; // ������� ����������� ���������� ���������
        return true; // �������� �����������   
    }

    pop(pt);         // ������� �������
    return false;    // ������ �����������
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
    const char* str = "0 1 2 3";                     
    N = 2;                                      // ���������� ����������

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
        // TestBitCountFunc(); // ��� ������� ������ ��� ������ ������ ������������
    }

    maps = create_karnaugh_map(N);

    if (maps.kmap != NULL) {
        printf("����� ����� (� ���� �������� �������):\n");
        print_karnaugh_map(maps.kmap, A, B);
    }

    // �������� ��������
    int size = 1;                      // ������������ ������ ������� ����������
    for (int i = 0; i < N-1; i++)
        size *= 2;
    pt = newStack(size);

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if ((maps.kmap[i][j] == 1) && (!maps.kmap_bool[i][j])) {
                printf("������� ������� �����������:\n");
                if (minimize(i, j))
                    clear(pt);                   // ������� �����
                    maps.kmap_bool[i][j] = true; // ������� ����������� ����������
            }
        }
    }

    // ������ ����� ����� � ����������� ���������
    if (maps.kmap_bool != NULL) {
        printf("����� ����� (� ���� �������� ���������� �������):\n");
        print_karnaugh_map_bool(maps.kmap_bool, A, B);
    }

    // ������������ ������
    if (gray_matrix && N != 2) {
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

    // TestStack();
    // TestList();

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
