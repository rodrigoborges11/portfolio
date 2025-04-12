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
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>
#define BUF_SIZE 100
#define QUEUE_SIZE 50
#define CONFIG_FILE "config.txt"
#define PIPE1 "USERpipenamed"
#define PIPE2 "BACKpipenamed"


int shmid;
int mqid;
int pipesengine[100][2];



pthread_t receiver, sender;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexlog = PTHREAD_MUTEX_INITIALIZER;

sem_t * mutexauth;


typedef struct {
    int data_total[3];
    int total_reqs[3];
} shareddata;

shareddata *sdptr;

typedef struct {
    int mobile_users;
    int queue_pos;
    int auth_servers;
    int auth_proc_time;
    int max_video_wait;
    int max_others_wait;
} SystemManager;

typedef struct Queue{
    int front, rear;
    char items[QUEUE_SIZE][100]; // Array de caracteres (strings)
} Queue;



Queue video_queue , other_queue;

// Função para inicializar uma fila vazia
void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

// Função para verificar se a fila está vazia
bool isEmpty(Queue *q) {
    return q->rear == -1;
}

// Função para verificar se a fila está cheia
bool isFull(Queue *q) {
    return (q->rear + 1) % QUEUE_SIZE == q->front;
}

// Função para adicionar uma string à fila
void enqueue(Queue *q, const char *item) {
    if (isFull(q)) {
        printf("Fila está cheia. Pedido não adicionado");
    } else {
        if (isEmpty(q)) {
            q->front = 0;
            q->rear = 0;
        } else {
            q->rear = (q->rear + 1) % QUEUE_SIZE;
        }
        strcpy(q->items[q->rear], item);
        
    }
}

void dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Fila está vazia. Nada a ser removido.\n");
    } else {
        if (q->front == q->rear) {
            // Se há apenas um elemento na fila, reinicializamos a fila
            q->front = -1;
            q->rear = -1;
        } else {
            // Avançamos o ponteiro da frente para o próximo elemento
            q->front = (q->front + 1) % QUEUE_SIZE;
        }
    }
}

void data_stats(){
    printf("Service     Total Data      Auth Reqs\n");
    for (int i = 0; i < 3; i++) {
        if(i==0){
    	printf("VIDEO		%d		%d\n",sdptr->data_total[i],sdptr->total_reqs[i]);
        }
        if(i==1){
     	printf("MUSIC		%d		%d\n",sdptr->data_total[i],sdptr->total_reqs[i]);
        }
        if(i==2){
        printf("SOCIAL		%d		%d\n",sdptr->data_total[i],sdptr->total_reqs[i]);
        }
                   
    }

}


void reset(){
    for(int i=0;i<3;i++){
        sdptr->data_total[i]=0;
        sdptr->total_reqs[i]=0;
    }
}



void authorizationenginefunc(int pipe[]){
    char * first;
    char * comando;
    char buffer[BUF_SIZE];


    while(1){
        int stringpipe  = read(pipe[0], buffer, BUF_SIZE);
        first  = strtok(buffer,"#");
            
        if (strcmp(first,"1") != 0){
            printf("Pedido vindo do mobileuser\n");

            char *categoriavalor;
            int data;
            char *frase;
            char categoria[100];
            categoriavalor = strtok(NULL,"#");

            if (strcmp(categoriavalor,"VIDEO")==0||strcmp(categoriavalor,"MUSIC")==0||strcmp(categoriavalor,"SOCIAL")==0){
                //apenas a categoria
                strcpy(categoria,categoriavalor);
                //valor 
                categoriavalor = strtok(NULL,"#");
                data = atoi(categoriavalor);

                //sem_wait(mutexauth);

                if (strcmp(categoria, "VIDEO") == 0){
                    sdptr->data_total[0] += data;
                    sdptr->total_reqs[0] += 1;
                }
                else if (strcmp(categoria, "MUSIC") == 0){
                    sdptr->data_total[1] += data;
                    sdptr->total_reqs[1] += 1;
                }
                else{
                    sdptr->data_total[2] += data;
                    sdptr->total_reqs[2] += 1;
                }
                //sem_post(mutexauth);
                   
                   
            }

            

        }else{
            printf("Pedido vindo do backoffice\n");
            comando = strtok(NULL,"#");
    
            if (strcmp(comando,"data_stats")==0){
                printf("Backoffice pediu data_stats\n");
        
                //sem_wait(mutexauth);
                data_stats();
                //sem_post(mutexauth);

            }else{
                printf("Backoffice pediu reset\n");
                //sem_wait(mutexauth);
                reset();
                //sem_post(mutexauth);
                
            }
        

        }
    }
    
    exit(0);
}

void send_to_logs(const char *message) {
    pthread_mutex_lock(&mutexlog);
    FILE *logs = fopen("logs.txt", "a");
    if (logs == NULL) {
        perror("Erro ao abrir o arquivo de log");
        return;
    }
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s",message);
    fprintf(logs, "[%02d:%02d:%02d] %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
    fclose(logs);
    pthread_mutex_unlock(&mutexlog);
}


// Função para ler as configurações do arquivo
SystemManager read_config_file(const char* filename) {
    SystemManager config;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de configuração");
        exit(EXIT_FAILURE);
    }
    if (fscanf(file, "%d %d %d %d %d %d", &config.mobile_users, &config.queue_pos,
               &config.auth_servers, &config.auth_proc_time, &config.max_video_wait,
               &config.max_others_wait) != 6) {
        perror("Erro ao ler as configurações do arquivo");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return config;
}

// Função para validar as configurações lidas
void validate_config(SystemManager config) {
    if (config.mobile_users < 0 ||
        config.queue_pos < 0 ||
        config.auth_servers < 1 ||
        config.auth_proc_time < 1 ||
        config.max_video_wait < 1 ||
        config.max_others_wait < 1) {
        fprintf(stderr, "Erro: As configurações no arquivo são inválidas.\n");
        exit(EXIT_FAILURE);
    }
}


 
//libertar os recursos
void cleanup() {
    if (msgctl(mqid, IPC_RMID, NULL) == -1) {
        perror("Erro ao liberar a Message Queue");
        exit(EXIT_FAILURE);
    }

    // Libertar a memória compartilhada
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erro ao liberar a memória compartilhada");
        exit(EXIT_FAILURE);
    }

    // Libertar os named pipes
    if (unlink(PIPE1) == -1) {
        perror("Erro ao remover o named pipe 1");
        exit(EXIT_FAILURE);
    }

    if (unlink(PIPE2) == -1) {
        perror("Erro ao remover o named pipe 2");
        exit(EXIT_FAILURE);
    }

    // Destrua o mutex
    pthread_mutex_destroy(&mutex1);

    // Terminar as threads
    pthread_cancel(receiver);
    pthread_cancel(sender);

    exit(EXIT_SUCCESS);
}

//signal ctrlc
void sigint_handler(int signum) {
    send_to_logs("SIGNAL SIGINT RECEIVED\n");
    cleanup();
    exit(signum);
}

//receiverThread
void *receiverThread(void *arg) {
    fd_set read_set;

	int pipes[2];
  	
  	if ((pipes[0]=open("USERpipenamed", O_RDWR)) < 0){
        perror("Cant open user: ");
        exit(0);
 	}


  	if ((pipes[1]=open("BACKpipenamed", O_RDWR)) < 0){
        perror("Cant open back: ");
        exit(0);
 	}


    while(1){
        FD_ZERO(&read_set);
        for (int i=0; i<2; i++){
            FD_SET(pipes[i], &read_set);
        }
		if (select(pipes[1]+1, &read_set, NULL, NULL, NULL) > 0 ) {
		    for (int j=0; j<2; j++)  {
                if (FD_ISSET(pipes[j], &read_set)) {
                    pthread_mutex_lock(&mutex1);
                    char buf[BUF_SIZE],fraseinteira[BUF_SIZE];
                    int n = 0;
                        n = read(pipes[j], buf, BUF_SIZE);
                        if (n > 0) {
                            char* categoria;
                            char* token;
                            token = strtok(buf,"\n");
                            strcpy(fraseinteira,token);
                            categoria = strtok(buf,"#");
                            categoria = strtok(NULL,"#");
                            if (strcmp(categoria,"VIDEO")==0){		
                               enqueue(&video_queue,fraseinteira);	   	
                            }
                            else{
                              enqueue(&other_queue,fraseinteira);    
                            } 
                        }
                    pthread_mutex_unlock(&mutex1);
                }
            }
        } 
    }
}

//sender thread
void *senderThread(void *arg) {
    char string[100];
    
    while (1) {
        pthread_mutex_lock(&mutex1);

        if (!isEmpty(&video_queue)) {
            strcpy(string, video_queue.items[video_queue.front]);
            dequeue(&video_queue);
            write(pipesengine[0][1], string, strlen(string) + 1);
            send_to_logs("SENDER: VIDEO AUTHORIZATION REQUEST SENT FOR PROCESSING ON AUTHORIZATION_ENGINE 1");
        }

        if (!isEmpty(&other_queue)) {
            strcpy(string, other_queue.items[other_queue.front]);
            dequeue(&other_queue);
            write(pipesengine[0][1], string, strlen(string) + 1);
            send_to_logs("SENDER: OTHER AUTHORIZATION REQUEST SENT FOR PROCESSING ON AUTHORIZATION_ENGINE 1");


        }

        pthread_mutex_unlock(&mutex1);
    }

    pthread_exit(NULL);
}


// Função para iniciar o sistema com base nas configurações lidas
void start_system(SystemManager config) {
    // Implemente aqui a lógica para iniciar o sistema com base nas configurações lidas
    printf("Sistema iniciado com sucesso!\n");
    printf("Configurações lidas do arquivo:\n");
    printf("Mobile Users: %d\n", config.mobile_users);
    printf("Queue Pos: %d\n", config.queue_pos);
    printf("Authorization Servers: %d\n", config.auth_servers);
    printf("Authorization Processing Time: %d ms\n", config.auth_proc_time);
    printf("Max Video Wait: %d ms\n", config.max_video_wait);
    printf("Max Others Wait: %d ms\n", config.max_others_wait);
}

int main() {

    send_to_logs("PROCESS SYSTEM_MANAGER CREATED");
    
    //filas video e other
    initQueue(&video_queue);
    initQueue(&other_queue);

    // Criação memória partilhada
    shmid = shmget(IPC_PRIVATE, sizeof(shareddata), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erro ao criar memória compartilhada");
        exit(0);
    }
    printf("Memória compartilhada criada com sucesso (ID: %d).\n", shmid);


    sdptr = (shareddata *)shmat(shmid, NULL, 0);
    if (sdptr == (shareddata *) -1) {
        perror("Erro ao associar memória compartilhada");
        exit(0);
    }


    //Criação da Message Queue
    mqid = msgget(IPC_PRIVATE, IPC_CREAT|0777);
    if (mqid < 0){
        perror("Creating message queue");
        exit(0);
    }
    
    
    SystemManager config = read_config_file(CONFIG_FILE);
    validate_config(config);
    start_system(config);

    pid_t autorization = fork();
    if (autorization == 0) {
        send_to_logs("PROCESS AUTHORIZATION_REQUEST_MANAGER CREATED\n");
        
        
        //Criação dos named pipes USER_PIPE e BACK_PIPE
        if((mkfifo(PIPE1,O_CREAT|O_EXCL|0666)<0) && (errno!=EEXIST)){
            perror("Cannot create USER pipe");
            exit(0);
        }

        if((mkfifo(PIPE2,O_CREAT|O_EXCL|0666)<0) && (errno!=EEXIST)){
            perror("Cannot create named pipe");
            exit(0);
        }

        if (pthread_create(&receiver, NULL, receiverThread, NULL) != 0) {
            perror("Erro ao criar a thread receiver\n");
            exit(EXIT_FAILURE);
        }
        
        send_to_logs("THREAD RECEIVER CREATED\n");
        // Criação da thread sender
        if (pthread_create(&sender, NULL, senderThread, NULL) != 0) {
            perror("Erro ao criar a thread sender\n");
            exit(EXIT_FAILURE);
        }
        send_to_logs("THREAD SENDER CREATED\n");

        pipe(pipesengine[0]);

        
        pid_t autorizationengine = fork();
        if (autorizationengine == 0) {
            send_to_logs("AUTHORIZATION_ENGINE 1 READY\n");
            authorizationenginefunc(pipesengine[0]);
                
        }

        // Espera pelas threads terminarem
        pthread_join(receiver, NULL);
        pthread_join(sender, NULL);

       
        exit(0);
    } 

    pid_t  monitorengine = fork();
    if (monitorengine == 0) {
        send_to_logs("PROCESS MONITOR_ENGINE CREATED\n");
        exit(0);
    }

    signal(SIGINT, sigint_handler);

    while(1){
    }

    
    return 0;
}
