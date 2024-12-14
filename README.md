# Algoritmo de Huffman Embarcado

Este repositório contém uma implementação do **algoritmo de Huffman** em linguagem **C**, adaptada para **sistemas embarcados**. A implementação foi projetada para operar com **arrays estáticos** e **sem recursão**, tornando-a ideal para dispositivos com recursos limitados, como microcontroladores.

---

## 📋 Descrição do Projeto

O **algoritmo de Huffman** é amplamente utilizado para compressão de dados, gerando códigos de comprimento variável com base na frequência de ocorrência dos símbolos. Esta implementação foi desenvolvida com foco em:

- **Eficiência em sistemas embarcados:** sem uso de alocação dinâmica de memória.
- **Simples de implementar e compreender:** usa estruturas iterativas no lugar de recursão.
- **Compatibilidade:** projetada para ser portátil e fácil de adaptar para diferentes plataformas embarcadas.

---

## ⚙️ Funcionalidades

- **Compressão de Dados:** Reduz o tamanho de dados de entrada ao gerar códigos compactados.
- **Estrutura Estática:** Utiliza arrays fixos para representar a árvore de Huffman.
- **Iterativo:** Implementação sem recursão, adequada para dispositivos embarcados com pilha limitada.
- **Exemplo de Compressão:** Gera códigos de Huffman para símbolos e frequências pré-definidos no código.

---

## 💪 Estrutura do Projeto

- `huffman.c`: Implementação do algoritmo de Huffman.
- `README.md`: Documentação do projeto.
- `LICENSE`: Informações sobre a licença do repositório.

---

## 🚀 Como Usar

### Pré-requisitos
- Compilador C (ex.: GCC ou outro compatível).
- Ambiente Linux, Windows, ou uma ferramenta de desenvolvimento para sistemas embarcados.

### Passos

1. **Clone o repositório:**
   ```bash
   git clone https://github.com/SEU_USUARIO/Embedded-Huffman-Encoding.git
   ```
2. **Entre no diretório do projeto:**
   ```bash
   cd Embedded-Huffman-Encoding
   ```
3. **Compile o programa:**
   ```bash
   gcc huffman.c -o huffman
   ```
4. **Execute o programa:**
   ```bash
   ./huffman
   ```

### Resultado
Ao executar, o programa exibirá os **códigos de Huffman** gerados para os símbolos e frequências predefinidos.

---

## 📊 Aplicações

Esta implementação é ideal para compressão de dados em:
- **Dispositivos IoT:** Reduz o tamanho de dados transmitidos por sensores.
- **Armazenamento Compactado:** Minimiza o espaço ocupado por arquivos em sistemas com memória limitada.
- **Redução de Tráfego de Rede:** Otimiza o uso de largura de banda em redes embarcadas.

---

## 📂 Estrutura do Código

### **Árvore de Huffman**
- Representada usando arrays estáticos:
  - Cada nó da árvore é armazenado como uma estrutura contendo peso, índices de filhos e pai.
- Árvore criada iterativamente, combinando nós com os menores pesos.

### **Códigos de Huffman**
- Gerados ao percorrer a árvore de maneira iterativa (sem recursão).
- Cada código é armazenado como uma string para facilitar a compressão.

---

## 📊 Complexidade

A análise de **tempo de complexidade** do **algoritmo de Huffman sem recursão e sem alocação dinâmica de memória** pode ser feita avaliando as etapas principais da sua execução. A ausência de recursão e alocação dinâmica não altera significativamente a complexidade assintótica do algoritmo; essas alterações apenas afetam a eficiência prática (desempenho e uso de memória).

---

### **Etapas do Algoritmo**

O algoritmo de Huffman pode ser dividido em duas etapas principais:

1. **Construção da Árvore de Huffman:**
   - O objetivo é encontrar repetidamente os dois nós de menor peso e combiná-los até restar um único nó, a raiz da árvore.
   - Na versão sem recursão, essa construção é feita iterativamente.

2. **Geração dos Códigos de Huffman:**
   - Cada folha da árvore gera um código de Huffman ao ser percorrida do nó folha até a raiz.

---

### **Complexidade da Construção da Árvore**

#### Melhor Caso:
- No melhor caso, as frequências dos símbolos são ordenadas em ordem crescente. Isso permite que os dois menores valores sejam encontrados imediatamente, sem necessidade de varreduras completas do conjunto.
- Encontrar os dois menores nós, \(n - 1\) vezes, é feito com complexidade \(O(n)\) para cada busca.
- **Complexidade no Melhor Caso:**  
  \[
  O(n \cdot n) = O(n^2)
  \]

#### Pior Caso:
- No pior caso, as frequências dos símbolos não estão ordenadas, e cada busca pelos dois menores valores requer uma varredura completa do conjunto restante de nós.
- Para \(n\) nós iniciais, há \(2n - 1\) nós na árvore final. As buscas \(n - 1\) vezes no pior caso custam \(O(n^2)\).
- **Complexidade no Pior Caso:**  
  \[
  O(n^2)
  \]

---

### **Complexidade da Geração dos Códigos**

#### Melhor e Pior Caso:
- A geração dos códigos envolve percorrer cada folha até a raiz. Como a altura da árvore é \(O(\log n)\) no caso balanceado e as folhas estão associadas a \(n\) símbolos:
  - Cada símbolo é percorrido em \(O(\log n)\).
  - Para \(n\) símbolos, isso resulta em:
    \[
    O(n \cdot \log n)
    \]

---

### **Complexidade Total**

A construção da árvore domina o tempo de execução, pois tem complexidade maior que a geração dos códigos no caso assintótico.

| **Etapa**               | **Melhor Caso**       | **Pior Caso**        |
|-------------------------|-----------------------|-----------------------|
| **Construção da Árvore**| \(O(n^2)\)            | \(O(n^2)\)            |
| **Geração dos Códigos** | \(O(n \cdot \log n)\) | \(O(n \cdot \log n)\) |
| **Total**               | \(O(n^2)\)            | \(O(n^2)\)            |

---

### **Justificativa**

1. **Construção da Árvore (etapa dominante):**  
   - A cada passo, os dois menores nós são combinados, o que exige uma busca linear no conjunto de nós restantes. Isso é repetido \(n - 1\) vezes, resultando em \(O(n^2)\).

2. **Geração dos Códigos:**  
   - É menos custoso do que a construção da árvore, pois cada símbolo percorre um caminho até a raiz com complexidade proporcional à altura da árvore (\(O(\log n)\)). Para \(n\) símbolos, a soma total dos percursos é \(O(n \cdot \log n)\).

---

### **Conclusão**
O tempo de complexidade do **algoritmo de Huffman sem recursão e alocação dinâmica** é o mesmo da versão tradicional:
- Melhor Caso: \(O(n^2)\)
- Pior Caso: \(O(n^2)\)

Isso ocorre porque o gargalo principal do algoritmo está na construção da árvore, onde os dois menores pesos precisam ser encontrados repetidamente.
