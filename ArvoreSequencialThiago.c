#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define TAMANHO 4
// Globais
int numeros[TAMANHO], *arvore, tam_arvore;

// Funções
void inserir(void);
void remover(int valor, int indice);
void criaArvore(int ini, int fim, int indice);
void atualizarNumeros(int valor);
void visualizarDados(void);
int altura(int ini, int fim);//Altura da árvore
//Ordenar a árvore
void mergeSort(int inicio, int fim);
void merge(int inicio, int meio, int fim);

int main(void){
    int opcao;
    //Inicializar o vetor da árvore e o seu tamanho
    int h = altura(0, TAMANHO-1);
    tam_arvore = (int)pow(2, h)-1;
    arvore = calloc(tam_arvore, sizeof(int));

    do{
        printf("\n1-Inserir\n2-Remover\n3-Visualizar\n0-Sair\nOpcao: ");
        scanf("%d", &opcao);

        if(opcao != 0){
            switch(opcao){

                case 1:
                    numeros[TAMANHO-1] != 0? printf("\nLimite de numeros atingindo.\n"): inserir();
                    break;

                case 2:
                    remover(0,0);
                    break;

                case 3:
                    visualizarDados();
                    break;

                default:
                    printf("\nOpcao invalida. Tente novamente.\n");
                    break;
            }
        }

    }while(opcao != 0);

    return 0;
}

// Inserir valores no vetor de números
void inserir(){
    int indice, valor;

    printf("\nValor: ");
    scanf("%d", &valor);

    for(indice = 0; indice < TAMANHO; indice++){
        if(numeros[indice] == 0){
            numeros[indice] = valor;
            break;
        }
    }
    mergeSort(0, indice);
    criaArvore(0, indice, 0);

    printf("Valor inserido!\n");
}
//Construção da árvore
void criaArvore(int ini, int fim, int indice){
    if(ini > fim){
        return;
    }
    int meio = (ini+fim)/2;

    arvore[indice] = numeros[meio];

    criaArvore(ini, meio-1, (2*indice)+1);
    criaArvore(meio+1, fim, (2*indice)+2);
}

//Remover valores da vetor da árvore
void remover(int valor, int indice){
    int prox_no, ultimo_no;//variáveis para auxiliar o método de remoção
    
    if(indice >= tam_arvore){
        printf("\nValor nao encontrado.");
        return;
    }
    //Primeira interação
    if(valor == 0){
        printf("\nValor: ");
        scanf("%d", &valor);
    }

    if(valor == arvore[indice]){
        //Caso 1: O nó não possui filhos (folha)
        if((arvore[(2*indice)+1] == 0 && arvore[(2*indice)+2] == 0) || indice == tam_arvore-1){
            arvore[indice] = 0;
            printf("Valor removido!\n");
            //Atualizar o vetor de números
            atualizarNumeros(valor);
            return;

        //Caso 2: O nó possui ao menos um filho
        }else if(arvore[(2*indice)+1] != 0){//Possui filho a esquerda
            ultimo_no = prox_no = (2*indice)+1;

            while(arvore[prox_no] != 0 && prox_no < tam_arvore){
                ultimo_no = prox_no;
                prox_no = (2*prox_no)+2;
            }

        }else{//Possui filho a direita
            ultimo_no = prox_no = (2*indice)+2;
             
            while(arvore[prox_no] != 0 && prox_no < tam_arvore){
                ultimo_no = prox_no;
                prox_no = (2*prox_no)+1;
            }
        }
        //Troca de valores entre o nó atual e o último
        arvore[indice] = arvore[ultimo_no];
        arvore[ultimo_no] = valor;
        remover(valor, ultimo_no);//Verificar a condição do último nó

    }else if(valor < arvore[indice]){
        remover(valor, (2*indice)+1);
    }else{
        remover(valor, (2*indice)+2);
    }
}

//Liberar posição no vetor de números
void atualizarNumeros(int valor){
    int indice, temp_valor;

    for(indice = 0; indice < TAMANHO; indice++){
        if(numeros[indice] == valor){
            numeros[indice] = 0;
            break;
        }
    }

    while(indice < TAMANHO-1){
        temp_valor = numeros[indice];
        numeros[indice] = numeros[indice+1];
        numeros[indice+1] = temp_valor;

        indice++;
    }
}

//Visualização geral
void visualizarDados(){
    printf("\nVetor de numeros: ");
    for(int indice = 0; indice < TAMANHO; indice++){
      
        if(numeros[indice] == 0){
            printf("[ ] ");
        }else{
            printf("[%d] ", numeros[indice]);
        }
    }
    
    printf("\nVetor da arvore:  ");
    for(int indice = 0; indice < tam_arvore; indice++){

        if(arvore[indice] == 0){
            printf("[ ] ");
        }else{
            printf("[%d] ", arvore[indice]);
        }
    }
    printf("\n");
}

//Decobrir  a altura da árvore
int altura(int ini, int fim){
    if(ini > fim){
        return 0;
    }
    int meio = (ini+fim)/2;

    int esq = altura(ini, meio-1);
    int dir = altura(meio+1, fim);

    if(esq > dir){
        return esq+1;
    }
    return dir+1;
}

//Ordenar o vetor de números
void mergeSort(int inicio, int fim){
    if(inicio < fim){
        int meio = (inicio + fim) / 2;
        mergeSort(inicio, meio);
        mergeSort(meio + 1, fim);
        merge(inicio, meio, fim);
    }
}

void merge(int inicio, int meio, int fim){
    int i, j, k;
    int tamanhoEsquerda = meio - inicio + 1;
    int tamanhoDireita = fim - meio;

    int vetorEsquerda[tamanhoEsquerda], vetorDireita[tamanhoDireita];

    for(i = 0; i < tamanhoEsquerda; i++){
        vetorEsquerda[i] = numeros[inicio + i];
    }
    for(j = 0; j < tamanhoDireita; j++){
        vetorDireita[j] = numeros[meio + 1 + j];
    }

    i = 0;
    j = 0;
    k = inicio;

    while(i < tamanhoEsquerda && j < tamanhoDireita){
        if(vetorEsquerda[i] <= vetorDireita[j]){
            numeros[k] = vetorEsquerda[i];
            i++;
        }else{
            numeros[k] = vetorDireita[j];
            j++;
        }
        k++;
    }

    while(i < tamanhoEsquerda){
        numeros[k] = vetorEsquerda[i];
        i++;
        k++;
    }

    while(j < tamanhoDireita){
        numeros[k] = vetorDireita[j];
        j++;
        k++;
    }
}