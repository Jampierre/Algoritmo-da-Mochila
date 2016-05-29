//
//  main.c
//  Algoritimo da Mochila
//
//  Created by Jampierre Vieira Rocha on 5/26/16.
//  Copyright © 2016 Jampierre Vieira Rocha. All rights reserved.
//

/* ===================================================
 Progama que Executa tres metodos para resolver o problema da mochila:
 Método Guloso e Backtracking desenvolvidos a aprtir do algoritmo
 proposto em - E. Horowitz e S. Sahni, Fundamentals of Computer Algorithms,
 Computer Science Press, 1978.
 =================================================== */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <limits.h>
#include "time.h"
#define MaxTam  1000        //Tamanho máximo do Vetor
#define N 500               //Maior valor Aleatorio
#define NoElementos 300     //Quantidade de Elementos para a Mochila
#define LeArquivo 1         // 1 - via arquivo, 0 - Gerado Randomicamente
typedef int Valor;

/* ===================================================
 Estrutura tipo Registro que armazena os Pesos, Importancias,
 Chave, e Resultados. Os resultados são armazenados de acordo com o metodo
 =================================================== */
typedef struct Item {
    Valor Importancia;
    Valor Peso;
    float Chave;
    short ResultG;
    short ResultB;
    short ResultD;
} Item;

typedef int Indice;

//Vetor de Registros
typedef Item Vetor[MaxTam+1];
Vetor Origem;

//Peso Final de Método Backtracking
float Tpeso, Timportancia;
Indice i, n, k;

//Capacidade da Mochila
int M;
//Arquivos de Entrada e Saida
FILE* fp;
FILE* fs;

/* ===================================================
 Função QuickSort disponibilizada no livro
 do Prof. Nivio Ziviane.(Projeto de Algoritmos/2004).
 Foi Alterada para fazer a ordenacao Crescente.
 =================================================== */
void Particao(Indice Esq, Indice Dir, Indice *i, Indice *j, Item *A){
    Item x, w;
    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2]; /*obtem o pivo x*/
    do {
        while (x.Chave < A[*i].Chave) (*i)++;
        while (x.Chave > A[*j].Chave) (*j)--;
        if (*i <= *j)
        { w = A[*i]; A[*i] = A[*j]; A[*j] = w;
            (*i)++; (*j)--;
        }
    } while (*i <= *j);
}

void Ordena(Indice Esq, Indice Dir, Item *A){
    Indice i, j;
    Particao(Esq, Dir, &i, &j, A);
    if (Esq < j) Ordena(Esq, j, A);
    if (i < Dir) Ordena(i, Dir, A);
}

void QuickSort(Item *A, Indice *n){
    Ordena(1, *n, A);
}

/* ===================================================
 O código foi copiado da biblioteca random
 de Eric Roberts.(The Art and Science of C)
 =================================================== */
int RandomInteger (int low, int high){
    double k;
    double d;
    d = (double) rand () / ((double) RAND_MAX + 1); k=d*(high-low+1);
    return (int)(low + k);
}

/* ===================================================
 Função que faz a impressão dos Pesos e Importancias no Método Guloso
 A impressão é dirigida pelo Valor de ResultG
 =================================================== */
void ImprimeGuloso(Item *V, Indice *n){
    int Tp;
    int Ti;
    Tp = 0;
    Ti = 0;
    for(i=1;i<=*n;i++){
        if (V[i].ResultG)
            fprintf(fs," %d %d\n",V[i].Peso,V[i].Importancia);
    }
}

/* ===================================================
 Função que faz a impressão dos Pesos e Imports. no Método Backtracking
 A impressão é dirigida pelo Valor de ResulB
 =================================================== */
void ImprimeBack(Item *V, Indice *n, double Tp, double Ti){
    for(i=1;i<=*n;i++){
        if (V[i].ResultB)
            fprintf(fs,"%d %d\n",V[i].Peso,V[i].Importancia);
    }
}

/* ===================================================
 Função que faz a impressão dos Pesos e Importancias no Métod Dinâmico
 A impressão é dirigida pelo Valor do ResulD
 =================================================== */
void ImprimeDinamico(Item *V, Indice *n){
    int Tp;
    int Ti;
    Tp = 0;
    Ti = 0;
    for (i = 1; i <= *n; i++){
        if (V[i].ResultD)
            fprintf(fs," %d %d\n",V[i].Peso,V[i].Importancia);
    }
}

void CopiaDois(int Fonte[],Item *Destino, Indice *n){
    for(i=1;i<=*n;i++)
        Destino[i].ResultB = Fonte[i];
}

/* ===================================================
 Função que Gera Randomicamente o Vetor com os Pesos e Importancias,
 faz o calculo da Chave=Importancia/Peso e Iniciaiza os Resultados.
 O valor de M eh a metade do total dos pesos.
 =================================================== */
void GeraVetor(Vetor A, Indice n,int *M){
    int i;
    Valor pesoTotal;
    pesoTotal=0;
    for(i = 1; i <= n; i++){
        A[i].Importancia = RandomInteger(1,N);
        A[i].Peso = RandomInteger(1,N);
        A[i].ResultG = 0;
        A[i].ResultB = 0;
        A[i].ResultD = 0;
        pesoTotal = pesoTotal + A[i].Peso;
        A[i].Chave =(double)A[i].Importancia / (double)A[i].Peso;
        //Calcula o Valor da Mochila com 1/2 Pesos
        *M=pesoTotal*1/2;
    }
}

/* ===================================================
 Executa o Metodo Guloso
 =================================================== */
void Guloso(Vetor A, Indice n, int M){
    int i,j, CM;
    CM = M;
    for(i = 1; i <= n; i++){
        if (A[i].Peso > CM)
            break;
        A[i].ResultG = 1;
        CM = CM - A[i].Peso;
    }
    j=i+1;
    while (j <= n) {
        if (A[j].Peso <= CM) {
            CM = CM - A[j].Peso;
            A[j].ResultG = 1;
        }
        j=j+1;
    }
}

/* ===================================================
 Executa o Metodo Dinâmico
 =================================================== */
//funcao Limite para o método
float Bound(Vetor A,float p, float w, int k, float M){
    float b,c;
    b = p;
    c = w;
    for (i = k + 1; i <= n; i++) {
        c = c + A[i].Peso;
        if (c <= M)
            b = b + A[i].Importancia;
        else
            return (b + (1 -((c - M)/A[i].Peso))*A[i].Importancia);
    }
    return(b);
}

void BKnap(float M, Indice n, Vetor A, float *fw, float *fp){
    int  k, Y[n];
    float cw,cp;
    
    cw = 0.0; cp = 0.0; k=1; *fp = -1;
    
    while (1) {
        while(( k <= n ) && (( cw + A[k].Peso ) <= M )){
            cw = cw + A[k].Peso;
            cp = cp + A[k].Importancia;
            Y[k]= 1;
            k = k + 1;
        }
        if (k > n) {
            *fp = cp;
            *fw = cw;
            k=n;
            CopiaDois(Y,A,&n);
        }else
            Y[k]=0;
        while (Bound(A,cp,cw,k,M) <= *fp){
            while (k != 0 && Y[k] != 1)
                k = k - 1;
            if(k == 0)
                return;
            Y[k] = 0;
            cw = cw - A[k].Peso;
            cp = cp - A[k].Importancia;
        }//while (Bound)
        k = k + 1;
    }//while(1)
}

/* ===================================================
 Executa o Metodo Dinâmico
 =================================================== */
void Dinamico(Item *A, Indice n, int M){
    int j;
    int c;
    int **a;
    Indice i;
    
    a = (int **) calloc(n+1,sizeof(int *));
    for( i = 0; i < n + 1; i++)
        a[i] = (int *) calloc( M+1, sizeof(int));
    
    //IniciaMatriz
    for(i=0;i<=n;i++)
        for(j=0;j<= M; j++)
            a[i][j] = -1;
    //Inicializa Linha
    for( c = 0 ; c <= M ; c++ )
        a[0][c] = 0;
    //Inicializa Coluna
    for(i = 0 ; i <= n ; i++ )
        a[i][0] = 0;
    
    for( i = 1 ; i <= n ; i++ ){
        for ( c = 1 ; c <= M ; c++ ){
            if (A[i].Peso <= c){
                if ((A[i].Importancia + a[i-1][c-A[i].Peso]) > a[i-1][c] )
                    a[i][c] = A[i].Importancia + a[i-1][c-A[i].Peso];
                else
                    a[i][c] = a[i-1][c];
            }else
                a[i][c] = a[i-1][c];
        }
    }
    //Calcula os elementos que fazem parte da Resposta
    i = n;
    j = M;
    while ((i > 0) && (j > 0)){
        if (a[i][j] != a[i-1][j]){
            A[i].ResultD = 1;
            j = j-A[i].Peso;
            i--;
        }else{
            A[i].ResultD = 0;
            i--;
        }
    }
    for( i=0; i < n+1; i++ )
        free(a[i]);
    free(a);
}

/*
 Função Principal. Deve receber como chamada a primeira letra do Metodo
 G - Guloso, B - Backtracking, D - Dinamico.
 A variável LeArquivo deve ser setada antes:
 1 - Le os dados do arquivo chamado avaliacao.tp2
 0 - Gera os dados num vetor de Pesos e Importancias
 Quando os dados sao gerados, a quantidade de elementos no vetor de
 pesos/importancias fica determinada pela variavel NoElementos=200.
 O peso da Mochila( M ) fica determinado pela metade das soma dos pesos
 gerados.
 O tamanho do maior vetor eh determinado pela variável MaxTam=1000
 O faixa de valore para os pesos/importancias eh determinado pela
 variavel N=500.
*/
int main(int argc, const char * argv[]) {
    srand((int)time(NULL));
    //Inicializa a Capacidade da Mochila
    M = 0;
    //Verifica se a entrada sera via Arquivo ou Geracao Randomica
    if (LeArquivo){
        if ((fp = fopen("avaliacao.tp2", "r")) == NULL)
            fprintf(stderr, "ERRO na Abertura do Arquivo de Entrada\n");
        else{
            printf("Arquivo de Entrada aberto com Sucesso.\n");
            i=1;
            //Le o numero de elementos do Vetor
            fscanf(fp, "%d", &n);
            fscanf(fp, "%d", &M);
            while(!feof(fp) && i <=n){
                fscanf(fp, "%d", &Origem[i].Peso);
                fscanf(fp, "%d", &Origem[i].Importancia);
                Origem[i].Chave =(double)Origem[i].Importancia / (double)Origem[i].Peso;
                i++;
            }
        }
    }else{
        //Numero de elementos do Vetor
        n=NoElementos;
        // Gera o Vetor com os Pesos, Importancia
        // e Calcula a Capacidade da Mochila
        GeraVetor(Origem,n,&M);
    }
    
    if (argc == 1)
        fprintf(stderr,"ERRO: Precisa Informar um Metodo (G/B/D) \n");
    else
        //Ordena em Orden Crescente de Imp/Peso
        QuickSort(Origem,&n);
    switch (*argv[1]) {
        case 'G': //Chama Metodo Guloso
            fprintf(stderr, "Guloso\n");
            startTimer();
            Guloso(Origem,n,M);
            finishTimer();
            showTimes();
            //Abre o Arquivo de Saida
            if ((fs = fopen("saidag", "w")) == NULL)
                fprintf(stderr, "ERRO na Abertura do Arquivo de Saida\n");
            ImprimeGuloso(Origem,&n);
            break;
            
        case 'B': //Chama Metodo Backtracking
            fprintf(stderr, "Backtracking\n");
            Tpeso = 0.0;
            Timportancia = 0.0;
            startTimer();
            BKnap(M,n,Origem,&Tpeso,&Timportancia);
            finishTimer();
            showTimes();
            //Abre o Arquivo de Saida
            if ((fs = fopen("saidab", "w")) == NULL)
                fprintf(stderr, "ERRO na Abertura do Arquivo de Saida\n");
            ImprimeBack(Origem,&n,Tpeso,Timportancia);
            break;
            
        case 'D': //Chama Metodo Dinâmico
            fprintf(stderr, "Dinamico\n");
            startTimer();
            Dinamico(Origem, n, M);
            finishTimer();
            showTimes();
            //Abre o Arquivo de Saida
            if ((fs = fopen("saidad", "w")) == NULL)
                fprintf(stderr, "ERRO na Abertura do Arquivo de Saida\n");
            ImprimeDinamico(Origem, &n);
            break;
            
        default:
            fprintf(stderr,"ERRO: Método Não informado (G/B/D) \n");
            break;
    }
    fclose(fp);
    fclose(fs);
    return 0;
}
