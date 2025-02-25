/*
 * Algoritmo de Codificação de Huffman - Implementação em C
 * Plataforma: STM32F030
 *
 * Autores: Enzo Girão de Cerqueira e Vladimir Rodaly Joseph
 * Data: 22 de Janeiro de 2025
 *
 * Descrição:
 * Esta aplicação implementa o algoritmo de codificação de Huffman para
 * compressão de dados de texto. Ela calcula frequências de caracteres, constrói
 * uma árvore de Huffman, gera códigos e comprime os dados de entrada em um formato binário.
 *
 * Entradas:
 * - Um array de caracteres (texto) para compressão.
 * - Caracteres ASCII para cálculo de frequência.
 *
 * Saídas:
 * - Códigos de Huffman para cada caractere.
 * - Dados comprimidos em formato binário convertidos para representação ASCII.
 *
 * Licença:
 * Livre para uso e modificação com atribuição ao autor.
 *
 * Uso:
 * Compile o código usando um compilador C direcionado ao STM32F030.
 * Personalize os dados de entrada e execute o programa para observar a saída.
 *
 * Histórico de Modificações:
 * -------------------------------------------------------------
 * --  #1.
 * --  Data: 18 de Dez, 2024
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Adicionado impressão de frequência e reformulação do cálculo de frequência.
 * -------------------------------------------------------------
 * --  #2.
 * --  Data: 18 de Dez, 2024
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Ajustado minHeapify para comparar dados quando frequências são iguais.
 * -------------------------------------------------------------
 * --  #3.
 * --  Data: 18 de Dez, 2024
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Melhorada a função de impressão para exibir códigos e frequências formatados.
 * -------------------------------------------------------------
 * --  #4.
 * --  Data: 18 de Dez, 2024
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Reformuladas funções para aceitar parâmetro de tamanho e removido arquivo de teste.
 * -------------------------------------------------------------
 * --  #5.
 * --  Data: 18 de Dez, 2024
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Comentada inicialização de array de teste na função principal.
 * -------------------------------------------------------------
 * --  #6.
 * --  Data: 15 de Jan, 2025
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Implementado tratamento de caractere EOF e melhorias na impressão de códigos.
 * -------------------------------------------------------------
 * --  #7.
 * --  Data: 20 de Jan, 2025
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Otimizado uso de array estático.
 * -------------------------------------------------------------
 * --  #8.
 * --  Data: 22 de Jan, 2025
 * --  Autor: Enzo Girão de Cerqueira
 * --  Motivo: Reduzido MAX_CHAR para 128.
 * -------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
//#include <windows.h>
//#include <psapi.h>
#include <time.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 128
#define EOF_CHAR '\0'
#define SIZE 8000

// Um nó na árvore de Huffman
struct MinHeapNode
{
  char data;                 // Caractere armazenado neste nó
  unsigned freq;             // Frequência do caractere
  struct MinHeapNode *left;  // Ponteiro para o filho esquerdo
  struct MinHeapNode *right; // Ponteiro para o filho direito
};

// Um MinHeap para armazenar ponteiros para MinHeapNode
struct MinHeap
{
  unsigned size;                       // Número atual de elementos no heap
  unsigned capacity;                   // Capacidade máxima do heap
  struct MinHeapNode *array[MAX_CHAR]; // Array de ponteiros para MinHeapNode
};

// Array estático para nós (otimização de memória)
static struct MinHeapNode nodes[MAX_CHAR];
static int nodeIndex = 0; // Rastrea o próximo índice de nó livre

// Variáveis globais para frequências de caracteres e códigos
static int freq[MAX_CHAR] = {0};
static char uniqueData[MAX_CHAR];
static int uniqueFreq[MAX_CHAR];
static char codes[MAX_CHAR][MAX_TREE_HT] = {{0}};
static char compressed[MAX_TREE_HT * MAX_CHAR] = {0};
static struct MinHeapNode *stack[MAX_TREE_HT];
static int arr[MAX_TREE_HT];
static int visited[MAX_TREE_HT] = {0};
static char padded_data[MAX_TREE_HT * MAX_CHAR] = {0};
static char output[MAX_TREE_HT * MAX_CHAR / 8] = {0};

/**
 * Aloca um novo MinHeapNode.
 *
 * @param data Caractere para o nó.
 * @param freq Frequência do caractere.
 * @return Ponteiro para o novo nó.
 */
struct MinHeapNode *newNode(char data, unsigned freq)
{
  struct MinHeapNode *temp = &nodes[nodeIndex++];
  temp->left = temp->right = NULL;
  temp->data = data;
  temp->freq = freq;
  return temp;
}

/**
 * Cria e inicializa um MinHeap.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @param capacity Capacidade máxima do heap.
 */
void createMinHeap(struct MinHeap *minHeap, unsigned capacity)
{
  minHeap->size = 0;
  minHeap->capacity = capacity;
}

// Função utilitária para trocar dois nós de min heap
void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b)
{
  struct MinHeapNode *t = *a;
  *a = *b;
  *b = t;
}

/**
 * Função MinHeapify para manter a propriedade do heap.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @param idx Índice do nó atual para aplicar heapify.
 */
void minHeapify(struct MinHeap *minHeap, int idx)
{
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < (int)minHeap->size &&
      minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < (int)minHeap->size &&
      minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx)
  {
    swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

/**
 * Verifica se o tamanho do MinHeap é um.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @return 1 se o tamanho for um, 0 caso contrário.
 */
int isSizeOne(struct MinHeap *minHeap) { return (minHeap->size == 1); }

/**
 * Extrai o nó com a frequência mínima do heap.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @return Ponteiro para o nó extraído.
 */
struct MinHeapNode *extractMin(struct MinHeap *minHeap)
{
  struct MinHeapNode *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[minHeap->size - 1];
  --minHeap->size;
  minHeapify(minHeap, 0);
  return temp;
}

/**
 * Insere um novo nó no MinHeap.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @param minHeapNode Ponteiro para o nó a ser inserido.
 */
void insertMinHeap(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode)
{
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
  {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  minHeap->array[i] = minHeapNode;
}

/**
 * Constrói um MinHeap a partir dos nós dados.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 */
void buildMinHeap(struct MinHeap *minHeap)
{
  int n = minHeap->size - 1;
  for (int i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

/**
 * Verifica se um nó é uma folha.
 *
 * @param root Ponteiro para a estrutura MinHeapNode.
 * @return 1 se for folha, 0 caso contrário.
 */
int isLeaf(struct MinHeapNode *root) { return !(root->left) && !(root->right); }

/**
 * Cria e constrói um MinHeap a partir dos dados fornecidos.
 *
 * @param minHeap Ponteiro para a estrutura MinHeap.
 * @param data Array de caracteres.
 * @param freq Array de frequências.
 * @param size Tamanho do array de caracteres.
 */
void createAndBuildMinHeap(struct MinHeap *minHeap, char data[], int freq[],
                           int size)
{
  createMinHeap(minHeap, size);

  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(data[i], freq[i]);

  minHeap->size = size;
  buildMinHeap(minHeap);
}

/**
 * Constrói uma Árvore de Huffman a partir dos dados de entrada.
 *
 * @param data Array de caracteres únicos.
 * @param freq Array de frequências dos caracteres.
 * @param size Número de caracteres únicos.
 * @return Ponteiro para a raiz da Árvore de Huffman.
 */
struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size)
{
  struct MinHeapNode *left, *right, *top;
  struct MinHeap minHeap;

  createAndBuildMinHeap(&minHeap, data, freq, size);

  while (minHeap.size > 1)
  {
    left = extractMin(&minHeap);
    right = extractMin(&minHeap);

    top = newNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;

    insertMinHeap(&minHeap, top);
  }

  return extractMin(&minHeap);
}

// Função utilitária para calcular a frequência dos caracteres por partes (ex: 1000 caracteres por vez)
void calculateFrequencyInChunks(const char data[], int freq[], int size, int chunkSize)
{
  for (int start = 0; start < size; start += chunkSize)
  {
    int end = (start + chunkSize < size) ? start + chunkSize : size; // Garante que não ultrapasse o tamanho total
    for (int i = start; i < end; ++i)
    {
      freq[(int)data[i]]++; // Acessa diretamente a memória Flash
    }
  }
}

/**
 * Gera os códigos de Huffman para cada caractere.
 *
 * @param root Ponteiro para a raiz da árvore de Huffman.
 * @param codes Matriz para armazenar os códigos gerados.
 */
void generateCodes(struct MinHeapNode *root, char codes[][MAX_TREE_HT])
{
  int stackTop = 0;
  struct MinHeapNode *current = root;
  int top = 0;

  while (stackTop > 0 || current != NULL)
  {
    while (current != NULL)
    {
      stack[stackTop++] = current;
      arr[top++] = 0;
      current = current->left;
    }

    current = stack[stackTop - 1];

    if (current->right != NULL && !visited[stackTop - 1])
    {
      visited[stackTop - 1] = 1;
      current = current->right;
      arr[top - 1] = 1;
    }
    else
    {
      stackTop--;
      top--;

      if (isLeaf(current))
      {
        for (int i = 0; i < top; ++i)
          codes[(int)current->data][i] = arr[i] + '0';
        codes[(int)current->data][top] = '\0';
      }

      current = NULL;
    }
  }
}

// Função para realizar a compressão
void compressInput(const char input[], int size, char codes[][MAX_TREE_HT],
                   char *compressed)
{
  int bitIndex = 0;
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; codes[(int)input[i]][j] != '\0'; ++j)
    {
      compressed[bitIndex++] = codes[(int)input[i]][j];
    }
  }
}

// Função para converter uma string binária para caracteres ASCII
char binary_to_char(const char *binary)
{
  int value = 0;
  for (int i = 0; i < 8; ++i)
  {
    value = value * 2 + (binary[i] - '0');
  }
  return (char)value;
}

// Função para alinhar a string binária e converter para caracteres
void convertToAscii(char *compressed)
{
  int length = strlen(compressed);
  int padded_length = ((length + 7) / 8) * 8;
  int padding = padded_length - length;

  memset(padded_data, '0', padding);
  strcpy(padded_data + padding, compressed);

  int index = 0;

  for (int i = 0; i < padded_length; i += 8)
  {
    output[index++] = binary_to_char(&padded_data[i]);
  }

  // Exibe a string convertida
  printf("Compressed (ASCII):\n");
  for (int i = 0; i < index; ++i)
  {
    putchar(output[i]);
  }
  putchar('\n');

  // Exibe a contagem de caracteres ASCII gerados, excluindo o EOF
  printf("Number of ASCII characters generated: %d\n", index - 1);
}

// Função para imprimir os códigos de Huffman gerados
void printHuffmanCodes(char codes[][MAX_TREE_HT])
{
  printf("Huffman Codes:\n");
  for (int i = 0; i < MAX_CHAR; ++i)
  {
    if (codes[i][0] != '\0')
    {
      if (i == (int)EOF_CHAR)
      {
        printf("EOF: %s\n", codes[i]);
      }
      else
      {
        printf("%c: %s\n", i, codes[i]);
      }
    }
  }
}

/**
 * Gera os códigos de Huffman e realiza a compressão.
 *
 * @param data Dados de entrada (texto).
 * @param size Tamanho dos dados de entrada.
 */
void HuffmanCodes(const char data[], int size)
{
  memset(freq, 0, sizeof(freq));

  // Chama a função que calcula a frequência em partes (1000 caracteres por vez)
  calculateFrequencyInChunks(data, freq, size, 1000);

  // Adiciona o símbolo EOF ao conjunto de caracteres
  freq[(int)EOF_CHAR] = 1;

  int uniqueSize = 0;
  for (int i = 0; i < MAX_CHAR; ++i)
  {
    if (freq[i] > 0)
    {
      uniqueData[uniqueSize] = (char)i;
      uniqueFreq[uniqueSize] = freq[i];
      uniqueSize++;
    }
  }

  struct MinHeapNode *root =
      buildHuffmanTree(uniqueData, uniqueFreq, uniqueSize);
  generateCodes(root, codes);

  // Imprime os códigos de Huffman gerados
  printHuffmanCodes(codes);

  compressInput(data, size, codes, compressed);

  // Adiciona o código do EOF ao final do fluxo comprimido
  for (int j = 0; codes[(int)EOF_CHAR][j] != '\0'; ++j)
  {
    compressed[strlen(compressed)] = codes[(int)EOF_CHAR][j];
  }

  convertToAscii(compressed);
}

// Programa principal
int main()
{
  static const char arr[] = {'B', 'y', 'z', 'b', 'Z', 'W', 'e', 'j', 'w', 'u', 'A', 'K', 'N', '7', 'j', 'M', 'i', 'n', 'i', 'T', 'A', 'W', '0', '2', 'M', 'L', '4', 'r', 'F', 'y', '3', 'E', 'n', 'r', 'd', 'A', 'D', '7', 'h', 'z', 'p', 'h', '7', '3', 'e', 'h', 'D', 'A', 'I', 'l', 'S', 'I', '5', 'I', '8', 'F', 'U', '1', 'u', 'X', 'p', 'm', 'a', 'Q', 'T', '3', 'R', 'U', 'z', 'w', 'J', 'c', 'U', 'F', 'T', 'w', 'k', 'X', 'x', 's', '6', 'D', '0', '0', 'L', 'x', '3', 'd', 'o', 'm', 'z', 'R', 'O', 'P', 'g', 'f', 'G', 'x', 'z', '3', 'h', '6', 'W', '1', 'B', 'n', 'o', 'l', 'A', 'Z', 'R', '5', 'c', 'G', 't', 'n', '3', 'k', 'F', 'f', 'M', '4', 'u', 'Y', 'J', 'Q', '1', 'N', 'B', 'q', 'H', 'j', 'l', 'b', 'a', 'k', 'E', 'E', 'K', 'D', 'd', 'b', '6', '3', 'f', 'p', 'h', 'W', 'P', 'k', '1', 'a', 'e', 'k', 'y', 'l', '0', 'q', 'y', '2', 'U', '3', 'b', '3', 'S', 'z', 'e', 'V', '2', 'o', 'W', 'T', 'D', 'S', 'N', 'j', '5', 's', '5', 'I', 'S', 'v', 'i', 'w', 'V', 'V', 'V', 'L', '9', 'J', 'B', 'R', 'B', 'a', 'L', 'R', 'Q', 'p', 'm', 'G', 'R', '8', 'Z', 'U', 'Q', 'm', '1', 'J', 'S', 'v', 'S', 'k', 'g', 'y', 'U', '1', 'H', 'p', 'A', 'G', 'x', 'A', 'W', 'y', 'a', 'h', 'F', 'o', 'K', 'q', 'u', 'Z', 'o', 'K', 'r', '3', 'u', 'j', 'C', 'D', 'U', 'S', 'l', 'y', 'F', '6', 'n', 'M', 'l', 'l', 'R', 'Y', 'l', 'l', 'K', 'C', 'I', 'Q', 'q', 'Q', '4', 'Z', 'n', 'i', 'h', 'U', 'Z', 'R', '1', 'a', 'S', 'L', 's', '4', '7', 'v', '0', 'k', '8', 'Z', 'M', 'N', 'y', 'v', 'y', '8', 'x', '4', 'M', 'c', 'I', 'G', 'z', 'v', 'n', 'W', 'F', 'C', 'l', '5', 'c', '4', 'G', 'J', 'w', 'C', '4', 'k', 'm', '0', '9', 'w', 'B', 'v', 'i', 'x', 's', 'T', 'p', '4', 't', 'x', '9', 'i', 'i', 'm', '4', 'm', 'm', 'g', 'h', 'D', 'Y', 'l', 'M', 'I', 'l', 'E', 'q', 'O', '3', 'p', '8', '2', 'Z', 'e', 'q', 'H', 'y', 'V', '9', 'h', 'H', '6', 'E', 'n', 'g', '8', 'P', 'J', 'c', 'u', 'M', 'y', 'U', 'W', '6', '5', 'Y', 'm', 'H', 'P', 'Q', 'G', 'I', 'n', 'i', 'w', 's', 'W', 'F', 'p', '2', 'M', 'm', '4', '0', 'I', '2', 'D', 'P', 'T', 'w', 'b', 'H', 'G', 'v', 'C', 'B', 'K', 'm', 'J', 'Z', '0', 'N', 'f', 'd', 'v', 'P', 'J', 'G', 'V', 'z', '6', 'f', 'b', '0', 'T', 'h', 'q', 'X', 'x', '9', 'H', 'O', 'F', 'N', '8', 'F', 'O', 'I', 'r', 'V', 'f', 'g', 'g', '9', '9', 'S', 'O', 'H', 'W', 'j', '6', 'Q', 'D', 'X', 'G', 'W', '2', 'u', 'r', 'p', 's', 'y', '2', 'x', 'B', '0', '0', 'O', '6', 'h', 'h', 'b', 'B', 'E', 'a', 'B', 'U', 'p', 'I', 'q', 'M', 'C', '6', 'N', 'X', 'A', 'k', 'P', 'u', 'P', '5', 'c', 'E', 'v', 'N', 'D', 'l', 'B', 'P', 'h', 'h', 'x', '6', 'i', '1', 'V', 'h', 'J', '8', 'p', 'z', 'K', 'p', 't', 'W', 'C', 't', '6', 'S', 'b', 'L', 'L', '2', 'N', '6', 'D', 'P', 'f', 'F', 'e', 'A', 'm', 'P', '6', 'K', 'E', 'P', 's', 'N', 'N', 'V', 'A', 'W', 'B', 't', 's', 'B', 'a', 'm', 'R', '2', 'x', 'a', 's', '9', 'U', 'v', 'O', 'z', 'y', 'q', 'x', 'Y', '6', 's', 'j', 'A', '7', 'P', 'M', 'L', 'i', 'M', 'f', 'j', '6', 'N', 'j', 'W', 'z', '0', 'M', 'k', 'P', '2', 'h', 'j', 'm', 'v', 'Z', 'a', 'c', 'K', 'x', 'W', '2', 'W', 'U', '0', 'j', 'g', 'B', 'H', 't', 'e', 'p', 'p', 'r', 'O', '9', 'e', 'C', 'K', 'E', 'R', 'A', 'm', '1', 'm', 'V', 'O', 'B', 'v', 'Y', 'y', 'H', 'R', 'I', 'c', 'H', 'r', '8', 'G', 'y', 'p', 'i', 'd', 'U', 'P', 'p', 'S', 'r', 's', 'a', 'v', '7', 'a', 'V', 'y', 'D', 'p', 'A', 'C', 'a', 'X', '0', 'g', 'o', '8', '6', 't', 'n', '2', 'x', 'b', 'i', 'V', '3', '0', 'i', 'i', 'G', 'P', 'O', 'g', '8', 'M', '5', 'U', '8', '8', '7', '9', '8', 'V', 'W', 'y', '1', '8', 'u', 'v', 'p', 'V', 'o', 'c', 'w', 'K', 'v', 'n', 'A', 'T', 'K', 'F', 'g', 'Y', 'l', 'e', 'i', 'g', 'w', 'h', 'c', 't', 'e', 'a', 'F', 'y', 'M', '4', 'w', '4', 'q', 'Z', 'P', 'U', 'z', 'A', 'c', 'k', 'l', 'c', '1', 'v', 'f', 'X', 'K', 'H', '1', 's', 'l', 'o', 'N', 'C', 'V', 'r', 'c', 'a', 'f', 'n', 'V', 'P', 'f', '9', 'o', 't', 'T', 'd', 't', 't', 'B', 'S', 'L', '2', 'E', 'p', 'N', 'M', 'w', 'D', '2', 'V', 'p', 'E', 'X', 'A', 'v', 'y', 'Z', 'P', 'b', 'u', 'e', '4', 't', 'H', 'n', 'A', 'k', 'U', 't', 'k', 'n', '2', 'a', 'p', 'h', 'o', '1', 'R', 'r', 'r', 'n', 'W', 'v', '8', 'U', 'f', 'w', 't', 'u', 'L', 'b', 'O', 'G', 'I', 'V', 't', 'I', '4', 'C', 'z', 'e', 'p', 'q', '4', 'U', 'L', 'g', 'J', 'd', 'N', 'y', 'E', 'h', 'k', 'A', '0', 'D', 'x', 'j', 'x', '8', 'B', 'B', 'C', 'J', 'V', 'u', 'Q', 'N', 'w', 'D', 'r', 'Z', 't', 'j', 't', '3', 'G', '0', 'U', 'R', 'o', 'Y', 'x', 'M', 'W', 'n', 'P', 'J', 'M', 'A', 'G', 'L', 'C', 'I', 'T', 'V', '0', 'h', 'i', 'm', 'k', 'N', 'B', 'S', 'w', 's', 'L', '2', 'g', 'f', 'I', 'J', '1', '5', 'V', 'N', 'g', 'j', 'V', 's', 'j', 'b', '2', 'j', 'h', 'L', '2', 'Y', 's', 'a', '8', 'Q', 'n', '8', 'i', 'Y', 'q', 'r', 'O', 'N', 'K', 'W', 'W', '9', 'P', 'p', 'L', 'v', 'M', 'g', 'c', 'u', '5', 'S', 'T', 'a', 'b', 'K', 'y', 'H', 'k', 'v', 'X', 'M', 't', '4', 'i', 'X', 'l', 'w', 'k', 'u', 'H', '4', 't', 'W', 'i', '4', 'G', 'u', 'Y', 'g', 'c', 'R', 'y', 'u', 's', 'P', '4', 'e', 'W', 'c', 'P', 'r', 'o', '8', 'l', 'K', 'W', 'L', '7', '4', '5', 'E', 'x', 'm', 'Y', 'V', 'f', 'c', 'D', '3', '8', 'g', 'J', 'u', 'O', '1', '7', 'I', 'V', 'T', 'k', 'l', 'B', 'N', 'h', 'k', 'X', '3', 'v', 'S', 'w', 'o', 'U', 'h', 'R', 't', '2', 'u', 'J', '3', 'l', 'I', 'X', 'u', '0', 'l', 'l', 'y', 't', '7', 'F', 'S', 'i', 'G', 'd', 'F', 'p', '0', '7', 'a', 'H', 'r', 'D', 'b', 'O', 'U', 'I', 'E', 'E', 'S', '7', 'p', 'Y', 'T', 'a', 'O', '2', 'B', 'C', 'k', '6', 'I', '2', 'e', 'M', 'V', 'i', '1', 'J', 'd', 'Q', 'Q', 'I', 'T', 'q', 'U', 'l', 'y', 'Y', 'n', 'G', 'T', 'S', 'c', 'm', 'q', 'r', 'f', 'q', 'r', 'F', 'm', 'z', '5', 'F', 'A', 'O', 'l', 'z', 'V', 'C', 'F', 'k', 'I', 'W', 'Q', 'c', '7', 'E', '0', 'k', 'J', 'J', '1', 'l', 'u', 'f', 'Q', 'N', 'L', '8', 'Q', 'w', 'v', 'L', 'z', 'v', 'X', 'Y', 'j', 's', 'Y', 'm', 'S', 'g', '9', 'i', '8', '4', 'l', 'y', 'f', 's', '5', 'U', 'T', 'n', 'z', 'k', '0', '9', 'g', 'E', 't', 'R', 'N', 'g', 'B', 'k', '4', 'k', 'T', 'S', 'M', 'l', 'y', 'J', 'I', 'u', 'E', 't', 'i', 'h', 'Z', 'b', 'z', 's', 'F', 'o', 'Q', '3', 'l', 'u', '8', 'S', 'C', 'L', 'y', 'B', 'u', 's', 'j', 'D', '8', 'v', 'm', 'u', '3', 'u', 'e', '7', 'b', 'A', 'c', '0', 'a', 'R', 'h', 'f', '3', 'v', 'W', 'd', 'g', 'S', 'v', 'g', 'c', 'k', 'h', 'M', 'Q', 'G', 'N', 'P', 'z', 'z', '7', 's', 'k', 'c', 'n', 'B', 'a', 'D', '0', '0', 'U', 'X', 'T', 'M', 'G', 'q', 'p', 'm', '6', 'a', 'I', 'W', '8', 'n', 'i', 'P', 't', 'v', 'c', 'h', 'j', 'X', 'P', 'J', 'x', '1', 'I', 'O', '4', '6', 'E', 'N', 'R', 'W', 'Z', 'Y', 'A', 'C', 'A', '6', 'a', 'H', 'T', 'X', 'u', 'z', 'm', 'c', 'j', 'C', '9', 'I', 'X', 'M', 'P', 'L', 'D', 'V', 'Z', '6', 'S', 'b', 'J', 'j', 'x', 'g', '7', 'x', 'i', '6', 's', '6', 'D', '9', 'T', 'v', 'y', '3', 'L', 'V', '3', 'K', 'b', 'R', 'X', 'q', 'c', 'Y', 'C', '1', 'U', 'U', 'Q', 'b', '3', 'E', '7', 'z', 'z', 'd', 'v', 'h', 'a', 'w', 'g', 'r', 'f', 'T', 'l', 'r', 'o', 'e', 'z', 'G', 'v', 'K', 'u', 'N', 'j', 'w', 'C', '1', 'E', 'T', 'R', '6', 'X', 'O', 'v', 'M', 'q', 'e', 's', 'E', 'O', 'O', 'v', 't', 'f', 'W', 'Y', 'K', '1', 'O', 'O', 'k', 'Y', '8', 'y', '5', 't', '0', 'v', 'x', 'J', 'C', 'r', 'e', 'O', 'c', 'p', '4', '4', 'X', '6', 'I', 'j', 's', '0', 'F', 'C', 'O', 'N', '1', 'a', 'b', 'c', 'z', 'X', 'Q', 's', 'G', 'E', 'b', '2', 'M', 'd', 'j', 'o', 'q', 'A', 'R', 'i', '2', 'm', 'f', 'B', 'M', 'L', '1', 'P', 'N', 'D', 'c', 'F', 'e', '1', 'h', 'R', 'O', 'v', '7', 'T', 'x', 'Z', 'J', '9', '0', 'q', 'm', 'g', 'o', '3', 'D', 'h', 'g', 'i', 'i', 'q', 't', 'X', '5', '4', 'Y', 'V', '9', '2', 'M', 'e', 'H', 'Y', 'P', 'F', 'p', 'D', 'c', 'w', 'A', 'S', 'd', 'k', 'x', 'H', 'e', '0', 'o', '8', 'W', 'K', 'o', 'D', 'f', 'h', '7', '4', '2', '5', 'u', 'C', 'Z', '2', 'a', 'p', '7', 'E', 'q', 'X', '0', 'o', 'p', 'T', 'P', 'a', 'Z', 'r', 'Q', 'D', 'n', 'k', 'M', 'P', 'l', 'r', 'w', 'j', 'j', 'm', 'e', 'U', 'o', '1', 'M', 'E', 'e', 'H', 'I', 'K', 'c', '7', 'x', 'H', 'Q', 'A', 'i', 'p', 'p', 'w', 'r', 'S', 'V', '1', 'h', '6', 'g', 'R', 'd', 'G', '3', '7', 'Y', 'h', 'x', 'i', 'j', 'R', 'n', 'q', 'a', 'F', 'n', 'N', 'M', '3', 'L', 's', 'h', '0', 'd', 'M', 'I', 'y', 'D', 'n', 's', 'j', '4', 'M', 'n', 'x', 'H', 'B', 'U', '4', 'j', '3', 'M', 'M', 'h', 'k', 'R', 'K', 'x', 'c', 'E', '8', 'I', 'j', 'w', 'l', 'v', '4', 'X', 'y', 'h', 'G', 'V', 'Z', 'S', '9', 'M', 'X', '8', 'e', 'S', 'g', 'V', 'c', 's', '3', 'C', '8', 'D', 'z', 'Y', 'F', 'v', 'g', 'o', 'h', 'G', 'X', 'Z', 'd', 'J', '4', 'h', 'f', 'T', 'x', 'c', 'd', 'L', 'Y', '5', 'd', '4', 'O', '5', 'l', 'H', '5', 'j', 'J', 's', '5', 'M', 'd', 'b', 'z', '8', 'h', 'M', 'X', '9', 'W', 'P', 'g', 'z', 'g', 'U', 'R', '7', 'd', 'p', '2', '6', 'i', 'G', 'z', 'J', 'M', 'u', 'q', 'T', 'c', 'l', 'f', '3', 'A', 'U', 'z', 'i', 'h', 'K', 'f', '1', 'Z', '9', 'q', '5', 'R', '5', '2', 'u', 'k', 's', 'q', 'G', 'w', 'd', 'P', '8', 'L', '5', 'P', 'l', 'e', 'v', 'e', 'f', 'D', 'T', 'B', 'i', 'd', 'g', 'Z', '2', 'f', 'F', 'v', 'x', 'A', 'l', 'h', 'j', 'T', 'M', 'n', 'E', 'n', '3', 'A', 'z', 'w', 'Q', 'a', 'P', '9', '3', 'u', 'C', 'M', 'v', 'j', 'o', 'P', '8', 'e', 's', 'C', 'O', 'd', 'A', 'z', 'B', 'h', 'H', 'L', 'L', 'L', 'w', 'C', 'L', 'l', 'w', 'Z', 'A', 'B', 'Y', '1', 'v', 'Z', 'D', 'e', '8', 'p', 'r', '4', 'K', 'a', '4', 'Y', '3', '5', 'L', '2', 'a', 'Q', 'B', 'j', 'b', 'y', 'l', 'l', 'X', 'Y', '8', 'V', 'Z', 'V', 'M', 'J', 'u', 'O', 'l', 'q', '3', 'T', 'k', 'B', 'r', 'f', 'X', 'i', 'Y', 'j', 'Z', 'y', 'z', 'a', 'Y', 'P', 'M', '7', 'y', 'k', 'V', 'u', '3', 't', 'F', 'G', '0', 'z', 'S', 'b', 'd', 'J', 's', 'C', 'L', 'Z', 'h', 'g', '6', '3', 'F', 'V', 'p', '3', 'v', 'B', 'G', '8', '9', '2', 'g', 'S', 'm', 'Z', '9', 'q', 'd', 'z', 'f', 't', 'R', '7', '3', '7', '9', 'E', 'X', 'e', 'i', 'T', 'X', 'm', 'm', 'A', 'f', 'X', 'C', 'j', 'T', '9', 'Z', 'x', 'R', 'B', 'L', 'R', 'r', 'o', 'E', 'L', 'Y', 'V', 'G', 'P', 'T', 'F', 'R', 'o', 'j', 'z', '5', '4', 'b', 'h', '4', '4', '2', '6', 'd', 'K', '5', '2', '7', 'L', '2', 'I', 'a', 'h', 'v', 'e', 'q', 'H', 'y', 'w', 'W', 'F', 'z', 'n', 't', 'Y', 'a', 'm', 'Q', '2', 'H', 'L', 'w', '7', 'H', 'Q', 'H', 'B', 'G', 'D', 'M', '6', 'J', 'k', 'c', '4', 'E', 'I', '9', '2', '3', 'T', '7', 's', '4', 'o', 'F', 'V', 'O', 'V', 'N', 'V', 'g', '9', 'T', 'm', 'N', 'Y', 'l', 'S', 'b', 'v', 'M', 'l', 'V', 'o', 'd', 'Z', 'x', 'b', 'Q', 'o', 'u', 'N', 'U', 'p', '0', 'Z', '8', 'E', 't', 'L', 'a', 'P', 'I', 'r', 'z', 'U', 'F', 'a', 'k', 'f', 'L', 'w', 'G', 'g', 'b', 'h', '4', 'M', '8', 'K', '0', 't', 'V', 'I', 'W', 'L', 'i', 'U', 'O', 'R', 'd', 'm', 'e', 'j', 'T', 'S', '3', 'W', 'q', 'd', '1', '0', 'O', '5', 'U', 'p', 'b', 'O', '1', 'Z', 'Y', 'p', 'G', 's', 'x', 'a', '3', 'T', 's', 'F', 'i', 'J', 'r', 'B', 't', 'A', 'T', 'k', 'V', '7', 'E', 'K', 'x', 'S', 'E', 'I', '5', 'f', 'W', 'u', '2', 't', 'X', '7', 'b', 'K', 'V', 'S', 'c', 'E', 'Y', '8', 'N', 'D', '9', '4', 'E', 'Q', 'f', 'X', 'O', 'j', 'h', 'B', 'z', 'j', 'H', 's', 'C', 'e', 'c', '5', 'L', 'z', '0', 'k', '7', 'L', '0', 'Z', 'N', 'M', 'W', 'Z', 'Q', 'V', 'T', 'S', 'k', 'y', 'p', 'y', 'V', 'K', 'x', 'I', 'u', '5', '0', 'w', 'X', 'S', 'p', 'g', 'H', 'd', 'G', 'D', 'p', '6', 'a', '0', 'H', 'w', 'P', 'X', 'I', 'j', 'p', 'q', 'V', 'S', 'c', 'r', 'c', 'N', 'x', 'K', 'I', 'o', '5', 'd', '4', 'i', '9', 'C', 'C', 'D', 'D', 'p', 'A', 'd', 'f', 'F', 'Q', 't', 'c', 'W', 'Q', 'F', 'C', 'l', 'X', 'e', 'Q', 'x', 'r', 'E', '6', 'y', 'q', '1', 'P', 'k', 'Z', 'P', 'k', 'Z', 'Q', 'n', 'E', 'O', 'H', 'k', 'T', 'X', 'T', 'v', 'r', 'j', 'y', 't', 'I', 'L', 'L', 'X', '7', '0', 'b', '3', 'm', 'F', 'u', '2', 'p', 'H', 'F', 'Q', 'f', 'V', '3', 'j', 'k', '8', 'H', '3', 'T', 'Y', 'n', 'A', '5', 'b', 'r', 'E', 'l', '2', 'b', 'i', 'r', '0', 'b', 'T', '5', 'J', 'J', 'i', 'O', 'O', 'y', 't', 'i', 'q', 'R', 'G', 'o', 'Y', '9', '6', 'x', 'k', '6', 'q', 'A', 'o', 'u', 'l', 'e', 'J', 'H', 'L', '7', 'i', 'd', '2', 'p', 'm', 'b', '1', '1', 'N', 'l', 'X', '1', '0', 'd', 'g', 'O', 'a', 'c', '9', 'B', 'W', 'q', '9', 'A', 'Y', 'k', 'd', 'g', 'p', 'o', 'd', 'L', 'F', 'U', '0', 's', 't', 'r', 'h', '6', 'Q', '4', 'y', 'E', 'X', 'S', 'T', 'x', 's', 'Q', 'w', 'E', '4', 't', 'D', 'T', 'R', 'g', 'x', '6', 'I', 'Q', 'H', 'O', 'k', '7', '4', 'T', 'n', 'b', 'O', '1', 'T', 'C', '6', 'q', 'S', 'N', 'b', 'A', 'b', 'M', 'C', 'W', '5', 'F', 'n', 'N', 'j', 'a', 'J', 'q', 'o', 'P', '2', 'P', 'K', 'w', 'g', 'x', 'N', 'u', 'n', 'g', 'w', 'h', 'K', 'C', 's', 'j', 'C', 'J', 't', 'F', 'd', 'p', 'I', 'E', '0', 'q', 'e', '7', 'W', 'H', 'M', 'M', 'W', 'X', '8', '2', 'I', 'J', 'n', 't', 'n', 'X', 'Q', 'v', 'a', '6', 'U', 'a', 'G', 'E', 'd', 'h', 'r', 'm', 'm', 'f', 'Q', 'E', 'c', 'k', 'L', 'n', 'w', 'f', '8', 's', 'Y', 'G', '1', '9', 'y', 'c', 'U', 'E', 'N', 's', '9', 'f', 'J', 'F', 'h', 'm', 'k', 'O', 'M', 'M', 't', 'c', 'P', 'K', 'A', 'Y', 'v', 'w', '4', 't', 'e', 'Q', 'z', 'T', 'P', 'l', 'v', 'h', 'q', '6', 'Q', 'p', 'c', 'X', 's', '9', '9', 'Q', 'O', 'K', 'd', '5', 'k', 'q', 'D', 'l', 'E', 'y', 'V', '8', 'h', 'y', 'O', 'V', 'H', 'b', '6', '1', '9', 'k', 'x', 'N', 'N', 'P', 'k', '3', 'P', 'h', 'K', 'b', 'r', 'l', 'U', 'S', 'R', 'W', '1', 'T', 'I', 'M', 'S', 'q', 'A', 'e', '9', 'G', '6', '3', '5', '3', 'c', 's', 'E', 'p', '6', 'o', 'h', 'J', 'L', 'r', 'i', '1', 'S', '0', 'H', 'h', 'M', '8', '0', 'V', 'I', 'G', '9', 'J', 'l', '6', 'N', 'f', 'z', 'I', 'Y', 'P', 'y', 'c', '5', 'u', 'E', 'c', '3', 'N', 'H', 'b', 'C', 'X', 'Q', 'J', '4', 'c', 'H', 'u', 'v', 'Q', '1', 's', 'X', 'a', 'm', 'i', '5', 'a', 'q', 'R', 'p', 'c', 't', 'k', 'N', 'v', 'A', 'E', '6', 'H', 'e', '9', 'e', 'u', 'I', 'W', 'K', 'O', 'F', '6', 'c', '6', 'm', 'x', 'U', 'P', 'V', 'P', 'p', '9', 'g', 'U', 'b', 'N', '3', 'm', '8', '1', 'r', '2', '9', 'L', '1', 'b', '5', 'A', 'y', 'F', 'M', '3', '9', 'o', 'x', 'k', 'b', 'F', 'z', 'u', 'U', 'c', 't', '0', 'u', 'I', 'B', 'l', 'v', '7', 'd', 'c', 'z', 'a', 'l', 'r', '1', 'g', 'p', 'n', 'j', '1', 'e', 'h', 'd', 'P', 'H', '4', 'U', '4', 'm', 'm', 'W', 'V', 'a', 'E', 'b', 'b', 'q', 'M', 'Y', 'J', 'm', 'M', 'j', 'L', '1', 'Y', 'p', 'g', 'A', 'P', 'V', 'e', 'u', 'y', 's', '1', 'q', 'C', 't', 'Q', 'n', 'D', 'j', 'D', 'H', 'A', 'f', 'x', 'L', '1', '4', 'v', 'D', 'b', '7', '4', 'y', 'm', 'Y', 'y', 'z', 't', 'Q', 'h', 'f', '8', 'Y', 'J', '9', 'F', 'Z', 'm', 'I', '6', 'p', 'Q', '7', 'I', 'B', 'I', 'A', '4', '1', 'B', 'V', 'y', '4', 'J', 'Z', 'S', '5', 'M', 'A', 'M', 'u', 'd', 'K', 'G', 'l', 'H', 'M', '8', 't', 'U', '5', 'X', 'i', '2', 'f', 'i', '8', 'd', 'c', 'z', 'f', 'v', 'm', 'X', '3', '9', 'p', 'y', 'L', 'p', 'A', '5', 'H', 'J', 'C', 's', 'Q', 'M', 'q', 'A', 'g', 'l', 'V', 'F', 'b', 'y', 'n', 'Z', 'S', 'D', 'N', 'v', 'y', 'z', 'I', 'r', 'y', 'v', 'e', '7', 'Z', 'e', '1', 'g', 'n', '3', 'O', '2', 'F', '4', '0', 'j', 'e', 'L', 'm', '5', '9', 'N', 'V', 'P', 'Q', 'g', 'A', 'D', 'V', 'G', 's', 'H', '2', 'M', 'C', 'R', 'p', '3', 'v', 'Q', 'u', '9', 'I', 'x', '1', '8', 'X', 'T', 'H', '7', 'P', 'H', 'L', 'i', 'W', 'Z', 'E', 'V', 'c', 'X', 'Z', 'J', 'c', 'P', 'V', 'e', 'g', '8', 'Y', 'P', 'P', 'I', 'O', 'X', '4', 'E', 'U', 'R', 'X', 'b', 'O', 'm', 'g', 'X', 'I', '1', 'x', 'M', 'K', 'N', 'h', 'j', 'U', '9', 'z', 'q', 'e', 'T', 'm', '0', 'j', '1', '6', 'v', 'N', '0', 'z', 'f', 'F', 'L', '6', 'S', 'v', 'b', 'p', '4', 'S', 'a', 'E', 'c', 'm', 'm', '9', '6', 'l', 'y', 'k', 'D', 'G', 'N', '1', 'p', 'C', 'w', 'Y', 'P', 'm', 'M', 's', 's', 'X', 'p', 'A', 'G', 'E', 'n', 'A', 'U', 'C', 'D', 'w', 'o', 'p', 't', 'i', 'O', 'i', 'J', 'Q', 'm', 'U', 'H', 'P', 'W', '3', 'n', 'k', 'e', 'x', 'S', 'K', 'K', '5', 'I', 'P', '9', 't', 'N', 'T', 'v', 'Q', 'D', 'X', '3', 'x', '6', 'I', 'T', 'D', 'W', '5', 'X', 'b', 'I', 'r', 'T', 'v', 'R', 'x', 'j', 'i', '7', 'r', 'd', 'D', '4', 'b', 'w', 'I', 's', 'i', 'Y', 'u', '5', 'S', 'f', 'z', 'Y', 'y', '2', 'u', 'r', 'P', 'L', 'x', '7', 'e', 'i', 'M', 'P', 'F', 'u', 'K', 'v', 'O', 'N', 'p', 'n', '8', 'v', 'V', 'g', 'K', 'F', 'Z', 'a', 'k', 'P', 'y', 'W', 'H', 'i', 'B', 'V', 'r', 'y', 'Q', 'K', 'V', 'c', 'Z', 'Y', 'N', 'k', 'H', 'Z', 'v', 'v', 'C', 't', 'g', 'V', 'N', 'o', 'Z', 'n', 'E', '7', '0', '0', 'S', '5', 'W', 'R', 'O', 'E', 'G', 'e', 'O', 'Z', '7', 'l', 'v', 'I', 'J', '3', 'f', '5', 'm', 'f', 'm', 'I', '0', '0', 'v', 'N', 'b', 'z', 'L', 'R', 'q', 'b', 'K', 'C', 's', 'Z', 'E', 'w', '3', 'S', 'L', 'y', '2', '5', '4', 'B', 'y', 'Z', '6', 'Y', '4', 'j', 'g', 't', 'X', 'R', '4', 'y', 'F', 'D', 'D', 'v', 'e', 'N', 'v', 'N', 'k', '0', '9', 'c', 'G', 'J', 'O', '8', 'C', 'J', 'A', '0', 'g', '4', 'O', 'Z', 'd', 's', 'G', '0', '8', 'A', 'm', 'D', 'E', 'p', 'w', 'i', '1', 'f', 't', 'b', 'V', 'r', '3', 'a', 'y', 'I', 'Y', '0', 'P', 'Y', 'o', 'v', 'R', '0', 'K', 'u', 'g', 'Q', 'j', 'e', 'P', 'J', 'f', 'T', 'z', 'P', 'z', 'q', 'v', 'j', 'F', 'G', 'O', '7', 'e', 'C', 'D', '3', '0', 'S', 'P', 'e', 'D', 'g', 'S', 'M', 'P', 'y', 'a', 'w', 'T', 'p', '5', 'w', '8', 'u', 'C', 'w', 'Y', 'x', 'T', 'e', '1', 'h', 'Z', 'W', 'h', 'c', 'P', 'X', 'u', 'c', 'z', 'v', '9', 'H', '5', 'O', '5', 'W', '8', 'M', '9', '3', '9', '8', 'o', 'B', 'u', 'A', 'y', 'B', 'c', 'n', 'i', '1', '9', 'D', 'T', 'M', 'Y', 'B', 'p', 'N', 'x', 'm', 'S', 'q', 'y', 'L', 'A', 'w', 'W', 'A', 'p', 'V', '8', 'R', 'U', 'q', 'S', 'G', 'p', 'u', 't', 'N', 'm', 't', 'Q', '5', '3', 'o', '5', 'g', 'z', 'q', 'I', 'F', 'W', '6', 'Q', 'X', 's', 'm', 'X', 'W', '5', 'T', 'n', 'P', '7', '5', 'V', 'm', 'q', 'F', 'x', 'Q', 'w', 'D', 'J', 'n', 'p', 'E', 'K', 'c', 'u', 'S', 'h', 'H', 'N', 'v', 'c', '3', 'W', 'x', 'P', 'R', 'G', '1', 'f', 'B', 'w', '0', 'n', 'a', '3', 'Y', 'q', 'n', 'Z', '0', 'R', 'E', '4', 'Z', 'e', 'n', 'r', '9', 's', '2', 'u', 'K', 'w', 'E', '7', '9', 'U', 'B', '0', 'z', 'C', 'l', 'n', 'n', 'B', 'f', 'B', 'q', 'G', 'Y', 'e', 'V', 'c', 'W', 'u', '4', '9', 'a', '3', 'r', 'S', 'l', 'z', 'C', 'o', 'u', 'C', '8', 'v', '0', 'v', 'v', 'b', 'W', 'W', 'b', '1', 'V', 'F', '8', 't', 'j', 'T', 'J', '5', 'C', 'D', '5', 'c', '4', 'k', 's', 'e', 'a', 'v', 'I', 'K', 'v', 'G', '4', 'j', 'z', 'n', '9', 'L', '8', 'Y', 'D', 'T', 'd', '9', 'B', 'C', 'Q', 'I', '7', 'Q', 'K', '0', 's', 'E', 'b', 'Z', 'i', 'z', 'I', 'o', '7', '3', 'u', 'z', 'a', 'h', 'd', 'Z', 't', 'Z', 'v', 'u', 'q', 'O', 'r', 'r', 'O', '7', '0', 'M', 'O', '8', 'A', '4', 'C', 'Z', 'T', 'j', 'O', 'Z', 'N', 'K', 'S', '6', '9', 'r', 'd', 'a', 'E', 'K', 'z', '0', '4', 'g', 'E', 'l', 'L', 'T', 'h', '9', 'd', 't', '7', 'b', 'l', '8', '1', '3', 'f', 'D', 'S', 'q', 'N', 'l', 'm', 'L', 'Q', 'E', 'l', 'U', 'O', 'Z', 'K', 'H', '5', 'N', 's', 'G', 'e', 'N', 'E', '7', '6', 'B', 'Y', 'i', '9', 'N', 'b', 'e', 'R', 't', 'L', 'c', 'S', 'v', 'n', 'i', 'x', 'N', '1', 'A', 'm', '9', 'H', 'f', 'M', 'x', 'j', 'q', 'B', 'n', 'l', '5', 'p', '8', 'd', 'm', 'L', '2', 'F', 'a', 'k', 'Q', '1', '2', '9', 'c', 'Z', 'x', 'p', 'O', 'v', 'P', 'N', '0', 'u', 'a', 'n', 'S', 'E', 'm', '5', 'q', 'i', 'i', 'm', '9', 'L', 'x', '0', 'O', 'M', 'a', 'e', 'D', 'Q', 'b', 'd', 'n', 'O', 'J', '1', '7', 'W', 'F', 'x', 'F', 'd', 'b', 'X', 'f', 'B', 'Q', 'L', 'h', 'z', 'x', 'f', '8', 'j', 'V', 'M', 't', 't', 'o', 'w', '9', 'F', 'P', 'n', 'S', 'W', 'c', 'N', 'r', 'h', 'B', 'w', '8', 'a', 'H', 'e', 'b', 'X', 'n', '9', 'K', 'Z', 'c', 'G', '6', 'v', 'Q', 'p', 'e', '2', 'Z', 'b', '6', 'm', 'C', 'O', '7', 'f', 'b', 'o', 'A', 'a', 'Y', '9', '0', 'f', 'd', 'S', '0', 'E', 'P', 'B', 'd', 'p', 'F', 'Z', 'a', 'U', 'C', '4', 'M', 'b', 'T', 'I', 'E', 'W', 'U', 'B', '1', 'u', 'n', '1', 'K', 'B', '0', 'B', 'g', 'R', 'R', 'U', 'U', 'g', 'T', 'x', 'L', 'Z', 'L', 'j', 't', 'N', 'd', '5', 'p', 'w', 'C', 'r', 'G', 'W', 'q', '7', 'G', 'T', 'x', 'R', 'U', 'l', 'S', 'y', '1', 'h', 'I', 'L', 'D', 'a', '8', 'M', 'z', 'H', 'v', 'g', 'V', 'M', 'Z', '8', '6', 'b', 'n', 'D', 'y', 'T', 'A', '2', 'k', 'v', 'J', '2', 'X', 'Z', 'q', 'O', '6', 'w', 'Z', '7', 'M', 'V', 'H', '9', 'd', '2', 'd', 'w', 'C', '3', 's', '9', 'U', 'V', 'C', 'G', 'o', 'A', '9', 'M', 'w', 'G', 'E', 'H', 'e', 's', 'V', 'a', 'e', 's', 'W', 'p', 'D', 'd', 'm', 'g', 'U', 'G', 'Q', 'W', '9', '8', 'V', 'R', 'T', 'V', 'Y', '5', 'U', 'V', 'H', 'E', 'b', 'L', 'L', '5', '1', 'e', 'U', 'W', 'M', 'o', '9', 'P', 'H', 'j', 'u', 'Z', 'p', 'A', 'w', 'm', '9', 'F', '4', 'Q', 'Z', 'Q', 'M', 't', 'j', 'R', 'x', '8', 'b', '6', '4', 'S', 'a', 'M', 'm', 'm', '0', 'j', 'z', '5', 'J', 'j', 'V', 'y', 'k', 'F', 'Z', 'h', 'I', 'R', 'x', 'h', 'f', '7', 'Q', 'E', 'P', 'B', 'C', 'o', '7', '4', '6', 'V', 'G', 'j', '7', '4', 'I', 'x', 'y', 'R', 'U', 'H', 'E', '4', 'M', 'b', 'Z', 'U', 's', 'L', '2', 'N', 'G', 'G', 'R', 'T', 'G', 'T', '7', 'D', 'M', '4', 'Z', 'S', 'b', 'U', 'L', 'j', 'H', '9', 'z', 'c', 'G', '1', 'U', 'Q', 'S', 's', 'i', '8', '1', 'Y', 'L', '7', 'f', 'a', 'R', 'l', 'r', 'O', 'm', '3', 'G', '9', 'K', 'h', 'U', 'V', 'F', 'Z', 'S', '2', 'z', 'W', 't', 'I', 'm', '9', '0', 'K', '7', 'r', 'h', 'G', 'm', 'A', 'g', '3', 'l', 'L', 'G', 'L', 'F', 'M', 'L', 'P', 's', 'd', 'i', 'v', '0', '0', 'n', 'q', 'M', 'U', 'w', 'w', 'T', 'k', '4', 'O', 'P', 'b', 'S', '1', 'b', 'y', 'v', 'A', '9', '1', 'K', 'C', 'B', 'T', 'R', 't', 'w', 'x', 'c', 'k', 'n', 'F', 'O', 'x', 'Z', 'A', 'h', 'q', 'k', 'c', '2', 'z', '1', 'K', 'r', 'T', '6', 'a', 'R', '4', 'R', 'b', '4', 'Q', 'u', 'K', '8', 'e', '7', 'a', 'F', 'j', 'g', 'R', 'W', '3', 'S', '4', 'i', '0', 'U', 'Y', 'q', 'L', 'h', 'V', 'c', 'b', 'v', 'u', 'V', 'A', 'J', 'O', 'Q', '2', 'Y', 'O', 'W', 'V', 'n', 'Z', '4', 'H', 'r', 'P', 'A', '7', 'J', 'g', 'x', 'b', '3', 'd', 'm', 'a', 'w', 'F', 'z', 'h', 'x', 'L', 'f', '6', 'Z', 'w', 'y', 'v', '8', 'J', 'G', 'l', 'g', '9', 's', 'L', 'O', 'r', 'I', 'V', 'L', '6', 'w', 'C', 'X', 'Y', 'c', 'I', 'b', 'S', 'p', 'O', 'b', 'L', 'J', 'y', '5', '5', 'j', '1', 'E', 'n', 'd', 'j', 'm', 'J', 'u', '0', 'y', '3', 'J', '8', 'x', '5', 'A', 'I', 'S', 'd', 'Q', 't', 't', '4', '8', 'I', 'D', 'F', '4', '8', 'A', 'b', 'z', 'D', 'E', 'Q', 'm', 'p', 'Y', 'W', 'd', 'M', 'N', 'n', 'K', '8', 'i', 'L', 'H', '0', 'm', 'V', 'k', 'T', 'P', 'g', 'b', 'S', 'j', 'T', 'O', 'j', 'u', 'E', 'm', 'y', 'S', 'M', 'b', 'q', 'j', '5', '1', 'u', 'i', 'B', 't', 'E', 'K', 'y', '5', 'u', 'J', 'd', 'D', 'X', '7', 'c', 'p', 'g', 'v', '2', 'D', 'd', '4', 'o', 'S', 'M', '0', 'r', '1', 'X', 'm', 's', 'I', 'K', 't', 'z', 'N', '2', 'n', 'G', 'm', 'w', 'j', 'q', 'J', 'e', 'G', 'x', '8', '2', 'p', 'B', 'V', 'j', 'n', 'l', 't', 'e', 'R', 'k', '1', '3', 'S', '7', 'C', 'A', 'w', 'P', '2', 'Z', 'V', 'e', 'K', '2', 'I', 'R', 'W', 'P', 'E', 'S', 'F', 'r', 'R', 'Y', 'O', '5', '8', '6', 'Z', 'P', 'g', 'O', 'G', 'x', 'M', 'I', 'v', '6', 'V', 'n', 'U', 'q', 'F', 'e', 'i', 'M', 'v', '2', 'Z', 'y', 'I', 'e', 'd', 'a', '0', 'q', 'V', 'y', 'm', 's', 'B', 'G', '4', 'I', '3', 'E', 'O', 'o', 'B', 'k', 'P', 'V', 'Q', 'T', 'x', 'x', 'f', 'g', 'n', '4', 'S', 'w', 'W', 'w', 'K', 'M', 'c', 'd', '9', 'E', 'J', 'A', 'K', 'D', 'G', 'C', 'G', 'V', 'o', 'H', '3', '4', 'a', 'H', 'L', 'L', '4', 'q', 'r', 'g', 'k', 'A', 'Q', '6', 'u', 'Y', 'G', 'K', '1', 'F', 'M', 'A', 'E', 'U', 'B', 'I', 'W', 'H', 'b', '9', 'M', 'U', '3', 'k', 'c', 'E', 'v', 'U', 's', 'b', '0', 'Q', 'Z', 'E', 'K', 'H', 'c', 'Q', 'R', 'T', 'U', 'd', 'R', 'Y', 'x', 'T', 'g', 'I', 'Y', '6', 'H', 'l', 'Q', '8', 'J', 's', 'A', '5', 'D', 's', 'U', '1', '8', 'r', '4', 'I', 'y', 'W', 'Y', 'F', 'o', 's', 'g', '3', 'E', 'T', 'M', 'j', 'b', 'j', 'f', 'i', 'K', 'v', 'e', 'R', 'c', 'e', 'K', 'd', 'M', 'e', 'S', 'K', 'J', 'M', 'Q', '7', 'h', 'n', 'A', 'L', 'T', 'g', 'E', 'Y', 'y', 'P', '5', 'P', 'w', 'a', 'w', '6', 'J', 'Q', 'N', 'j', 't', 'W', 'C', '5', '0', 'V', 'E', 'A', 'f', 'U', '5', 'C', '5', '6', 'L', 'P', 'a', 'O', 'l', 'O', 'b', 'e', 'c', 'N', '4', 'M', 'H', 'B', 'a', 'S', 'l', 'J', 'o', 'l', 'F', 'f', '4', 'H', 'd', 'Y', 'Z', 'Y', 'a', 'V', 'S', 'k', 'k', 't', 'y', 'J', '5', 'P', 'n', 'X', 'a', 'f', 'j', 'h', 'e', '8', 'z', 'D', 'H', 'c', 'p', 'K', '7', 'h', 'R', 'Y', '5', 'r', 'u', 'W', 'A', 'B', '4', 'i', 'u', 's', 'r', 'p', '5', 'S', 'B', 'V', 'x', 'i', '0', 'R', 'g', 'n', 'V', 'o', 'F', '8', 'w', 'A', 'f', 'E', 'Y', 'Z', 't', 'H', 't', 't', 'I', 'n', 'P', '0', 'V', '6', 'd', 'O', 'O', 'e', 'h', '9', 'B', 'X', 'O', 'h', 'A', 'h', 'J', 'G', 'd', '6', 'E', '9', '8', 'd', 'W', 'p', 'i', 'F', 'W', 'q', 's', 'l', 'e', 'B', 'f', '7', 'P', 'r', 'a', 'u', 'q', 'b', 'H', '2', '6', 'F', 'a', 'G', 'J', '1', 'A', 'O', 'y', '6', 'p', 'K', 'l', 'N', 'P', '5', '1', '7', 'g', 'V', '8', '9', 'Q', 'L', 'q', 'o', '5', 'U', 'D', 'A', 'M', 'A', 'G', 'k', 'E', 'N', 'c', 'C', 'b', 'Q', '8', 'E', 'b', 'h', 'R', 'o', 'a', 'G', 'm', '4', 'Z', 'i', '3', 'q', 'u', 'j', 'S', 'n', '3', 'W', 'o', 'D', 'U', 's', 'o', 'W', '5', 'E', 'Y', 'V', 'S', 'U', 'X', 'r', 'z', 'n', 'W', 'P', 't', '6', 'J', 'J', 'e', 'C', 'x', 'M', 'j', 'F', '0', 'c', 'Z', 'c', 'f', 't', 'K', 'H', 'D', 'D', 'L', 'b', 'W', 'e', 't', 'u', 'L', 'i', 'X', 'f', 'v', 'E', 'b', '4', 'N', '6', '4', '8', 'G', 'd', 'C', '6', '3', 'b', 'W', 'Z', 'J', 'g', 'e', 'M', 'i', 'q', 'm', '4', 'I', 'V', 'm', 'R', '2', '7', 'x', 'n', 'C', 'M', 'g', 'N', 'G', 'a', 'M', 'N', '2', 'O', 'H', 'v', 'n', 'e', 'I', 'w', '8', 'l', '7', 'g', 'P', 'j', 'Z', 'X', '4', 'B', 'm', 'u', '9', 'X', 'Y', '9', 'k', '4', 'M', 'q', 'S', 'W', '1', 'I', 'i', '9', '2', 'M', 'b', '8', '6', 'X', 'j', '1', '4', 'y', 'a', 'R', 'J', 'T', 'Q', 'w', 'D', 'N', 'H', 'Z', 'M', 'r', 'T', 'Z', 'W', 'k', 't', 'L', 's', 'S', 'I', 'i', 'c', 'j', 'h', 'Y', '5', 'E', 'O', 'z', '3', 'o', 'E', 'A', '5', 'U', 'w', '7', 'i', '2', 'W', 'u', 'h', 'o', 'H', '1', 'O', '1', 'D', '4', 'H', 'L', 'd', 'j', 't', '8', '5', 'o', 'C', 'J', 'b', '5', 'w', 'f', '4', 'r', 'z', 'q', 'o', 'V', 'g', '9', 'G', 'E', 'n', 'L', '5', '1', 'A', '8', 't', 'H', 'I', 'K', 'q', 'z', 'I', 'k', 'b', 'J', 't', '2', 'C', 'f', 'V', '6', 'L', 'I', 'l', 'x', 'e', 'H', 'w', 'i', 'L', 'Z', 'r', 'F', 'O', 's', 'B', '6', 'z', 'J', 'G', 'e', '6', 'N', 'E', 'X', 'W', 'v', 'N', 'Y', 'P', 'i', 'V', 'a', 'r', '6', 'L', 'J', 'D', '6', 'p', 'N', 'T', 'W', 'S', 'g', 'E', 'T', 'c', '2', 'b', 'g', 'W', 'X', 't', 'Y', 'v', 'F', 'J', '8', 'c', 'Y', 'f', 'x', 'w', 'M', 't', '8', 'T', 'u', '4', '6', '7', 'L', 'S', 'N', 'r', 'V', 'h', 'J', 'N', '8', 'q', 'h', 'T', 'Z', '5', 'E', 'd', 'E', 'B', '5', 'b', 'g', 'q', 'N', 'q', 'Z', 'J', '9', 'H', 'B', '5', 'D', 'N', 'L', 'Q', '4', 'g', 'v', 'C', 't', 'r', 's', 'O', 'B', 'F', 'J', 'F', 'g', 'M', 'G', 'b', 'n', 'm', 'F', 'y', 'S', 'c', '8', 'P', 'k', '7', 'I', 'n', 'K', 'U', '1', 'D', '0', 'n', 'F', 'i', 'U', 'v', 'w', 'V', 'y', '6', 'Z', 'V', 'G', 'd', 'u', 't', 'E', '0', 'f', 'W', 'S', 'd', 'k', '0', 'Z', 's', 'b', 'j', 'C', 'T', 'k', '1', '4', 'n', 'Z', 'M', 'Y', 'J', 'i', 'L', 'F', '5', 'g', 'L', 'Y', 'O', '2', 'c', 'E', 'Y', 'x', 'V', 'z', 'X', 'L', 'M', 'D', 'l', 'u', 'E', '4', 'U', '5', 'w', '8', 'S', '8', 'U', 'b', 'e', 'f', 'h', 'Z', '9', 'q', 'w', 'O', 'h', 'M', 'Q', '3', '7', 'j', 'q', 'S', 't', '3', 'W', 'U', 'n', 'a', 'M', '5', 'T', '8', '1', 'l', '6', 'M', 'C', 'Z', 'p', 'h', 'w', 'p', 'M', 'g', '1', 'r', 't', 'H', 'k', 'o', 'p', 'O', '7', 'Y', 'H', 'R', 's', 's', 'p', '4', 'o', '8', '0', 'f', 'h', 'u', 'p', 'h', 'J', 'T', 'F', '6', '8', 'P', 'a', 'x', '6', 'H', '2', 'e', 'w', 'h', 's', 'r', '5', 'y', '8', 'n', 'e', 'l', 'f', 'I', 'h', 'V', 'm', 'E', 'G', 'P', 'l', 'N', 'i', 'o', 'H', 'g', '1', 'i', 'R', 'x', 'p', 'K', 'P', 'b', 'r', '6', 'G', 'l', 'u', 'E', 'M', 'O', 'p', 's', 'V', 'K', 'B', '5', 'O', 'F', 'K', 'y', 'T', 't', 'a', 'a', 'N', 'S', '6', 'f', 'D', 'm', 'n', 'R', 'B', 'S', 'N', 'I', '3', '5', 'K', 'G', 'J', 'y', 'w', 'c', '8', 'v', 'X', 'L', '1', 'i', '7', 'K', 'P', 'V', 'i', 'c', 'l', 'd', '5', 'n', 'D', 'i', '4', 'E', '1', 'H', 'K', 's', 'C', 'T', 'w', 'J', 'F', 'i', 'k', 'B', 'S', '7', 'K', 'J', 'd', 'H', 'R', 'q', 'b', 'x', 'J', 'C', 'Q', 'C', 'p', 'T', 'l', 'h', 'W', 'a', 'o', 'e', 'G', 'o', 'x', '3', 'w', '0', 'Z', 'W', '2', 'r', 'R', 'A', '0', 't', 'G', 'H', 'X', 'h', '5', 'e', 'j', 'J', 'h', 'N', 'd', 'G', 'u', 'z', 'g', 'X', 'T', 'm', '9', 'F', 'd', 't', '3', '3', 'D', 'v', 'k', 'T', 'w', 'Z', 'A', '2', 'e', 'X', 'X', 'X', '2', '4', 'h', 'Z', 'H', 'A', 'f', '0', 'x', '9', 'L', 'E', 'l', 'K', 'J', 'F', '2', 'D', '6', '5', 'w', 'e', 'O', 'g', '3', 'O', 'W', 'Y', 'm', 'r', 't', 'e', 'm', 'Q', '1', 'r', 'O', 'W', 'h', 'B', 'V', 's', 'E', '4', '1', 'N', '7', 't', 'O', '3', 'm', '9', 'Y', '1', 'd', 'R', 'F', 'y', 'n', 'p', 'f', 'V', 'H', 'H', 'l', '8', 'z', 'y', 'U', 'U', 'x', 'n', 'B', '1', 'i', '2', 'D', 'f', 'j', 'P', 'Z', 'L', 'M', 'v', 'C', 'q', 'A', 'G', 'e', 'm', 't', '9', '7', '1', 'H', 'g', 'z', '4', 'U', 'K', 'O', 'G', 'v', 'P', '7', 'T', 'F', '8', 'x', 'o', 'O', 'K', 'y', 'a', '5', 'y', 'E', '3', '2', 'i', 'f', 'm', 'g', 'c', 'd', 'l', '9', 'S', 'f', 'R', 'a', 't', 'X', 'M', '9', 'T', 'd', 'C', 'P', 'Q', 'r', 'd', 'Y', 'd', '2', 'R', 'R', '4', 'L', 'K', 'd', 'q', 'w', '7', 'H', 'N', 'i', 'G', 'f', 'D', 'V', '4', 'u', 'r', 'E', 't', '8', 'h', 'w', 'N', 'w', 'b', 'r', 'K', '4', 'h', 'c', 'J', 'b', 'l', 'R', '2', 'R', 'B', 'z', 'Y', 'O', 'X', 'c', 'u', 'Y', 'y', 'm', 'J', 'd', 'q', '2', 'b', 'N', 'm', 'o', '7', 'B', 'T', 'G', '3', 'y', 'g', 'D', 'O', 'H', 'U', 'G', 'Y', 'U', '4', 'v', 'i', 'R', 'N', 'Q', 'q', '9', '2', 'z', 'c', 'i', 'p', '1', 'u', 'Q', 'g', 'r', 'P', 'x', 'x', 'J', 'm', 'T', 'K', '0', 'a', 'c', '4', 'z', 'w', 'y', 'i', 'V', 'E', 'v', 'l', 's', 'v', 'c', 'f', 'L', 'A', 'K', 'D', 'u', 'Y', 'h', 'c', 'm', 'S', 'N', 'v', '4', 'h', '3', 's', '7', 'T', 'n', 'u', 'G', 'Z', 'S', 'Z', 'd', 'C', '8', 'L', 'v', 'a', 'q', '4', 'b', 'z', '5', 'J', 'L', 'c', 'j', 'x', 't', 'x', 'g', 'n', 'U', 'Z', 'U', 'P', 't', '5', '8', 'x', 'U', 'O', 'K', 'y', 'Q', 'I', '7', '9', 'h', 'o', '4', '8', 'b', 'z', 'F', 'm', 'Q', 'p', 'Y', '9', 'c', '4', 'k', 'u', 'U', '4', '9', 'B', 'z', '7', 'w', 'I', 'K', '6', '6', 'Y', 'C', '1', 'Y', 'j', 'f', 'S', 'f', '4', 'F', 'l', 'f', 'V', 'Q', '3', 'U', 'q', 'x', '3', 'a', 'F', 'x', 'X', 'E', 'l', 'T', '0', 't', 'd', 'u', 'n', '1', 'x', 'e', 'P', 'W', '8', 'f', '2', '2', 'l', 'b', 'X', '6', 'r', 'O', 'P', 'V', 'C', 'I', 'v', 'H', '3', 'I', 'K', 'c', 'b', 'A', 'J', '2', 'v', 'w', 'u', 'g', 'P', '7', '2', 'L', 'd', 's', 'D', 'E', 'J', 'Z', '8', 'y', 'l', 'N', 'J', 'n', 'T', '2', 't', 'L', '9', '3', 'n', 'Y', '1', 'x', 'R', 'k', 'h', '9', 'G', 'u', '6', '9', '5', 'Z', 'r', '7', 'b', 'z', 'U', 'a', 'n', '5', 'l', 'x', 'j', '5', 'n', 'Q', 'G', 'm', 'H', 'r', 'B', '8', 'e', 'Q', 'h', 'A', 'P', 'n', 'u', 'L', 'k', 'q', 'j', 'Q', 'n', '8', 'D', '7', 'Y', 'q', '0', 'A', 'd', 'Z', '3', 'H', 'n', '7', 'r', 'u', 'o', 'q', 'r', 'H', '6', 'O', 'F', 'L', 'z', 'z', 'V', 'Y', 'd', '2', 'm', 'E', '0', 'p', '9', 'P', 'V', '0', 'N', 'x', 'P', 'E', '2', '1', 'B', 'h', 'l', 'n', 'O', 'S', 's', 'K', 'e', 'x', 'U', 'S', 'l', 'p', 'q', 'E', 'h', 'Q', 'H', 'X', '5', 'G', 'k', 'O', '6', 'v', '9', 'K', 'z', '1', 'B', 'y', 'Z', 'k', 'c', 'n', '1', 'I', 'v', 'V', '6', 'F', 'l', 'f', 'u', 'R', 'h', 'P', 'f', 'o', 'n', 'Y', 'v', 'L', 'n', 'p', '7', 'k', 'z', 'w', 'c', 'y', 'l', 'z', 'Z', 'B', 'a', 'O', 'H', 'J', 'j', 'D', 'P', 'L', 'g', '7', 'a', 'E', 'N', '4', 's', 'y', 'S', 'b', '9', '3', 'H', '6', 'e', '4', 't', '4', 't', 'S', 'r', 'G', 'R', 'H', 'U', 'Y', 'Q', '1', 'a', 'd', 'A', '6', 'b', 'Z', 'A', 'o', 'T', 'r', 'c', 'j', 'I', 'b', 'd', 'N', 'Y', '7', 'I', 'F', 'z', 'z', 'V', 'g', '3', 'm', 'n', 'N', '8', '1', 'C', 'Y', 'V', 'D', 'T', 'u', 'a', 'R', 'Y', 'r', '8', 'y', 'Q', 'H', 'P', 't', 'U', 'l', 'o', 'a', 'o', 'd', 'P', '9', '9', 'I', 'j', 'k', 'T', 'i', 'a', 'W', '4', 't', 'X', 'N', 'd', 'v', 'f', '1', 'c', 'd', 'p', 's', 'i', '2', 'a', '1', 'c', 'C', 'R', 'G', 'e', 'f', 'G', 'd', 'n', 'p', 'C', '5', 'N', 'c', 'P', 'G', 'L', 'k', 'T', 'm', 'V', 'w', 'd', 'v', 'O', 'G', 'b', 'w', '6', 'z', 'l', 'Y', '2', '3', 'f', 'W', 'W', 'j', 'x', '7', 'O', 'z', '2', 'a', 'P', 'F', 'g', 'Y', 'p', 'x', 'A', '8', 'i', 'b', 't', 'w', 'h', 'J', 'g', 'c', '8', 'I', 'y', 'y', '9', 'T', 'I', 'V', '0', '6', 'S', 'F', 't', 'L', 'd', '9', 'Q', '7', 'X', '4', 's', 'W', '2', 'Q', 'x', 'k', 'C', 'T', 't', 'h', 'v', 'p', 'p', 'j', 'e', 'o', '1', 'm', '7', 'r', 'g', 'P', 'u', 'Q', 'Y', 'L', 'N', 'n', 'I', 'i', 'h', 'z', '4', 'X', 'F', 's', '7', 'G', 'B', 'o', 'n', 'u', 'U', 'Q', 'R', 'w', '2', 'I', 'Y', 'z', 'y', '5', 'E', 'i', 'J', 'd', 's', 'W', 'G', 'y', '4', 'l', 'n', 'z', '8', 'r', 'f', '6', 'v', 'g', 'k', 'W', 'O', '2', 'm', 'f', 'o', 'e', 'o', 'B', 'R', 'a', '4', 'V', '8', 'D', 'w', 'o', 'X', '2', 'd', 'R', 'b', 'G', 'E', 'a', 'v', 'j', 'W', 'g', 'D', '4', '0', 'R', 'v', 'a', 'v', 'Z', '2', 'X', 'Y', 'J', 'x', 'S', 'f', 't', 'T', 'P', 'Y', 'q', 'G', '1', '8', 'h', '5', 'A', '7', 'r', 'i', 'T', 'L', 'j', 'M', 'C', 'z', '7', 'c', 'k', 'U', 'V', '7', 't', 'e', 'u', 'B', '7', 'd', 'V', 'N', '1', '9', 'T', 'q', '5', '0', 'm', '6', 'w', 'R', 'e', 'D', 'a', 'B', 'P', 'c', '0', 'K', '3', 'a', 'f', 'M', 'X', 'O', 'o', 'F', 'N', 'k', 'j', 'g', 'x', 'Y', 'g', 'E', 'F', 'b', '2', 'p', 'V', 'o', '4', 'x', 's', 'U', 'z', '5', 'v', 'p', 'G', 'o', 'G', 'l', 'y', 'b', 'x', 'c', 'h', '8', 'C', 'E', 'd', 'x', 'a', '7', 'z', 'd', 'Y', 's', 'I', 's', 'U', 'A', 'f', 'A', 'V', 'S', '6', 'G', '8', 'C', 's', 'C', 'l', 'g', 'b', 'W', '6', '6', 'U', '7', 'A', 'v', 'u', 'Z', 's', 'i', '2', 'F', 'O', 'B', 'x', 'i', 'B', 'Q', 'h', 'U', 'j', 'd', 'a', 'h', 'f', 'I', 'h', 'E', 'm', '8', 'a', 'j', '3', 's', 'g', '3', 'e', 'O', 'S', 'M', 'u', 'J', 'P', '8', 'I', 'C', 'f', 'J', 'T', 'C', 'c', '0', 'f', '2', 'X', '8', '9', '2', 'C', 'l', 'y', 'c', 'I', 'r', 'K', 'm', 'j', 'm', '1', 'z', 'x', 'l', '8', 'C', 'i', 'G', 'D', 'D', 'O', 'W', 'D', 'q', 'M', 'i', 'g', 'h', 'e', 'd', 'X', 'g', 'F', 'L', '6', 'L', '1', 'E', 'y', 'a', 'r', 'p', 'N', 'e', 'O', 'M', 'e', 'w', 'Q', 'h', 'x', 'e', '1', '0', 'I', 'B', 'W', 'p', 'i', '0', 'I', '3', 'U', 'L', 'D', 'Q', 'X', '4', 'V', 'L', 'U', 'C', 'y', 'h', 'e', 'A', 'r', '6', 'v', '8', 'e', 'i', 'c', 'V', 'X', 'l', 'X', 't', 'O', '3', 'i', 'W', 'x', '2', 'g', 'y', 'J', '1', 's', 'e', 'A', 'A', 'e', 'y', 'h', '6', 'w', 'N', '2', 'h', 'L', 'U', 'G', 'l', 'q', 'd', 'K', 'B', 'K', 'Y', '4', 's', 't', 'p', 'l', 'N', 'd', 'u', 'E', 'J', 'M', 'E', 'J', 'o', '0', 'p', 'i', 'm', '2', 'a', 'K', 'B', 'v', 'O', 'm', 'Z', 'p', 'x', 'a', 'x', 'J', 'S', 'g', '0', '6', 'F', 'D', 'X', 'x', 'F', 'h', '9', 'J', 'q', 'l', '9', 'V', 'J', 'k', 'L', 'h', 's', 'M', 'Q', '6', 'z', 'p', 'j', 'k', 'D', 'W', 'r', 'W', '2', 'g', 'Q', '7', 'h', 'n', 'u', 'n', 'I', 'r', 'u', 'z', 'Q', 's', 'I', 'Z', 'S', 'U', '5', 'A', 'g', 'L', '6', 'T', '1', 'd', '1', '2', 'z', 'j', 'M', 'q', 'D', 'c', 'l', 'k', 'E', 'U', 'N', 'M', 'B', '6', '9', 'Q', 'o', 'G', 'p', '6', 'Y', 'k', '6', '4', 'u', '0', 'M', 'j', 'T', 'D', 'b', 'G', 'k', 'o', 'w', 'n', 'E', 'Y', 'O', 'I', 's', 'Z', 'T', 'r', 'V', 'S', '7', '7', 'W', 'l', '3', 'u', 'J', 'x', 'n', '3', 'n', 'z', 'c', '4', '0', '4', '9', 'Z', 'g', 'v', 'C', 'k', 'H', 'O', 'r', 'x', 'm', 'A', 'd', '5', 'Q', 'a', '3', 'n', '9', 'w', 'X', 'J', 'i', '8', 'A', 'J', 'x', 'd', 'E', 'm', 'V', 'B', 'B', 'z', 'u', 'B', 'Z', '2', 'O', 'E', 'n', '0', 'E', 'G', 'v', 'V', 'f', 'o', '6', 'e', 'Z', 'R', 'l', '7', 'Q', 'k', 'G', 'B', 'D', 'I', 'n', 'W', 'H', 'm', 'J', '2', 'o', 'i', 'u', '2', 'n', 'X', 'q', 'p', 'c', 'b', '8', '7', 'E', '4', 'Z', 'b', 'M', 'B', 'W', 'a', 'l', 'a', 'b', 'm', 'h', 'D', '6', 'o', 'p', 'F', 'g', 'R', 'l', 'O', 'H', 'M', 'k', 'x', '0', 'C', 'N', 'y', '7', 'P', 't', 'W', 'o', '3', 'V', 'A', 'T', '6', 'Y', 'u', 'g', '5', 'x', 'a', 'i', 'b', 'd', 'C', 'q', 'F', 'R', 'y', 'P', 'z', 'j', 'F', 'z', 'u', '4', 'w', '7', 'l', 'I', 'l', 'c', 'e', 'j', 'v', 'Y', '8', 'd', '3', '1', 'P', 'T', 'Z', 'o', 'x', 'a', 'U', '0', 'p', 'G', 'D', 'e', 'o', 'J', 'R', 'Y', 'B', 'D', 'W', 'k', 'J', '5', 'C', 'n', 'f', 'v', 'A', 'b', 'O', '3', 'S', 'b', 'K', 'q', 'F', '5', 'E', 'Y', 't', 't', 'e', 'x', 'L', 'I', '6', 'c', 'h', '5', 'f', '1', 'f', 'm', 'w', 'h', 'Q', 'P', 'Q', 'O', 'q', 'e', 'H', '7', '4', 'P', 'l', '9', 'L', 'p', 'X', '4', 'W', '0', 'r', 'h', '8', 'l', '7', 'd', 'e', 'a', 'S', '9', 'C', 'D', 'e', 'S', 'S', 'u', 'g', '7', 'N', 'l', '2', 'H', '1', 'b', 'E', 'A', 'G', 'a', '2', 'a', 'Q', 'k', '7', 'M', 'J', '2', 'o', 'o', 'S', '6', 'l', 'U', '9', 'G', 'l', 'Q', 'y', 'H', 'N', 'B', 'r', 'F', 'G', 'g', 'g', 'J', 'e', 'k', 'j', 'W', '8', 'x', '4', '3', '9', 'E', 'v', 'n', 'q', 'D', 'i', 'R', 'X', 'h', 'V', '8', 'x', 'K', 'E', '8', 'J', 't', 'B', 'O', 'P', 'f', 'X', 't', 'D', '4', 'D', 'B', 'r', '8', '4', 'o', 'A', 'p', 'Q', 'q', 'q', 'y', '5', 'E', 'T', 'R', 'A', 'F', 'Z', 'E', 'B', 'g', 'x', 'D', 'u', 'A', 'g', 'F', 'r', 'k', '9', 'v', 'l', 'o', 'r', 'e', 'R', 'r', 'H', 'h', 'V', 'y', 'T', 'Q', '2', 'k', 'f', '0', 'p', '4', '5', 'r', 'b', 'q', 's', 'J', 'r', 'O', 'P', 'W', 'y', 'M', 'F', 'Y', 'z', 'w', '0', 'G', 'b', '7', 'l', 'w', 'v', '4', 'B', 'l', 'e', 'g', 'c', 'I', 'Z', 'V', 'z', 'y', 'B', 'h', '7', 'q', 't', 'K', 'D', 'g', 'V', 'I', '4', 'K', '3', 'u', 'O', 'U', 'p', 'z', 'H', 'Z', 't', 'G', 'A', 'L', 'm', 'a', 'T', 'B', 'v', 'G', 'z', 'v', 'l', 'v', 'b', 'V', '3', 'c', '1', 'O', 'j', 't', 'W', 'c', 'b', 'k', 'u', 'G', 'X', 'z', 'f', 'G', '5', 'e', 'Q', 'i', '4', 'h', 'h', 'o', 'o', 'V', 'X', 'P', 'E', 'w', 'i', '7', 'P', 'X', 'K', 'w', 'G', 'g', 'M', 'h', 'F', '6', 'm', 'c', 'w', 'F', 'h', 'r', 'j', 'v', 'N', 'c', 'S', 'v', 'E', '6', 'E', 'b', 'K', 'I', 'N', 'q', 'F', 'a', 'E', 'N', 'M', 'K', 'u', 'Y', 'q', 'x', 'V', 'Q', 'N', 'F', 'V', 'u', 'w', '3', 'f', 'A', 'V', 'w', 't', 'Z', 's', 'x', 'y', '0', '5', 'B', 'h', '8', 'a', 'l', 'M', 'm', 't', '4', '9', 'X', 'r', 'S', 'n', '4', 'X', '7', 'n', 'J', '0', 'G', 'H', 'L', '2', '0', 'i', 'j', 'n', 'W', 'Z', 'g', 'X', '6', 'f', 'v', 'f', 'p', 'Y', 'P', 'j', 'V', 'm', 'O', 'n', 'O', 'G', 'A', 'L', 't', 'H', 'B', 'y', 'P', 'M', 'q', 'D', 's', 'N', 'p', 'E', 'n', 'L', 'Z', 'h', 'o', 'L', 'D', 'T', 'h', 'Q', '0', '2', '2', 'E', 'f', 'E', 'J', 'd', 'P', '0', 'i', 'Q', 'o', 'v', 'a', 'T', 'z', 'I', 'g', 'e', 'K', 'J', 'n', 'k', 'p', 'Q', 't', 's', 'j', 'Q', '8', 'Y', 'I', 'y', 'a', 'l', '3', 'j', 'C', 'I', 'a', 'k', 'X', 'E', 'U', 'v', 'V', 'J', '2', '2', 'l', 'C', '9', 'M', 'k', 'o', 'c', 'T', 'U', 'A', 'h', 'S', 'Y', 'p', 'H', 'w', 'O', '8', 'W', 'O', 'E', 'w', 'z', 'Z', 'y', 'H', 'L', 'K', 'O', 'B', 'A', 'z', 'D', '9', '9', 'm', 'm', 'c', '3', '6', 'a', 'b', 'P', 'w', 'E', 'U', 'i', 'T', 'Q', '4', 'h', 'U', 'o', 'U', 'u', 'b', 'Z', '3', 'j', 'n', '4', 'j', 'a', '5', 'g', 'a', 'f', 'I', '0', 'Z', 'F', 'Q', '0', 'S', 'A', '4', 'm', 'i', 'L', '0', 'b', 'r', 'K', 'D', '9', '2', 'e', 'Z', 't', 'B', 'C', 'l', 'k', 'd', 'f', 'H', '3', 'A', 'N', 'r', 'Z', 'S', '7', 'N', 'i', '7', 'I', 'I', 'd', 'R', '8', '4', '8', 'J', '6', '8', '9', 'a', 'V', 't', 'c', 'X', 'U', 'A', 'y', 'z', 'F', 'p', 'y', 'T', 'W', 'N', 'j', 'R', 'b', 'I', 'M', 'h', 'Q', 'q', 'y', 'P', 'i', 'v', 'W', 'e', 't', 'T', '3', 'C', 'C', 'T', 'X', 'V', 'T', 'M', 'I', 'X', '1', '6', 'q', 'M', 'I', 'N', 'b', 'h', 'T', 'o', 'C', 'k', 'S', '0', 'x', 'y', 'l', 'H', 'T', 'S', 'Y', 'K', 'U', 'b', 'd', 'q', 'u', 'u', '2', '0', 'H', 'r', 'v', 'v', '1', '3', '7', 'T', 'Y', 'Q', '7', 'a', 'y', 'N', 'O', 'j', 'B', '0', 'q', 'S', 'G', 'F', 'c', 'Z', 'e', '3', 'F', 'M', 'm', '5', 'A', 'r', 'm', 'v', 'd', 'e', 'o', 'a', 'v', 'A', 'o', 'q', 'a', 'd', '3', 'p', 'C', '2', 'd', 'r', 'L', 'h', 'u', 'l', '4', 'M', 'd', '9', 'Y', 'F', '2', 'Y', 'w', 'f', 'I', 'N', '7', 'u', 'l', 'q', 'v', 'Q', 'W', 'J', 'r', 'P', 'y', 't', 'H', 'P', 'Y', 'Q', 'v', 'I', '0', 'p', 'U', 'T', 'n', 'q', 'W', 'f', 'D', 'G', '8', 'L', 'U', '5', '3', '3', 'j', 'o', 'J', '5', 'y', '0', 'K', 'k', 'i', 'Q', 'y', '6', 'e', 'j', 'D', 'U', 'O', 'V', 'l', '1', 'C', '5', 'X', 'D', 'C', 'V', 'M', 'U', 'P', 'F', 'N', 'y', 's', 'V', 'u', 'g', 'J', '2', 'E', 'p', 'G', '2', 'k', 'l', 'b', 'n', '3', 'q', '8', 'f', 'h', '8', 'Y', '4', 'B', 'a', 'O', 'L', 's', 'd', 'R', '4', 'P', '9', 'N', '9', 'd', 'U', '0', 'h', 'A', '6', 'a', 'k', 'f', '1', 'N', 'Z', 'h', 'J', '2', 'D', 'I', 'Q', '5', 'J', 'p', 'J', 'V', 'X', 'k', 'k', 'R', '0', 'h', 'e', 'x', 'A', 'x', 'n', 'h', 'x', 'i', '5', 'V', 'D', 'x', 'g', 'c', 'S', 'p', 'S', 'V', 'x', 'j', 'R', '5', 'O', 'a', 'O', 'j', '9', 'y', 'z', 'z', 'V', 'T', 'k', 'T', 'E', 'O', 'y', '1', 'u', 'u', 'M', 'x', 'h', 's', 'O', 'x', 'Y', 'g', 'J', 'J', 'D', 'Y', 'E', 'R', 'w', 'S', 'z', 'v', 'E', 'o', 'k', 'X', '5', 'L', 'q', 'A', 'X', 'f', '1', 'H', 'P', 'C', '2', 'u', 's', 'G', 'h', 'E', 'l', 'o', 'O', 'o', 'C', 'Q', '4', 'z', 'j', 't', 'k', 'n', 'V', 'J', '9', 'Q', 'S', 'p', 'Q', 'n', 'I', 'G', 's', 'V', 'I', 'k', 'D', 'y', 'p', 'j', '3', 'O', 'N', 'F', '2', 'Q', 'V', 'u', 'D', '2', 'b', 'l', 'e', 'w'};

  clock_t start, end;
  start = clock(); 
  HuffmanCodes(arr, SIZE);
  end = clock(); 
  // Cálculo do tempo em segundos
  double elapsedTime = (double)(end - start) / CLOCKS_PER_SEC;
  
  // Exibe os resultados
  printf("\nTempo de execucao: %.6f segundos\n", elapsedTime);
  
  return 0;
}