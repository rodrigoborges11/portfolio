// André Lourenço Albuquerque
// Rodrigo Morenito Borges

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

sem_t *mutex;

int pipe_fd;

typedef struct {
    int plafond_inicial;
    int num_pedidos_aut;
    int intervalo_VIDEO;
    int intervalo_MUSIC;
    int intervalo_SOCIAL;
    int dados_reservar;
} DadosConsola;

void open_sem() {
    sem_unlink("MUTEX");
    mutex = sem_open("MUTEX", O_CREAT | O_EXCL, 0700, 1);
    if (mutex == SEM_FAILED) {
        perror("Erro ao abrir o semÃ¡foro");
        exit(EXIT_FAILURE);
    }
}

void close_sem() {
    sem_close(mutex);
    sem_unlink("MUTEX");
}



void request(int interval, char* ID, int dados) {
    while(1) {
        sleep(interval);
        sem_wait(mutex);
        
        char string[100];

        sprintf(string,"%d#%s#%d\n", getppid(), ID, dados);
        printf("%s",string);
        write(pipe_fd,string,strlen(string));

        sem_post(mutex);
    }
}



int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Uso: %s <plafond_inicial> <num_pedidos_aut> <intervalo_VIDEO> <intervalo_MUSIC> <intervalo_SOCIAL> <dados_reservar>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
 DadosConsola dados_base;
    dados_base.plafond_inicial = atoi(argv[1]);
    dados_base.num_pedidos_aut = atoi(argv[2]);
    dados_base.intervalo_VIDEO = atoi(argv[3]);
    dados_base.intervalo_MUSIC = atoi(argv[4]);
    dados_base.intervalo_SOCIAL = atoi(argv[5]);
    dados_base.dados_reservar = atoi(argv[6]);

    printf("%d#%d\n", getpid(), dados_base.plafond_inicial);

    char str[100];
    pipe_fd = open("USERpipenamed",O_RDWR);

    sprintf(str,"%d#%d\n", getppid(),dados_base.plafond_inicial);

    if (pipe_fd < 0) {
        perror("Erro ao abrir o pipe para escrita");
        exit(EXIT_FAILURE);
    }

    open_sem();

    if (fork() == 0) {
        request(dados_base.intervalo_VIDEO, "VIDEO", dados_base.dados_reservar);
        exit(EXIT_SUCCESS);
    }

    if (fork() == 0) {
        request(dados_base.intervalo_MUSIC, "MUSIC", dados_base.dados_reservar);
        exit(EXIT_SUCCESS);
    }

    if (fork() == 0) {
        request(dados_base.intervalo_SOCIAL, "SOCIAL", dados_base.dados_reservar);
        exit(EXIT_SUCCESS);
    }

    close_sem();

    close(pipe_fd);

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}