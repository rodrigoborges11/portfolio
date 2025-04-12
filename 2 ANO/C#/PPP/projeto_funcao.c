
#define TAM 200
#include "projeto_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


pLista cria(){
    pLista aux;
    struct doente dheader ={0,0,"","","",{0, 0, 0}};
    aux = (pLista) malloc(sizeof(noListadoente));
    if(aux!=NULL){
        aux->doenteLista=dheader;
        aux->prox=NULL;
        aux->listaRegisto=NULL;
    }
    return aux;
}
int vazia(pLista lista){
    if(lista->prox==NULL)return 1;
    else return 0;
}
pLista destroi(pLista lista){
    pLista temp_ptr;
    while (!vazia(lista))
    {
        temp_ptr=lista;
        lista = lista->prox;
        free(temp_ptr);
    }
    free(lista);
    return NULL;
}
// ver melhor o procura
void procura(pLista lista, int chave, pLista * ant, pLista * actual)
{
    *ant = lista;
    *actual = lista->prox;
    while ((*actual) != NULL && (*actual)->doenteLista.id < chave)
    {
        *ant = *actual;
        *actual = (*actual)->prox;
    }
    if ((*actual) != NULL && (*actual)->doenteLista.id != chave){
        *actual = NULL;
    }
    if ((*actual)==NULL){
        printf("ID não existe\n");
    }
}

void elimina(pLista lista, int chave)
{
    pLista ant, actual;
    procura(lista, chave, &ant, &actual);
    if (actual != NULL)
    {
        ant->prox = actual->prox;
        free(actual);
    }
}
void insere(pLista lista, struct doente d)
{
    pLista no = (pLista)malloc(sizeof(noListadoente));
    if (no != NULL)
    {
        no->doenteLista = d;
        no->prox = NULL;
        if(lista->prox==NULL){
            lista->prox=no;
        }
        else{
            pLista ant=lista;
            pLista atual= lista->prox;
            while(atual!=NULL && strcasecmp(atual->doenteLista.nome,d.nome)<0){
                ant=atual;
                atual=atual->prox;
            }
            ant->prox=no;
            no->prox=atual;
        }
    }
    else{
        printf("Erro a alocar memória para o nó");
    }
    
}

void insere_registo(pLista lista, struct registo r){
    pLista aux = lista->prox; // Dá skip ao header
    while (aux!=NULL)
    {
        if(aux->doenteLista.id==r.id){
           noListaregisto *no = (noListaregisto*)malloc(sizeof(noListaregisto));
           if(no==NULL){
            printf("Erro a alocar memória");
           }
           no->registoLista=r;
           noListaregisto *ant=NULL;
           noListaregisto *atual = aux->listaRegisto;
           while (atual!=NULL &&atual->registoLista.tensao_max>r.tensao_max)
           {
                ant=atual;
                atual=atual->prox;
           }
           if(ant==NULL){ // Ou seja lista vazia
           
                no->prox=aux->listaRegisto;
                aux->listaRegisto=no;
            }
           else{
            no->prox=atual;
            ant->prox=no;
           }
           return; 
        }
        
        aux=aux->prox;
    }
    printf("Id não existe\n");
    
}

void imprime_nomes_id(pLista lista){
    pLista aux = lista->prox; /* Salta o header */
    while (aux)
    {
       printf("%d\n",aux->doenteLista.id);
       printf("%s \n", aux->doenteLista.nome); 
       aux = aux->prox;
    }
}
void imprime_alfabetica(pLista lista) // Imprimir todos os dados incluindo os registos de um determinado doente
{
    pLista aux = lista->prox; /* Salta o header */
    while (aux)
    {
        printf("%d\n",aux->doenteLista.id);
        printf("%s \n", aux->doenteLista.nome);
        printf("%d/%d/%d \n",aux->doenteLista.data_nascimento.dia,aux->doenteLista.data_nascimento.mes,aux->doenteLista.data_nascimento.ano);
        printf("%d \n", aux->doenteLista.telemovel);
        printf("%s \n", aux->doenteLista.email);
        printf("\n");
        aux = aux->prox;
    }
}

void imprime_dados(pLista lista, int id){
    pLista aux = lista->prox; /* Salta o header */
    int existe=0;
    while (aux)
    {
        if(aux->doenteLista.id == id){
            printf("%d\n",aux->doenteLista.id);
            printf("%s \n", aux->doenteLista.nome);
            printf("%d/%d/%d \n",aux->doenteLista.data_nascimento.dia,aux->doenteLista.data_nascimento.mes,aux->doenteLista.data_nascimento.ano);
            printf("%d \n", aux->doenteLista.telemovel);
            printf("%s \n", aux->doenteLista.email);
            printf("\n");
            noListaregisto *registo_atual = aux->listaRegisto;
            existe=1;
            while (registo_atual)
            {
                printf("\nData de Registo: %d/%d/%d\n", registo_atual->registoLista.data_registo.dia, registo_atual->registoLista.data_registo.mes, registo_atual->registoLista.data_registo.ano);
                printf("Tensão Máxima: %d\n", registo_atual->registoLista.tensao_max);
                printf("Tensão Mínima: %d\n", registo_atual->registoLista.tensao_min);
                printf("Peso: %d\n", registo_atual->registoLista.peso);
                printf("Altura: %d\n", registo_atual->registoLista.altura);
                registo_atual=registo_atual->prox;
            }
             break;
        }
       aux= aux->prox;
    }
    if(existe==0){
        printf("Doente com ID %d não encontrado.\n", id);
    }
}


void listar_tensao_max(pLista lista,int valor){
    struct registo registos[50];
    int j = 0;
    pLista aux = lista->prox; // Salta o header
    while(aux){
        //Vai ao primeiro elemento da lista registo (maior) e vê se é maior que o valor pedido
        noListaregisto *registo_atual = aux->listaRegisto;
        if (registo_atual && registo_atual->registoLista.tensao_max > valor) {
            registos[j] = registo_atual->registoLista;
            j++;
        }
        aux = aux->prox;
    };
    //ordenar os registos de maneira decrescente
    for (int i = 1; i < j; i++) {
        struct registo registo_atual = registos[i];
        int j = i - 1;
        while (j >= 0 && registos[j].tensao_max < registo_atual.tensao_max) {
            registos[j + 1] = registos[j];
            j = j - 1;
        }
        registos[j + 1] = registo_atual;
    }
    for (int i = 0; i < j; i++) {
        struct registo r = registos[i];
        printf("\nId: %d\n", r.id);
        printf("Tensão Máxima: %d\n", r.tensao_max);
    }
    
}

void lerDoente(pLista lista){
    FILE *ficheiro;
    FILE *ficheiro_temp;
    ficheiro = fopen(DATA_FILE_PATH_doentes,"r");
    ficheiro_temp=fopen("doentes_temp.txt","w");
    if(ficheiro==NULL || ficheiro_temp == NULL){
        printf("ficheiro não foi aberto \n");
        return;
    }
    char linha[TAM];
    int conta_linha=0;
    int adicionado=1;
    while (fgets(linha,TAM,ficheiro)!=NULL)
    {
        linha[strcspn(linha,"\n")] = '\0'; // Remove o \n do final da linha
        doente novoDoente;
        switch (conta_linha)
        {
            case 0://id
                novoDoente.id=atoi(linha);
                break;
            case 1://nome
                strcpy(novoDoente.nome,linha);
                break;
            case 2://data
                char *token=strtok(linha,"/");
                novoDoente.data_nascimento.dia=atoi(token);
                token=strtok(NULL,"/");
                novoDoente.data_nascimento.mes=atoi(token);
                token=strtok(NULL,"/");
                novoDoente.data_nascimento.ano=atoi(token);
                if(!verifica_data(novoDoente.data_nascimento.dia, novoDoente.data_nascimento.mes, novoDoente.data_nascimento.ano)){
                    adicionado=0;
                }
                break;
            case 3://cc
                strcpy(novoDoente.cc,linha);
                if (strlen(novoDoente.cc) != 8 || verifica_cc_ficheiro(lista, novoDoente.cc)) {
                    adicionado=0;
                }
                else{
                    for (int i=0; i<8 ; i++ ){
                        if (verifica_digito(novoDoente.cc[i])==0){
                            adicionado=0;
                            break;
                        }
                    }
                }
                break;
            case 4://telemovel
                novoDoente.telemovel=atoi(linha);
                if (strlen(linha) != 9 || verifica_telemovel_ficheiro(lista, linha)) {
                    adicionado = 0;
                }
                else{
                    for (int i=0; i<9 ; i++ ){
                        if (verifica_digito(linha[i])==0){
                            adicionado=0;
                        break;
                        }
                    }
                }
                break;
            case 5://email
                strcpy(novoDoente.email,linha);
                int last = strlen(novoDoente.email);
                int count = 0;
                for (int i = 0; i < last; i++) {
                    if (novoDoente.email[i] == '@') {
                        count++;
                    }
                }
                if (count != 1 || novoDoente.email[0] == '@' || novoDoente.email[last - 1] == '@' || verifica_email_ficheiro(lista, novoDoente.email)) {
                    adicionado = 0;
                }
                if(adicionado==1){
                  insere(lista,novoDoente); 
                  escreve_verifica_Doente(novoDoente); 
                }
                adicionado=1;
                break;
        }
        conta_linha++;
        if(conta_linha==6){
            conta_linha=0;
        }
    }
    fclose(ficheiro);
    fclose(ficheiro_temp);
    remove(DATA_FILE_PATH_doentes);
    rename("doentes_temp.txt",DATA_FILE_PATH_doentes);
    
}
void lerRegisto(pLista lista){
    FILE *ficheiro;
    FILE *ficheiro_temp;
    ficheiro = fopen(DATA_FILE_PATH_registos,"r");
    ficheiro_temp=fopen("registos_temp.txt","w");
    if(ficheiro==NULL || ficheiro_temp == NULL){
        printf("ficheiro não foi aberto \n");
        return;
    }
    char linha[TAM];
    int conta_linha=0;
    int adicionado=1;
    while (fgets(linha,TAM,ficheiro)!=NULL)
    {
        linha[strcspn(linha,"\n")] = '\0'; // Remove o \n do final da linha
        registo novoRegisto;
        switch (conta_linha)
        {
            case 0://id
                for (int i=0; linha[i] != '\0' ; i++ ){
                    if (verifica_digito(linha[i])==0){
                        adicionado=0;
                        break;
                    }
                }
                novoRegisto.id=atoi(linha);
                break;
            case 1://data
                char *token=strtok(linha,"/");
                novoRegisto.data_registo.dia=atoi(token);
                token=strtok(NULL,"/");
                novoRegisto.data_registo.mes=atoi(token);
                token=strtok(NULL,"/");
                novoRegisto.data_registo.ano=atoi(token);
                if(!verifica_data(novoRegisto.data_registo.dia, novoRegisto.data_registo.mes, novoRegisto.data_registo.ano)){
                    adicionado=0;
                }
                break;
            case 2://tensao_maximo
                for (int i=0; linha[i]!='\0'; i++ ){
                    if (verifica_digito(linha[i])==0){
                        adicionado=0;
                        break;
                    }
                }
                novoRegisto.tensao_max=atoi(linha);
                break;
            case 3://tensao_minimo
                for (int i=0; linha[i]!='\0'; i++ ){
                    if (verifica_digito(linha[i])==0){
                        adicionado=0;
                        break;
                    }
                }
                novoRegisto.tensao_min=atoi(linha);
                break;
            case 4://peso
                for (int i=0; linha[i]!='\0'; i++ ){
                    if (verifica_digito(linha[i])==0){
                        adicionado=0;
                        break;
                    }
                }
                novoRegisto.peso=atoi(linha);
                if (novoRegisto.peso<25||novoRegisto.peso>565){
                    adicionado=0;
                    break;
                }
                break;
            case 5://altura
                for (int i=0; linha[i]!='\0'; i++ ){
                    if (verifica_digito(linha[i])==0){
                        adicionado=0;
                        break;
                    }
                }
                novoRegisto.altura=atoi(linha);
                if (novoRegisto.altura<62||novoRegisto.altura>217){
                    adicionado=0;
                    break;
                }
                if(adicionado==1){
                  insere_registo(lista,novoRegisto); 
                  escreve_verifica_Registo(novoRegisto); 
                }
                adicionado=1;
                break;
        }
        conta_linha++;
        if(conta_linha==6){
            conta_linha=0;
        }
    }
    fclose(ficheiro);
    fclose(ficheiro_temp);
    remove(DATA_FILE_PATH_registos);
    rename("registos_temp.txt",DATA_FILE_PATH_registos);
    
}
void escreve_verifica_Doente(doente d){
    FILE *ficheiro;
    ficheiro = fopen("doentes_temp.txt","a");
    if(ficheiro==NULL){
        printf("ficheiro não foi aberto \n");
    }
    fprintf(ficheiro, "%d\n", d.id);
    fprintf(ficheiro, "%s\n", d.nome);
    fprintf(ficheiro, "%d/%d/%d\n", d.data_nascimento.dia, d.data_nascimento.mes, d.data_nascimento.ano);
    fprintf(ficheiro, "%s\n", d.cc);
    fprintf(ficheiro, "%d\n", d.telemovel);
    fprintf(ficheiro, "%s\n", d.email);
    fclose(ficheiro);
}
void escreveDoente(pLista lista){
    FILE *ficheiro;
    ficheiro = fopen(DATA_FILE_PATH_doentes,"w");
    if(ficheiro==NULL){
        printf("ficheiro não foi aberto \n");
    }
    pLista atual = lista->prox;
    while(atual!=NULL){
        struct doente d = atual->doenteLista;
        fprintf(ficheiro,"%d\n",d.id);
        fprintf(ficheiro,"%s\n",d.nome);
        fprintf(ficheiro,"%d/%d/%d\n",d.data_nascimento.dia,d.data_nascimento.mes,d.data_nascimento.ano);
        fprintf(ficheiro,"%s\n",d.cc);
        fprintf(ficheiro,"%d\n",d.telemovel);
        fprintf(ficheiro,"%s\n",d.email);
        atual=atual->prox;
    }
    
    fclose(ficheiro);
}
int verifica_id(pLista lista){
    int maior_id=0;
        pLista aux=lista->prox;
        while(aux!=NULL){
            if(aux->doenteLista.id > maior_id){
                maior_id=aux->doenteLista.id;
            }
            aux=aux->prox;
        }
    return maior_id+1;
}
void escreve_verifica_Registo(registo r){
    FILE *ficheiro;
    ficheiro = fopen("registos_temp.txt","a");
    if(ficheiro==NULL){
        printf("ficheiro não foi aberto \n");
    }
    fprintf(ficheiro, "%d\n", r.id);
    fprintf(ficheiro, "%d/%d/%d\n", r.data_registo.dia,r.data_registo.mes,r.data_registo.ano);
    fprintf(ficheiro, "%d\n", r.tensao_max);
    fprintf(ficheiro, "%d\n", r.tensao_min);
    fprintf(ficheiro, "%d\n", r.peso);
    fprintf(ficheiro, "%d\n", r.altura);
    fclose(ficheiro);
}
void escreveRegisto(pLista lista){
    FILE *ficheiro;
    ficheiro = fopen(DATA_FILE_PATH_registos,"w");
    if (ficheiro == NULL)
    {
        printf("Erro a abrir o arquivo");
    }
    pLista aux = lista->prox;
    while(aux){
        noListaregisto *registo_atual = aux->listaRegisto;
        while(registo_atual){
            fprintf(ficheiro,"%d\n",registo_atual->registoLista.id);
            fprintf(ficheiro,"%d/%d/%d\n",registo_atual->registoLista.data_registo.dia,registo_atual->registoLista.data_registo.mes,registo_atual->registoLista.data_registo.ano);
            fprintf(ficheiro,"%d\n",registo_atual->registoLista.tensao_max);
            fprintf(ficheiro,"%d\n",registo_atual->registoLista.tensao_min);
            fprintf(ficheiro,"%d\n",registo_atual->registoLista.peso);
            fprintf(ficheiro,"%d\n",registo_atual->registoLista.altura);
            registo_atual=registo_atual->prox;
        }
        aux=aux->prox;
    }
    fclose(ficheiro);
}


int verifica_digito(char dig) {
    return dig >= '0' && dig <= '9';
}


int verifica_telemovel_ficheiro(pLista lista,char * numero){
    int encontrou=0;
    pLista aux=lista->prox;
    while(aux!=NULL){
        if (aux->doenteLista.telemovel == atoi(numero)){
            encontrou=1;
        }
        aux=aux->prox;
    }
    return encontrou;
}

int verifica_cc_ficheiro(pLista lista,char * cc){
    int encontrou=0;
    pLista aux=lista->prox;
    while(aux!=NULL){
        if (strcmp(aux->doenteLista.cc,cc)==0){
            encontrou=1;
        }
        aux=aux->prox;
    }
    return encontrou;
}

int verifica_email_ficheiro(pLista lista,char * email){
    int encontrou=0;
    pLista aux=lista->prox;
    while(aux!=NULL){
        if (strcmp(aux->doenteLista.email,email)==0){
            encontrou=1;
        }
        aux=aux->prox;
    }
    return encontrou;
}


int verifica_data(int dia , int mes , int ano){
    int certo=0;
    if (ano>1907 && ano <2025){
        if (mes>=1 && mes<=12){
            if ((dia >= 1 && dia <= 31) && (mes==1 || mes ==3 || mes==5 || mes==7 || mes==8 || mes==10 || mes==12)){
                certo=1;
            } else if ((dia >= 1 && dia <= 30) && (mes == 4 || mes==6 || mes==9 || mes== 11)){
                certo=1;
            }else if ( dia==29 && mes==2 && (ano%400==0 || (ano%4==0 && ano%100!=0))){
                certo = 1;
            }else if ((dia >= 1 && dia <= 30) && (mes==2)){
                certo = 1;
            }else{
                certo = 0;
            }
            
        }else{
            certo=0;
        }
        
    }else{
        certo = 0;
    }

    return certo;
}

int verifica_nome(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == ' '));
}
