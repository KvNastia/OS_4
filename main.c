/*Необходимо решить проблему "Читатели-писатели" с использованием библиотеки PTHREAD с учетом следующих ограничений:
Есть общий ресурс, из которого можно читать или писать.
В системе есть N потоков читателей и M потоков писателей. Они работают так: в бесконечном цикле сначала спят случайный интервал времени, потом пытаются получить доступ на чтение/запись, после получения доступа выполняют эту операцию случайный интервал времени.
Чтение может происходить несколькими нитями одновременно, но не больше K.
Когда какая-то из нитей производит запись, остальные нити не должны иметь доступа к ресурсу.
Если писатель пишет, никто другой не может иметь доступа к ресурсу.
Если писатель хочет записывать, но в это время читатели читают, он должен ждать, пока читатели завершат чтение, но другие читатели уже не могут начинать чтение, пока писатель не начнет запись.
Результаты работы программы должны отображаться в консоли по мере их появления, как показано ниже. Вывод должен быть синхронизированным, т.е. последовательность строк в логе должна соответствовать последовательности операций, выполняемых программой.
*/

#include "/usr/include/pthread.h"
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define MAXTHREAD 5  /* define # readers */
#define MAXREAD 5
#define MAXWRITETHREAD 2

void annouce_access_request();
void access_database();     /* prototypes */

void* reader(void*);
void* writer(void*);

sem_t q;        /* establish que */
int rc = 0;     /* number of processes reading or wanting to */
int wc = 0;
int write_request = 0;

int main()
{
    pthread_t readers[MAXTHREAD],writerTh[MAXWRITETHREAD];
    int index;
    int ids[MAXTHREAD]; /* readers and initialize mutex, q and db-set them to 1 */
    sem_init (&q,0,1);
    for(index = 0; index < MAXTHREAD; index ++) {
        ids[index]=index+1;                             /* create readers and error check */
        if(pthread_create(&readers[index],0,reader,&ids[index])!=0){
            perror("Cannot create reader!");
            _exit(1);
        }
    }

    int writerIndex;
    int writersIds[MAXWRITETHREAD];
    for (writerIndex = 0; writerIndex < MAXWRITETHREAD; writerIndex++) {
        writersIds[writerIndex] = writerIndex + 1;
        if (pthread_create(&writerTh, 0, writer, &writersIds[writerIndex]) != 0) {
            perror("Cannot create writer");     /* create writers and error check */
            _exit(1);
        }
    }

    pthread_join(writerTh,0);
    sem_destroy (&q);
    return 0;
}

void* reader(void*arg)          /* readers function to read */
{
    srand(time(NULL));
    int index = *(int*)arg;
    int can_read;
    while(1) {
        can_read = 1;

        announce_access_request();

        sem_wait(&q);
        if(wc == 0 && write_request == 0 && rc < MAXREAD) {
            rc++;
            printf("Reader %d wants to read | Resource is free\n", index);
        }
        else {
            can_read = 0;
            printf("Reader %d wants to read | Resource is busy\n", index);
        }
        sem_post(&q);

        if(can_read) {
            printf("Reader %d reading\n", index);
            access_database();

            sem_wait(&q);
            rc--;
            sem_post(&q);
            printf("Reader %d stopped reading\n", index);
        }

        sched_yield();
    }
    return 0;
}
;
void* writer(void*arg)      /* writer's function to write */
{
    srand(time(NULL));
    int index = *(int*)arg;
    int can_write;
    while(1){
        can_write = 1;

        announce_access_request();

        sem_wait (&q);
        if(rc == 0 && wc == 0) {
            wc++;
            printf("Writer %d wants to write. | Resource is free\n", index);
        }
        else {
            can_write = 0;
            printf("Writer %d wants to write. | Number of readers %d\n", index, rc);
        }
        write_request++;
        sem_post(&q);

        if(can_write) {
            printf("Writer %d is now writing\n",index);
            access_database();

            sem_wait(&q);
            wc--;
            write_request--;
            sem_post(&q);
            printf("Writer %d stopped writing\n", index);
        }

        sched_yield();
    }
    return 0;
}

void announce_access_request() {
    sleep(rand() % 21);
}

void access_database() {
    sleep(rand() % 21);
}
