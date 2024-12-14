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

| **Etapa**               | **Melhor Caso**       | **Pior Caso**        |
|-------------------------|-----------------------|-----------------------|
| **Construção da Árvore**| \(O(n^2)\)            | \(O(n^2)\)            |
| **Geração dos Códigos** | \(O(n\log n)\) | \(O(n\log n)\) |

---

