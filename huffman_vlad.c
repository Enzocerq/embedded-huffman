#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 8000
#define MAX_CHAR 256

// Estrutura para representar um nó na árvore de Huffman
struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// Estrutura para representar o Min Heap
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode* array[MAX_CHAR];
};

// Array estático para os nós
struct MinHeapNode nodes[MAX_CHAR];
int nodeIndex = 0;

// Função para criar um novo nó
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = &nodes[nodeIndex++];
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Função para criar um Min Heap
void createMinHeap(struct MinHeap* minHeap, unsigned capacity) {
    minHeap->size = 0;
    minHeap->capacity = capacity;
}

// Função utilitária para trocar dois nós
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Função para minHeapify
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < (int)minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < (int)minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Função para extrair o nó de menor valor
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Função para inserir um novo nó no Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Função para construir o Min Heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Função para criar e construir o Min Heap
void createAndBuildMinHeap(struct MinHeap* minHeap, char data[], int freq[], int size) {
    createMinHeap(minHeap, size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);
}

// Função para construir a árvore de Huffman
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap minHeap;

    createAndBuildMinHeap(&minHeap, data, freq, size);

    while (minHeap.size > 1) {
        left = extractMin(&minHeap);
        right = extractMin(&minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(&minHeap, top);
    }

    return extractMin(&minHeap);
}

// Função para gerar os códigos de Huffman
void generateCodes(struct MinHeapNode* root, char* code, int top, char codes[][MAX_TREE_HT]) {
    if (root->left) {
        code[top] = '0';
        generateCodes(root->left, code, top + 1, codes);
    }
    if (root->right) {
        code[top] = '1';
        generateCodes(root->right, code, top + 1, codes);
    }
    if (!root->left && !root->right) {
        code[top] = '\0';
        strcpy(codes[(int)root->data], code);
    }
}

// Função para realizar a compressão
void compressInput(char input[], char codes[][MAX_TREE_HT], char* compressed) {
    for (int i = 0; input[i] != '\0'; ++i) {
        strcat(compressed, codes[(int)input[i]]);
    }
}

// Função para converter uma string binária para caracteres ASCII
char binary_to_char(const char *binary) {
    int value = 0;
    for (int i = 0; i < 8; ++i) {
        value = value * 2 + (binary[i] - '0');
    }
    return (char)value;
}

// Função para alinhar a string binária e converter para caracteres
void convertToAscii(char* compressed) {
    int length = strlen(compressed);
    int padded_length = ((length + 7) / 8) * 8;
    int padding = padded_length - length;

    char padded_data[padded_length + 1];
    memset(padded_data, '0', padding);
    strcpy(padded_data + padding, compressed);

    char output[padded_length / 8];
    int index = 0;

    for (int i = 0; i < padded_length; i += 8) {
        output[index++] = binary_to_char(&padded_data[i]);
    }

    // Exibe a string convertida
    printf("Compressed (ASCII):\n");
    for (int i = 0; i < index; ++i) {
        putchar(output[i]);
    }
    putchar('\n');
}

// Função para gerar os códigos de Huffman e realizar a compressão
void HuffmanCodes(char data[], int size) {
    int freq[MAX_CHAR] = {0};
    for (int i = 0; i < size; ++i)
        freq[(int)data[i]]++;

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
    char code[MAX_TREE_HT];
    generateCodes(root, code, 0, codes);

    char compressed[MAX_TREE_HT * MAX_CHAR] = {0};
    compressInput(data, codes, compressed);

    printf("Compressed Data:\n%s\n", compressed);
    convertToAscii(compressed);
}

// Programa principal
int main() {
    char arr[8000];
    for (int i = 0; i < 8000; ++i) {
        arr[i] = 'a' + (i % 26); // Preenche o array com caracteres de 'a' a 'z'
    }  
    //char arr[] = { 'a', 'b', 'r', 'a', 'c', 'a', 'd', 'a', 'b', 'r', 'a' };

    int size = sizeof(arr) / sizeof(arr[0]);

    HuffmanCodes(arr, size);

    return 0;
}