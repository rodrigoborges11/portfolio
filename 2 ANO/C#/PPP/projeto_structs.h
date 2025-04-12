#ifndef PROJETO_STRUCTS_H
#define PROJETO_STRUCTS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam 100
typedef struct data
{
    int dia,mes,ano;
}data;
typedef struct registo{
    int id,tensao_max,tensao_min,peso,altura;
    data data_registo;

}registo;
typedef struct noListaregisto{
    struct registo registoLista;
    struct noListaregisto *prox;
}noListaregisto;
typedef noListaregisto *rLista;
typedef struct doente
{
    int id,telemovel;
    char nome[tam],cc[tam],email[tam];
    data data_nascimento;
}doente;
typedef struct noListadoente
{
    struct doente doenteLista;
    struct noListadoente *prox;
    rLista listaRegisto;
}noListadoente;

typedef noListadoente *pLista;

pLista cria();
int vazia(pLista lista);
pLista destroi(pLista lista);
void procura(pLista lista, int chave, pLista *ant, pLista *actual);
void elimina(pLista lista, int chave);
void insere(pLista lista, doente d);
void insere_registo(pLista lista, registo r);
void imprime_nomes_id(pLista lista);
void imprime_alfabetica(pLista lista);
void imprime_dados(pLista lista, int id);
void listar_tensao_max(pLista lista, int valor);
int verifica_data(int dia, int mes, int ano);
int verifica_cc_ficheiro(pLista lista, char *cc);
int verifica_digito(char dig);
int verifica_telemovel_ficheiro(pLista lista, char *numero);
int verifica_email_ficheiro(pLista lista, char *email);
void escreveDoente(pLista lista);
void escreve_verifica_Doente(doente d);
int verifica_nome(char c);
void lerDoente(pLista lista);
void lerRegisto(pLista lista);
int verifica_id(pLista lista);
void escreveRegisto(pLista lista);
void escreve_verifica_Registo(registo registo_atual);

#endif