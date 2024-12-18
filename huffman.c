#include <stdio.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 256

// Um nó da árvore de Huffman
struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// Um Min Heap: Coleção de nós de min-heap (ou árvore de Huffman)
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// Função utilitária para alocar um novo nó de min heap com o caractere e a frequência dados
struct MinHeapNode* newNode(char data, unsigned freq) {
    static struct MinHeapNode nodes[MAX_CHAR];
    static int nodeIndex = 0;
    struct MinHeapNode* temp = &nodes[nodeIndex++];
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Função utilitária para criar um min heap com a capacidade dada
struct MinHeap* createMinHeap(unsigned capacity) {
    static struct MinHeap minHeap;
    static struct MinHeapNode* array[MAX_CHAR];
    minHeap.size = 0;
    minHeap.capacity = capacity;
    minHeap.array = array;
    return &minHeap;
}

// Função utilitária para trocar dois nós de min heap
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// A função padrão minHeapify.
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if ((unsigned int)left < minHeap->size && 
        (minHeap->array[left]->freq < minHeap->array[smallest]->freq || 
        (minHeap->array[left]->freq == minHeap->array[smallest]->freq && minHeap->array[left]->data < minHeap->array[smallest]->data)))
        smallest = left;

    if ((unsigned int)right < minHeap->size && 
        (minHeap->array[right]->freq < minHeap->array[smallest]->freq || 
        (minHeap->array[right]->freq == minHeap->array[smallest]->freq && minHeap->array[right]->data < minHeap->array[smallest]->data)))
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Função utilitária para verificar se o tamanho do heap é 1 ou não
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Função padrão para extrair o nó de valor mínimo do heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Função utilitária para inserir um novo nó no Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Função padrão para construir o min heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Função utilitária para imprimir um array de tamanho n
void printArr(int arr[], int n) {
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
    printf("\n");
}

// Função utilitária para verificar se este nó é uma folha
int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

// Cria um min heap com capacidade igual ao tamanho e insere todos os caracteres de data[] no min heap. Inicialmente, o tamanho do min heap é igual à capacidade
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

// A função principal que constrói a árvore de Huffman
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

// Função utilitária para calcular a frequência dos caracteres
void calculateFrequency(char data[], int freq[], int size) {
    for (int i = 0; i < size; ++i)
        freq[(int)data[i]]++;
}

// Imprime os códigos de Huffman a partir da raiz da árvore de Huffman. Usa arr[] para armazenar os códigos
void printCodes(struct MinHeapNode* root, char codes[][MAX_TREE_HT]) {
    int arr[MAX_TREE_HT];
    int top = 0;
    struct MinHeapNode* stack[MAX_TREE_HT];
    int stackTop = 0;
    struct MinHeapNode* current = root;
    int visited[MAX_TREE_HT] = {0};

    while (stackTop > 0 || current != NULL) {
        while (current != NULL) {
            stack[stackTop++] = current;
            arr[top++] = 0;
            current = current->left;
        }

        current = stack[stackTop - 1];

        if (current->right != NULL && !visited[stackTop - 1]) {
            visited[stackTop - 1] = 1;
            current = current->right;
            arr[top - 1] = 1;
        } else {
            stackTop--;
            top--;

            if (isLeaf(current)) {
                for (int i = 0; i < top; ++i)
                    codes[(int)current->data][i] = arr[i] + '0';
                codes[(int)current->data][top] = '\0';
            }

            current = NULL;
        }
    }
}

// Função utilitária para imprimir os códigos de Huffman e suas frequências
void printHuffmanCodesAndFrequencies(char data[], char codes[][MAX_TREE_HT], int freq[], int size) {
    int printed[MAX_CHAR] = {0};

    printf("%-10s %-10s %-10s\n", "char", "code", "frequency");
    printf("--------------------------------\n");

    for (int i = 0; i < size; ++i) {
        if (!printed[(int)data[i]]) {
            printf("%-10c %-10s %-10d\n", data[i], codes[(int)data[i]], freq[(int)data[i]]);
            printf("--------------------------------\n");
            printed[(int)data[i]] = 1;
        }
    }
}

// A função principal que constrói uma árvore de Huffman e imprime os códigos percorrendo a árvore de Huffman construída
void HuffmanCodes(char data[], int size) {
    int freq[MAX_CHAR] = {0};
    calculateFrequency(data, freq, size);

    char uniqueData[MAX_CHAR];
    int uniqueFreq[MAX_CHAR];
    int uniqueSize = 0;

    for (int i = 0; i < MAX_CHAR; ++i) {
        if (freq[i] > 0) {
            uniqueData[uniqueSize] = (char)i;
            uniqueFreq[uniqueSize] = freq[i];
            uniqueSize++;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(uniqueData, uniqueFreq, uniqueSize);

    char codes[MAX_CHAR][MAX_TREE_HT] = {{0}};
    printCodes(root, codes);

    printHuffmanCodesAndFrequencies(data, codes, freq, size);

    for (int i = 0; i < size; ++i)
        printf("%s", codes[(int)data[i]]);
    printf("\n");
}

// Programa principal para testar as funções acima
int main() {
    /* char arr[8000];
    for (int i = 0; i < 8000; ++i) {
        arr[i] = 'a' + (i % 26); // Preenche o array com caracteres de 'a' a 'z'
    } */
    char arr[] = { 'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a' };
    int size = sizeof(arr) / sizeof(arr[0]);

    HuffmanCodes(arr, size);

    return 0;
}