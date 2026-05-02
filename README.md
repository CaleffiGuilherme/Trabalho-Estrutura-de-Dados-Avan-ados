## Trabalho-Estrutura-de-Dados-Avancados

Este sistema implementa um gerenciador de eventos críticos urbanos para uma cidade inteligente. O núcleo da aplicação é uma **Árvore AVL** (Adelson-Velsky e Landis), estrutura de dados de busca binaria auto-balanceada que garante altura O(log n) e operações em tempo eficiente mesmo com grandes volumes de dados.

## Descrição Geral
O sistema monitora diversos tipos de incidentes urbanos:
* **Acidente de trânsito**
* **Falha em semáforo**
* **Interrupção de energia**
* **Alagamento**
* **Incêndio**

Cada evento armazena um ID único (chave da árvore), tipo, nível de severidade (1-5), data/hora, região e status (ATIVO/RESOLVIDO).

## Funcionalidades
* **Inserção com Balanceamento:** Rotações automáticas LL, RR, LR, RL.
* **Remoção Segura:** Apenas eventos com status RESOLVIDO podem ser removidos.
* **Buscas e Relatórios:** * Busca por ID com desempenho O(log n).
  * Listagem por intervalo de severidade.
  * Relatório por região (percurso em-ordem).
  * Busca por intervalo de IDs com poda BST.
* **Métricas:** Exibição de altura, total de nós, eventos ativos e total de rotações.

## Compilação e Execução
### Requisitos
* Compilador GCC (versão 4.8 ou superior).
* Padrão C99 (obrigatório para declarações em blocos case).

### Comandos
```bash
# Compilação (Linux, macOS ou Windows/MinGW)
gcc -std=c99 -Wall -Wextra -o smart_city main.c

# Execução
./smart_city  # (Linux/macOS)
smart_city.exe # (Windows)
