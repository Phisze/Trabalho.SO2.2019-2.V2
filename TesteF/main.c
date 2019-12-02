#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include <sys/unistd.h>

#define N 6
#define PENSAR 0
#define FOME 1
#define COMER 2
#define ESQUERDA (nfilosofo+4)%N //agarrar garfo
//da esquerda
#define DIREITA (nfilosofo+1)%N  //agarrar garfo
//da direit

typedef struct {
    int id;
    char nome[20];
    int prioridade;
    int status;

} pessoa_t;

void *filosofo(void *num);

void agarraGarfo(int);

void setPrioridades(char);

void verificaMaior(int);

void deixaGarfo(int);

void testar(int);

sem_t mutex;
sem_t S[N]; //inicializacao do semáforo
int estado[N];
int listaEspera[N] = {0, 0, 0, 0, 0, 0};
int assento = 0;
int maior = 0;

pessoa_t passageiros[] = {
        {0, "Antonio", 2, PENSAR},
        {1, "Antonia", 2, PENSAR},
        {2, "Bruno",   1, PENSAR},
        {3, "Bruna",   1, PENSAR},
        {4, "Carlos",  0, PENSAR},
        {5, "Carla",   0, PENSAR}
};

void *filosofo(void *num) {
    while (1) {
        int *i = num;
        sleep(1);
        agarraGarfo(*i);
        sleep(1);
        deixaGarfo(*i);
    }
}

void agarraGarfo(int nfilosofo) {
    sem_wait(&mutex);
    estado[nfilosofo] = FOME;
    listaEspera[nfilosofo] = 1;
    printf("Filosofo %s[%d] tem fome.\n", passageiros[nfilosofo].nome, passageiros[nfilosofo].prioridade);
    //+1 para imprimir filosofo 1 e nao filosofo 0
    testar(nfilosofo);
    sem_post(&mutex);
    sem_wait(&S[nfilosofo]);
    //sleep(1);
}

int maiorT() {
    int maiorF = passageiros[0].prioridade;
    int retorno = 0;
    for (int i = 0; i < N; i++) {
        //printf("LISTAESPERA I %d ", listaEspera[i]);
        if (listaEspera[i] == 1) {
            if (passageiros[i].prioridade > maiorF) {
                //printf("PASSOU %d PRIORIDADE DE I: %d", i, passageiros[i].prioridade);
                retorno = i;
                maiorF = passageiros[i].prioridade;
            }
        }
    }
    //printf("%s RETORNO %d[%d]\n", passageiros[retorno].nome, retorno, maiorF);
    return retorno;
}

void deixaGarfo(int nfilosofo) {
    sem_wait(&mutex);
    assento = 0;
    maior = 0;
    setPrioridades(passageiros[nfilosofo].nome[0]);
    estado[nfilosofo] = PENSAR;
    //printf("Filosofo %d deixou os garfos %d e %d.\n", nfilosofo + 1, ESQUERDA + 1, nfilosofo + 1);
    printf("Filosofo %s[%d] esta a pensar.\n", passageiros[nfilosofo].nome, passageiros[nfilosofo].prioridade);
    testar(maiorT());
    //testar(passageiros[nfilosofo+2].id);
    sem_post(&mutex);
}

void verificaMaior(int nfilosofo) {
    maior = 0;
    for (int i = 0; i < N; i++) {
        if (listaEspera[i] == 1) {
            if (passageiros[i].prioridade > passageiros[nfilosofo].prioridade) {
                maior = 1;
            }
        }
    }
}

void setPrioridades(char primeiraLetra) {
//    printf("PRIMEIRA %c\n", primeiraLetra);
//    if(primeiraLetra == 'A'){
//        passageiros[0].prioridade = 0;
//        passageiros[1].prioridade = 0;
//        passageiros[2].prioridade = 1;
//        passageiros[3].prioridade = 1;
//        passageiros[4].prioridade = 2;
//        passageiros[5].prioridade = 2;
//    }
//    if(primeiraLetra == 'B'){
//        passageiros[0].prioridade = 1;
//        passageiros[1].prioridade = 1;
//        passageiros[2].prioridade = 0;
//        passageiros[3].prioridade = 0;
//        passageiros[4].prioridade = 2;
//        passageiros[5].prioridade = 2;
//    }
//    if(primeiraLetra == 'C'){
//        passageiros[0].prioridade = 2;
//        passageiros[1].prioridade = 2;
//        passageiros[2].prioridade = 1;
//        passageiros[3].prioridade = 1;
//        passageiros[4].prioridade = 0;
//        passageiros[5].prioridade = 0;
//    }
    for (int i = 0; i < N; i++) {
        passageiros[i].prioridade = (passageiros[i].prioridade + 1) % 3;
        //printf("%s PRIORIDADE: %d\n", passageiros[i].nome, passageiros[i].prioridade);
    }
}

void testar(int nfilosofo) {
    //maiorT(nfilosofo);
    //printf("MAIOR: %d\n", maior);
    if (estado[nfilosofo] == FOME && assento == 0 && maior == 0) {
        assento = 1;
        listaEspera[nfilosofo] = 0;
        estado[nfilosofo] = COMER;
        sleep(2);
        //printf("Filosofo %d agarrou os garfos %d e %d.\n", nfilosofo + 1, ESQUERDA + 1, nfilosofo + 1);
        printf("Filosofo %s[%d] esta a comer.\n", passageiros[nfilosofo].nome, passageiros[nfilosofo].prioridade);
        sem_post(&S[nfilosofo]);
    }
}

int main() {
    int i;
    pthread_t thread_id[N]; //identificadores das
    //threads
    sem_init(&mutex, 0, 1);
    for (i = 0; i < N; i++)
        sem_init(&S[i], 0, 0);
    for (i = 0; i < N; i++) {
        pthread_create(&thread_id[i], NULL, filosofo, &passageiros[i].id);
        //criar as threads
        printf("Filosofo %s[%d] esta a pensar.\n", passageiros[i].nome, passageiros[i].prioridade);
    }
    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL); //para
    //fazer a junção das threads
    return (0);
}