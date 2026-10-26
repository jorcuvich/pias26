#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROD 100       // Limite maximo de produtos
#define MAX_DEP 10         // Limite maximo de departamentos
#define TAM_NOME 50        // Tamanho maximo do texto do nome
#define TAM_DEP 30         // Tamanho maximo do texto do departamento

// --- VETOR DE DEPARTAMENTOS (TABELA DE CONSULTA) ---
char departamentos[MAX_DEP][TAM_DEP];
int total_departamentos = 0;

// --- VETORES PARALELOS DE PRODUTOS ---
char nomes[MAX_PROD][TAM_NOME];
int departamento_produto[MAX_PROD]; // Guarda o INDICE numerico do departamento
int estoques[MAX_PROD];
float precos[MAX_PROD];
int total_produtos = 0;

// --- FUNCOES UTILITARIAS DE TELA ---

// Limpa o terminal tanto no Windows (cls) quanto no Linux/Mac (clear)
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Pausa a execucao para que o usuario possa ler o resultado
void pausar() {
    printf("\nPressione [ENTER] para continuar...");
    getchar();
}

// Le e exibe o logotipo ASCII a partir de um arquivo de texto
void exibir_logo() {
    FILE *arquivo = fopen("logo.txt", "r");

    // Caso o arquivo nao exista, gera um padrao inicial automaticamente
    if (arquivo == NULL) {
        arquivo = fopen("logo.txt", "w");
        if (arquivo != NULL) {
            fprintf(arquivo, "============================================\n");
            fprintf(arquivo, "       SISTEMA INTEGRADO DE LOJA v3.2       \n");
            fprintf(arquivo, "============================================\n");
            fclose(arquivo);
            arquivo = fopen("logo.txt", "r");
        }
    }

    if (arquivo != NULL) {
        char linha[128];
        while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);
        }
        fclose(arquivo);
    }
    printf("\n");
}

// --- FUNCOES DE CADASTRO E LISTAGEM ---

void cadastrar_departamento() {
    limpar_tela();
    exibir_logo();
    printf("--- NOVO DEPARTAMENTO ---\n");

    if (total_departamentos >= MAX_DEP) {
        printf("[AVISO] Limite de %d departamentos atingido!\n", MAX_DEP);
        pausar();
        return;
    }

    printf("Nome do departamento (ex: Alimentos, Games): ");
    fgets(departamentos[total_departamentos], TAM_DEP, stdin);
    departamentos[total_departamentos][strcspn(departamentos[total_departamentos], "\n")] = '\0';

    printf("Departamento cadastrado com o codigo ID: %d\n", total_departamentos);
    total_departamentos++;
    pausar();
}

void listar_departamentos() {
    printf("\n--- DEPARTAMENTOS DISPONIVEIS ---\n");
    if (total_departamentos == 0) {
        printf("(Nenhum departamento cadastrado ainda)\n");
        return;
    }
    for (int i = 0; i < total_departamentos; i++) {
        printf("[%d] %s\n", i, departamentos[i]);
    }
}

void cadastrar_produto() {
    limpar_tela();
    exibir_logo();
    printf("--- CADASTRAR PRODUTO (%d de %d) ---\n", total_produtos + 1, MAX_PROD);

    if (total_departamentos == 0) {
        printf("[ERRO] Cadastre ao menos um departamento antes de adicionar produtos!\n");
        pausar();
        return;
    }

    if (total_produtos >= MAX_PROD) {
        printf("[AVISO] Limite maximo de produtos atingido!\n");
        pausar();
        return;
    }

    printf("Nome do produto: ");
    fgets(nomes[total_produtos], TAM_NOME, stdin);
    nomes[total_produtos][strcspn(nomes[total_produtos], "\n")] = '\0';

    // Mostra os departamentos cadastrados para o usuario escolher pelo numero
    listar_departamentos();
    int cod_dep;
    do {
        printf("Digite o codigo do departamento correspondente: ");
        scanf("%d", &cod_dep);
        while (getchar() != '\n');
        if (cod_dep < 0 || cod_dep >= total_departamentos) {
            printf("Codigo invalido! Escolha um dos numeros da lista.\n");
        }
    } while (cod_dep < 0 || cod_dep >= total_departamentos);

    departamento_produto[total_produtos] = cod_dep;

    printf("Quantidade em estoque: ");
    scanf("%d", &estoques[total_produtos]);

    printf("Preco unitario (R$): ");
    scanf("%f", &precos[total_produtos]);
    while (getchar() != '\n');

    total_produtos++;
    printf("\nProduto cadastrado com sucesso!\n");
    pausar();
}

void listar_produtos() {
    limpar_tela();
    exibir_logo();

    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado ate o momento.\n");
        pausar();
        return;
    }

    printf("============================= LISTA DE PRODUTOS =============================\n");
    printf("%-4s | %-20s | %-16s | %-8s | %-10s\n", "ID", "Nome", "Departamento", "Estoque", "Preco");
    printf("-----------------------------------------------------------------------------\n");

    for (int i = 0; i < total_produtos; i++) {
        int dep_id = departamento_produto[i];
        // Busca o nome do departamento no vetor 'departamentos' usando o indice numerico
        char *nome_dep = departamentos[dep_id];

        printf("%-4d | %-20s | %-16s | %-8d | R$ %8.2f\n", 
               i + 1, nomes[i], nome_dep, estoques[i], precos[i]);
    }
    printf("=============================================================================\n");
    pausar();
}

// --- FUNCOES DE ARQUIVO (PERSISTENCIA) ---

void salvar_dados() {
    limpar_tela();
    exibir_logo();

    // 1. Salva os departamentos
    FILE *arq_dep = fopen("departamentos.txt", "w");
    if (arq_dep != NULL) {
        for (int i = 0; i < total_departamentos; i++) {
            fprintf(arq_dep, "%s\n", departamentos[i]);
        }
        fclose(arq_dep);
    }

    // 2. Salva os produtos com o id do departamento: Nome;ID_Dep;Estoque;Preco
    FILE *arq_prod = fopen("loja.txt", "w");
    if (arq_prod != NULL) {
        for (int i = 0; i < total_produtos; i++) {
            fprintf(arq_prod, "%s;%d;%d;%.2f\n", 
                    nomes[i], departamento_produto[i], estoques[i], precos[i]);
        }
        fclose(arq_prod);
    }

    printf("Departamentos e Produtos salvos com sucesso nos arquivos TXT!\n");
    pausar();
}

void carregar_dados() {
    limpar_tela();
    exibir_logo();

    // 1. Carrega departamentos
    FILE *arq_dep = fopen("departamentos.txt", "r");
    total_departamentos = 0;
    if (arq_dep != NULL) {
        while (total_departamentos < MAX_DEP && fgets(departamentos[total_departamentos], TAM_DEP, arq_dep) != NULL) {
            departamentos[total_departamentos][strcspn(departamentos[total_departamentos], "\n")] = '\0';
            total_departamentos++;
        }
        fclose(arq_dep);
    }

    // 2. Carrega produtos
    FILE *arq_prod = fopen("loja.txt", "r");
    total_produtos = 0;
    if (arq_prod != NULL) {
        while (total_produtos < MAX_PROD && 
               fscanf(arq_prod, " %49[^;];%d;%d;%f", 
                      nomes[total_produtos], 
                      &departamento_produto[total_produtos], 
                      &estoques[total_produtos], 
                      &precos[total_produtos]) == 4) {
            total_produtos++;
        }
        fclose(arq_prod);
    }

    printf("Dados carregados: %d departamento(s) e %d produto(s).\n", total_departamentos, total_produtos);
    pausar();
}

// --- CONTROLE PRINCIPAL DO MENU ---

void executar_menu() {
    int opcao;

    do {
        limpar_tela();
        exibir_logo();

        printf("=== MENU DE OPCOES ===\n");
        printf("1. Cadastrar Departamento\n");
        printf("2. Cadastrar Produto\n");
        printf("3. Listar Produtos\n");
        printf("4. Salvar Dados em TXT\n");
        printf("5. Carregar Dados de TXT\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
                cadastrar_departamento();
                break;
            case 2:
                cadastrar_produto();
                break;
            case 3:
                listar_produtos();
                break;
            case 4:
                salvar_dados();
                break;
            case 5:
                carregar_dados();
                break;
            case 0:
                limpar_tela();
                printf("Programa encerrado com sucesso.\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }

    } while (opcao != 0);
}

// --- FUNCAO MAIN RESTRITA APENAS A CHAMADAS ---

int main() {
    executar_menu();
    return 0;
}