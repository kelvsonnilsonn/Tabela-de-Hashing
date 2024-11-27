#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 57

typedef struct carro {
    char placa[8];
    char marca[15];
    char modelo[15];
    char cor[15];
    int status;  // 1 - ativo ou 0 - removido
} CARRO;

typedef struct noTabela {
    char placa[8];
    int posicao;
    struct noTabela* prox;
    struct noTabela* ant;
} No;

FILE* prepararArquivo(char nome[]);
void fecharArquivo(FILE* arq);
int buscar(No* tabelaHashing[], char placa[]);
void cadastrar(FILE* arq, No* tabelaHashing[]);
void consultar(FILE* arq, No* tabelaHashing[]);
void alterar(FILE* arq, No* tabelaHashing[]);
void remover(FILE* arq, No* tabelaHashing[]);
void exibirCadastro(FILE* arq);
void criarIndice(FILE* arq, No* tabelaHashing[]);
void inserirTabelaHash(No* tabelaHashing[], char placa[], int pos);
void removerTabelaHash(No* tabelaHashing[], char placa[], int posTabela);
int hashing(char placa[]);
void exibirOpcoes();
void desalocarIndice(No* tabelaHashing[]);

int main() {
    char nomeArq[] = "carros.dat";
    int op;
    FILE* cadastro;
    No* tabelaHashing[N] = { NULL };
    cadastro = prepararArquivo(nomeArq);

    if (cadastro == NULL)
        printf("Erro na abertura do arquivo. Programa encerrado \n");
    else {
        criarIndice(cadastro, tabelaHashing);
        do {
            exibirOpcoes();
            scanf("%d", &op);
            fflush(stdin);
            switch (op) {
            case 1: cadastrar(cadastro, tabelaHashing); break;
            case 2: consultar(cadastro, tabelaHashing); break;
            case 3: alterar(cadastro, tabelaHashing); break;
            case 4: remover(cadastro, tabelaHashing); break;
            case 5: exibirCadastro(cadastro); break;
            case 0: fecharArquivo(cadastro); desalocarIndice(tabelaHashing); break;
            default: printf("Opção inválida \n");
            }
        } while (op != 0);
    }
    return 0;
}

FILE* prepararArquivo(char nome[]) {
    FILE* arq = fopen(nome, "r+b");
    if (arq == NULL) {
        arq = fopen(nome, "w+b");
        if (arq == NULL) {
            printf("Erro ao abrir ou criar o arquivo!\n");
            return NULL;
        }
    }
    return arq;
}

void fecharArquivo(FILE* arq) {
    fclose(arq);
}

void criarIndice(FILE* arq, No* tabelaHashing[]) {
    CARRO carro;
    int pos = 0;
    while (fread(&carro, sizeof(CARRO), 1, arq)) {
        if (carro.status == 1) { // Apenas carros ativos
            inserirTabelaHash(tabelaHashing, carro.placa, pos);
        }
        pos++;
    }
}


int hashing(char placa[]) {
    int n1 = 0, n2 = 0;

    // Parte alfabética (AAA)
    n1 = (placa[0] - 'A') * 26 * 26 +
         (placa[1] - 'A') * 26 +
         (placa[2] - 'A');

    // Parte numérica (1234)
    n2 = (placa[3] - '0') * 1000 +
         (placa[4] - '0') * 100 +
         (placa[5] - '0') * 10 +
         (placa[6] - '0');

    return (n1 + n2) % N;
}

void inserirTabelaHash(No* tabelaHashing[], char placa[], int posicao) {
    int indice = hashing(placa);

    No* novoNo = malloc(sizeof(No));
    strcpy(novoNo->placa, placa);
    novoNo->posicao = posicao;
    novoNo->prox = NULL;
    novoNo->ant = NULL;

    if (tabelaHashing[indice] == NULL) {
        tabelaHashing[indice] = novoNo;
    } else {
        No* atual = tabelaHashing[indice];
        No* anterior = NULL;

        while (atual != NULL && strcmp(atual->placa, placa) < 0) {
            anterior = atual;
            atual = atual->prox;
        }

        if (anterior == NULL) {
            novoNo->prox = tabelaHashing[indice];
            tabelaHashing[indice]->ant = novoNo;
            tabelaHashing[indice] = novoNo;
        } else {
            novoNo->prox = atual;
            novoNo->ant = anterior;
            anterior->prox = novoNo;
            if (atual != NULL) {
                atual->ant = novoNo;
            }
        }
    }
}

void exibirOpcoes() {
    printf("Opções \n");
    printf("1 - Cadastrar um carro \n");
    printf("2 - Consultar carro \n");
    printf("3 - Alterar dados do carro \n");
    printf("4 - Remover carro \n");
    printf("5 - Exibir carros cadastrados \n");
    printf("0 - Encerrar programa \n");
    printf("Informe a opção: ");
}

void cadastrar(FILE* arq, No* tabelaHashing[]) {
    CARRO carro;
    printf("Digite a placa do carro (ex: ABC1234): ");
    scanf("%s", carro.placa);

    if (buscar(tabelaHashing, carro.placa) != -1) {
        printf("Carro já cadastrado!\n");
        return;
    }

    printf("Digite a marca do carro: ");
    scanf("%s", carro.marca);
    printf("Digite o modelo do carro: ");
    scanf("%s", carro.modelo);
    printf("Digite a cor do carro: ");
    scanf("%s", carro.cor);

    carro.status = 1;

    fseek(arq, 0, SEEK_END);
    int pos = ftell(arq) / sizeof(CARRO);
    fwrite(&carro, sizeof(CARRO), 1, arq);

    inserirTabelaHash(tabelaHashing, carro.placa, pos);
}

int buscar(No* tabelaHashing[], char placa[]) {
    int indice = hashing(placa);
    No* atual = tabelaHashing[indice];

    while (atual != NULL) {
        if (strcmp(atual->placa, placa) == 0) {
            return atual->posicao;
        }
        atual = atual->prox;
    }
    return -1;
}


void exibirCadastro(FILE* arq) {
    /* Exibe todos os registros de carros ATIVOS constantes no arquivo. */
    fseek(arq, 0, SEEK_SET);  // Coloca o ponteiro do arquivo no início.
    CARRO carro;
    while (fread(&carro, sizeof(CARRO), 1, arq)) {
        if (carro.status == 1) {  // Verifica se o carro está ativo.
            printf("Placa: %s\n", carro.placa);
            printf("Marca: %s\n", carro.marca);
            printf("Modelo: %s\n", carro.modelo);
            printf("Cor: %s\n\n", carro.cor);
        }
    }
}

void liberarArquivo(FILE* arq) {
    /* Apagar fisicamente os registros que foram deletados logicamente e fechar o arquivo.
       1.1 - Criar um arquivo novo (vazio).
       1.2 - Copiar todos os registros de STATUS igual a 1 do arquivo de carros para o arquivo novo.
       1.3 - Fechar os dois arquivos.
       1.4 - Remover o arquivo de carros ("carros.dat").
       1.5 - Renomear o arquivo novo com o nome "carros.dat". */
    FILE* arqNovo = fopen("carros_temp.dat", "w+b");
    if (arqNovo == NULL) {
        printf("Erro ao criar o arquivo temporário.\n");
        return;
    }

    fseek(arq, 0, SEEK_SET);
    CARRO carro;
    while (fread(&carro, sizeof(CARRO), 1, arq)) {
        if (carro.status == 1) {  // Apenas carros ativos.
            fwrite(&carro, sizeof(CARRO), 1, arqNovo);
        }
    }

    fclose(arq);
    fclose(arqNovo);

    remove("carros.dat");
    rename("carros_temp.dat", "carros.dat");
}

void desalocarIndice(No* tabelaHashing[]) {
    /* Desalocar os nós que compõem as listas da tabela de hashing. */
    for (int i = 0; i < N; i++) {
        No* atual = tabelaHashing[i];
        while (atual != NULL) {
            No* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

void removerTabelaHash(No* tabelaHashing[], char placa[], int posTabela) {
    /* Remover da tabela de hashing o nó que contém a placa passada como parâmetro. 
       Recebe como parâmetro também a posição na tabela onde a chave se encontra. */
    int indice = hashing(placa);
    No* atual = tabelaHashing[indice];

    while (atual != NULL) {
        if (strcmp(atual->placa, placa) == 0 && atual->posicao == posTabela) {
            if (atual->ant != NULL) {
                atual->ant->prox = atual->prox;
            } else {
                tabelaHashing[indice] = atual->prox;
            }
            if (atual->prox != NULL) {
                atual->prox->ant = atual->ant;
            }
            free(atual);
            return;
        }
        atual = atual->prox;
    }
}

void consultar(FILE* arq, No* tabelaHashing[]) {
    /* Consultar o registro do carro no arquivo. */
    char placa[8];
    printf("Informe a placa do carro: ");
    scanf("%s", placa);
    int pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro não encontrado.\n");
    } else {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO carro;
        fread(&carro, sizeof(CARRO), 1, arq);
        if (carro.status == 1) {
            printf("Placa: %s\n", carro.placa);
            printf("Marca: %s\n", carro.marca);
            printf("Modelo: %s\n", carro.modelo);
            printf("Cor: %s\n", carro.cor);
        } else {
            printf("Carro removido.\n");
        }
    }
}

void menu(FILE* arq, No* tabelaHashing[]) {
    /* Exibe o menu de opções e chama as funções correspondentes. */
    int opcao;
    do {
        printf("\nMenu de opções:\n");
        printf("1. Cadastrar Carro\n");
        printf("2. Consultar Carro\n");
        printf("3. Alterar Carro\n");
        printf("4. Remover Carro\n");
        printf("5. Exibir Cadastro\n");
        printf("6. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        switch(opcao) {
            case 1:
                cadastrar(arq, tabelaHashing);
                break;
            case 2:
                consultar(arq, tabelaHashing);
                break;
            case 3:
                alterar(arq, tabelaHashing);
                break;
            case 4:
                remover(arq, tabelaHashing);
                break;
            case 5:
                exibirCadastro(arq);
                break;
            case 6:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while(opcao != 6);
}

void alterar(FILE* arq, No* tabelaHashing[]) {
    char placa[8];
    printf("Informe a placa do carro a ser alterado: ");
    scanf("%s", placa);
    int pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro não encontrado.\n");
    } else {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO carro;
        fread(&carro, sizeof(CARRO), 1, arq);
        if (carro.status == 1) {
            printf("Dados do carro:\n");
            printf("Placa: %s\n", carro.placa);
            printf("Marca: %s\n", carro.marca);
            printf("Modelo: %s\n", carro.modelo);
            printf("Cor: %s\n", carro.cor);
            printf("Quais dados deseja alterar? (1 - Marca, 2 - Modelo, 3 - Cor): ");
            int opcao;
            scanf("%d", &opcao);
            if (opcao == 1) {
                printf("Informe a nova marca: ");
                scanf("%s", carro.marca);
            } else if (opcao == 2) {
                printf("Informe o novo modelo: ");
                scanf("%s", carro.modelo);
            } else if (opcao == 3) {
                printf("Informe a nova cor: ");
                scanf("%s", carro.cor);
            }

            fseek(arq, pos * sizeof(CARRO), SEEK_SET);
            fwrite(&carro, sizeof(CARRO), 1, arq);
            printf("Dados atualizados com sucesso!\n");
        } else {
            printf("Este carro foi removido e não pode ser alterado.\n");
        }
    }
}

void remover(FILE* arq, No* tabelaHashing[]) {
    char placa[8];
    printf("Informe a placa do carro a ser removido: ");
    scanf("%s", placa);
    int pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro não encontrado.\n");
    } else {
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        CARRO carro;
        fread(&carro, sizeof(CARRO), 1, arq);
        if (carro.status == 1) {
            carro.status = 0; // Marca o carro como removido.
            fseek(arq, pos * sizeof(CARRO), SEEK_SET);
            fwrite(&carro, sizeof(CARRO), 1, arq);
            removerTabelaHash(tabelaHashing, placa, pos); // Remove da tabela hash.
            printf("Carro removido com sucesso!\n");
        } else {
            printf("Este carro já foi removido.\n");
        }
    }
}