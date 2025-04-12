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
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/msg.h>
#include <signal.h>

#include <pthread.h>

int pipeback;

#define MSG_SIZE 128



sem_t * mutex;

void open_sem() {
    sem_unlink("MUTEX");
    mutex = sem_open("MUTEX", O_CREAT | O_EXCL, 0700, 1);
}

void close_sem() {
    sem_close(mutex);
    sem_unlink("MUTEX");
}



void request(char * msg , int id) { 

    char string[100];
    sprintf(string,"%d#%s\n", id , msg);
    printf("%s",string);
    write(pipeback,string,strlen(string));
    printf("Ação enviada com sucesso\n");

}

int main(int argc, char* argv[]) {
    open_sem();

    pipeback = open("BACKpipenamed",O_RDWR );


    if (pipeback < 0) {
        perror("Erro ao abrir o pipe para escrita");
        exit(EXIT_FAILURE);
    }

    while(1) {
        char str[30]; 
        scanf("%s", str);
        if (!strcmp(str, "data_stats")) {
            request("data_stats", 1);
            // data_stats
        } else if (!strcmp(str, "reset")) {
            request("reset", 1);
            // "reset"
        } else {
            printf("Comando não existe\n");
        }
    }

    close_sem(); 
    close(pipeback);


    return 0;
}