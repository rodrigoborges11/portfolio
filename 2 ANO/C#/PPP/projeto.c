#include "projeto_structs.h"
#include <stdio.h>

int main(){
    int on=1;
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║              Bem-vindo à Aplicação!               ║\n");
    printf("╠═══════════════════════════════════════════════════╣\n");
    pLista lista=cria();
    lerDoente(lista);
    escreveDoente(lista);
    lerRegisto(lista);
    escreveRegisto(lista);
    while (on){ 
        int n;
        printf("║               Escolhe uma função:                 ║\n║");
        printf(" 1 → Introduzir dados de um novo doente            ║\n║ 2 → Eliminar um doente existente                  ║\n║ 3 → Ver doentes por ordem                         ║\n║ 4 → Doentes com tensões máximas acima de X        ║\n║ 5 → Ver a informação de um doente                 ║\n║ 6 → Registo de Medições                           ║\n║ 7 → Sair                                          ║\n╚═══════════════════════════════════════════════════╝\nOpção:");
        scanf("%d",&n);
        getchar();
        if(n<1 || n>7){
            printf("Erro ao escolher o numero\n");
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==1){
            printf("►         INSERIR DOENTE         ◄\n");
            printf("Introduza os dados do novo doente:\n");
            struct doente novoDoente;
            int erronome=1;
            while (erronome==1){
                char nome[100];
                printf("Nome: ");
                fgets(nome,sizeof(nome),stdin);
                nome[strcspn(nome,"\n")]='\0';
                erronome = 0;

                if (strlen(nome)<3){
                    erronome=1;
                }

                for (int i=0; nome[i]!='\0' ; i++ ){
                    if (verifica_nome(nome[i])==0){
                        erronome = 2;
                        break;
                    }
                }
        
                if (erronome==1){
                    printf("[ERRO]-Insira o com pelo menos 3 caracteres\n");
                    erronome = 1;   
                }else if (erronome==2){
                    erronome = 1;
                    printf("[ERRO]-Nome inválido (só usar letras/espaços)\n");
                }else {
                    strcpy(novoDoente.nome,nome);
                    erronome=0;
                }
                
            }

            int erro = 1;
            while (erro==1){
                char numero[9];
                printf("Telemóvel: ");
                scanf("%s", numero);
                erro = 0;

                if (strlen(numero)!=9){
                    erro=2;
                }

                for (int i=0; i<9 ; i++ ){
                    if (verifica_digito(numero[i])==0){
                        erro = 2;
                        break;
                    }
                }

                if (verifica_telemovel_ficheiro(lista,numero)==1){
                    erro = 3;
                }
        
                if (erro==2){
                    printf("[ERRO]-Insira o número de telemóvel com 9 digitos\n");
                    erro = 1;   
                }else if (erro==3){
                    erro = 1;
                    printf("[ERRO]-Número de telemóvel já existente\n");
                }else {
                    novoDoente.telemovel = atoi(numero);
                }
                
            }
            
            int errocc = 1;
            while (errocc==1){
                char cc[9];
                printf("CC: ");
                scanf("%s", cc);
                
                if (strlen(cc)!=8){
                    errocc=2;
                }

                for (int i=0; i<8 ; i++ ){
                    if (verifica_digito(cc[i])==0){
                        errocc = 2;
                        break;
                    }
                }

                if (verifica_cc_ficheiro(lista,cc)==1){
                    errocc = 3;
                }

                if (errocc==2){
                    printf("[ERRO]-Insira o cartão de cidadão com 8 digitos\n");
                    errocc=1;
                }else if (errocc==3) {
                    printf("[ERRO]-Número de cartão de cidadão já existente\n");
                    errocc=1;
                }else{
                   strcpy(novoDoente.cc,cc);
                   errocc = 0;
                }
                
            }

            int erromail=1;

            while (erromail==1){
                char email[254];
                printf("Email: ");
                scanf("%s", email);
            
                if (strlen(email)<6){
                    erromail=2;
                }
  
                if (verifica_email_ficheiro(lista,email)==1){
                    erromail = 3;
                }

                int last =strlen(email);

                int count=0;
                for (int i=0; i<last; i++ ){
                    if (email[i]=='@'){
                        count++;
                    }
                }
                if ((count!=1) || (email[0]=='@') || (email[last-1]=='@')){
                    erromail = 4;
                }

                if (erromail==2){
                    printf("[ERRO]-Insira um email com pelo menos 6 carateres\n");
                    erromail=1;
                }else if (erromail==4) {
                    printf("[ERRO]-Email tem de ter um e um só @ sem ser no inicio ou no fim \n");
                    erromail=1;
                }else if (erromail==3){
                    printf("[ERRO]-Email já usado \n");
                    erromail=1;
                }else{
                    strcpy(novoDoente.email,email);
                    erromail= 0;
                }
                
            }


            int errodata=1;

            while (errodata==1){
                int dia , mes , ano;
                printf("Data de Nascimento (dd mm aaaa): ");
                scanf("%d %d %d",&dia , &mes , &ano);
                
                if (verifica_data(dia,mes,ano)==0){
                    printf("[ERRO]-Data errada \n");
                    errodata=1;
                }else{
                    errodata=0;
                    novoDoente.data_nascimento.dia=dia;
                    novoDoente.data_nascimento.mes=mes;
                    novoDoente.data_nascimento.ano=ano;
                }
            }
            
            novoDoente.id=verifica_id(lista); 
            insere(lista, novoDoente);
            escreveDoente(lista); 
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==2){ // Falta eliminar os registos desse mesmo doente
           printf("►         ELIMINAR DOENTE         ◄\n");
           int id_eliminar;
           imprime_nomes_id(lista);
           int erroeli=1;
           while (erroeli==1){
                char ideliminar[9];
                printf("ID: ");
                scanf("%s",ideliminar);
                erroeli = 0;

                for (int i=0; ideliminar[i] != '\0' ; i++ ){
                    if (verifica_digito(ideliminar[i])==0){
                        erroeli = 1;
                        break;
                    }
                }
        
                if (erroeli==1){
                    printf("[ERRO]-Insira o ID a eliminar válido/com digitos\n");
                    erroeli = 1;   
                }else {
                    id_eliminar = atoi(ideliminar);
                    erroeli=0;
                }
                

            }
           elimina(lista,id_eliminar); // eliminar pelo id
           escreveDoente(lista);
           printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==3){
            printf("►         LISTA ORDENADA         ◄\n");
            imprime_alfabetica(lista);
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==4){
            int tensao_max_min;
            int erromaxmin=1;
            printf("►         DOENTES ACIMA DA TENSÃO MÁXIMA         ◄\n");

            while (erromaxmin==1){
                char maxmin[9];
                printf("Diga qual o valor mínimo da tensão máxima pretende: ");
                scanf("%s",maxmin);
                erromaxmin = 0;

                for (int i=0; maxmin[i] != '\0' ; i++ ){
                    if (verifica_digito(maxmin[i])==0){
                        erromaxmin = 1;
                        break;
                    }
                }
        
                if (erromaxmin==1){
                    printf("[ERRO]-Insira o valor minimo da tensão máxima válido/com digitos\n");
                    erromaxmin = 1;   
                }else {
                    tensao_max_min = atoi(maxmin);
                }
                
                
            }
            
            listar_tensao_max(lista,tensao_max_min);
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==5){
            int id_show;
            int erroshow=1;
            printf("►         INFORMAÇÃO DE UM DOENTE         ◄\n");
            while (erroshow==1){
                char id[9];
                printf("Diga o ID do doente que quer ver: ");
                scanf("%s",id);
                erroshow = 0;

                for (int i=0; id[i] != '\0' ; i++ ){
                    if (verifica_digito(id[i])==0){
                        erroshow = 1;
                        break;
                    }
                }
        
                if (erroshow==1){
                    printf("[ERRO]-Insira o ID válido/com digitos\n");
                    erroshow = 1;   
                }else {
                    id_show = atoi(id);
                }
                

            }
            imprime_dados(lista,id_show);
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==6){
            printf("►         INSERIR REGISTO DE DOENTE         ◄\n");
            printf("Introduza os dados do registo do doente:\n");
            struct registo novoRegisto;
           
            int erronew=1;

            while (erronew==1){
                char idnew[9];
                printf("ID: ");
                scanf("%s",idnew);
                erronew = 0;

                for (int i=0; idnew[i] != '\0' ; i++ ){
                    if (verifica_digito(idnew[i])==0){
                        erronew = 1;
                        break;
                    }
                }
        
                if (erronew==1){
                    printf("[ERRO]-Insira o ID válido/com digitos\n");
                    erronew = 1;   
                }else {
                    novoRegisto.id = atoi(idnew);
                    erronew=0;
                }
                

            }
            

            int errodatareg=1;

            while (errodatareg==1){
                int dia , mes , ano;
                printf("Data de Registo (dd mm aaaa): ");
                scanf("%d %d %d",&dia , &mes , &ano);
                
                if (verifica_data(dia,mes,ano)==0){
                    printf("[ERRO]-Data errada \n");
                    errodatareg=1;
                }else{
                    errodatareg=0;
                    novoRegisto.data_registo.dia=dia;
                    novoRegisto.data_registo.mes=mes;
                    novoRegisto.data_registo.ano=ano;
                }
            }
            int errotensao = 1;
            while (errotensao==1){
                char tensaomax[3];
                printf("Tensão Máxima: ");
                scanf("%s",tensaomax);
                errotensao=0;

                for (int i=0; tensaomax[i]!='\0'; i++ ){
                    if (verifica_digito(tensaomax[i])==0){
                        errotensao = 1;
                        break;
                    }
                }

                if (errotensao==1){
                    printf("[ERRO]-Insira um valor de tensão máxima válido/com digitos\n");
                    errotensao = 1; 
                }else{
                    errotensao = 0; 
                    novoRegisto.tensao_max= atoi(tensaomax);
                }    
            }

            int errotensaomin = 1;
            while (errotensaomin==1){
                char tensaomin[3];
                printf("Tensão Minima: ");
                scanf("%s",tensaomin);
                errotensaomin=0;

                for (int i=0; tensaomin[i]!='\0'; i++ ){
                    if (verifica_digito(tensaomin[i])==0){
                        errotensaomin = 1;
                        break;
                    }
                }

            
                if(atoi(tensaomin) >= novoRegisto.tensao_max){
                    errotensaomin=2;
                }


                if (errotensaomin==1){
                    printf("[ERRO]-Insira um valor de tensão minima válido/com digitos\n");
                    errotensaomin = 1; 
                }else if (errotensaomin==2){
                    errotensaomin = 1; 
                    printf("[ERRO]-Insira o número de tensao menor que a tensao máxima(%d)\n",novoRegisto.tensao_max);
                }else{
                    errotensaomin = 0; 
                    novoRegisto.tensao_min= atoi(tensaomin);
                }        
            }
           
            int erropeso = 1;
            while (erropeso==1){
                char peso[3];
                printf("Peso: ");
                scanf("%s",peso);
                erropeso=0;

                for (int i=0; peso[i]!='\0'; i++ ){
                    if (verifica_digito(peso[i])==0){
                        erropeso = 2;
                        break;
                    }
                }

                if (atoi(peso)<25||atoi(peso)>565){
                    erropeso=1;
                }

                if (erropeso==1){
                    printf("[ERRO]-Insira um valor de peso válido(25 < peso < 565)\n");
                    erropeso = 1; 
                }else if (erropeso==2){
                    printf("[ERRO]-Insira o valor de peso com digitos\n");
                    erropeso = 1; 
                }else{
                    erropeso = 0; 
                    novoRegisto.peso= atoi(peso);
                }    
            }

            int erroalt = 1;
            while (erroalt==1){
                char alt[3];
                printf("Altura: ");
                scanf("%s",alt);
                erroalt=0;

                for (int i=0; alt[i]!='\0'; i++ ){
                    if (verifica_digito(alt[i])==0){
                        erroalt = 2;
                        break;
                    }
                }

                if (atoi(alt)<62||atoi(alt)>217){
                    erroalt=1;
                }

                if (erroalt==1){
                    printf("[ERRO]-Insira um valor da altura válido ( 62 < altura < 217)\n");
                    erroalt = 1; 
                }else if (erroalt==2){
                    printf("[ERRO]-Insira o valor da altura com digitos\n");
                    erroalt = 1; 
                }else{
                    erroalt = 0; 
                    novoRegisto.altura= atoi(alt);
                }    
            }
            insere_registo(lista, novoRegisto);
            escreveRegisto(lista); 
            printf("╔═══════════════════════════════════════════════════╗\n");
        }
        if(n==7){
            on=0;
            printf("A encerrar a Aplicação...\n");
        }
    }
    
    
    return 0;
}