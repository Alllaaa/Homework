#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
int buffer_size=10;

void wait_sem(int sem_id, int sem_num) {
    struct sembuf sem_buf = { sem_num, -1, 0 };
    semop(sem_id, &sem_buf, 1);
}

void signal_sem(int sem_id, int sem_num) {
    struct sembuf sem_buf = { sem_num, 1, 0 };
    semop(sem_id, &sem_buf, 1);
}
int main(void){
     // разделяемая память
     key_t key = 100;
     int shm_id = shmget(key, buffer_size * sizeof(int), IPC_CREAT | 0666);
     int *buffer  = (int*) shmat(shm_id, NULL, 0);
    //семафоры 
    key = 1000;
    int  sem_id = semget(key, 3, IPC_CREAT | 0666);
    semctl(sem_id, 0, SETVAL, buffer_size); // empty 
    semctl(sem_id, 1, SETVAL, 0); // full 
    semctl(sem_id, 2, SETVAL, 1); // mutex 
    // процесс производитель
    if (fork() == 0) 
    {
        for (int i = 0; i < 100; i++) 
        {
            wait_sem(sem_id, 0); 
            wait_sem(sem_id, 2);
            buffer[i % buffer_size] = i;
            signal_sem(sem_id, 2); 
            signal_sem(sem_id, 1);
        }
        exit(0);
    }
    // процесс потребитель
    if (fork() == 0) 
    {
        for (int i = 0; i < 100; i++) 
        {
            wait_sem(sem_id, 1); 
            wait_sem(sem_id, 2);
            printf("%d\n", buffer[i % buffer_size]);
            signal_sem(sem_id, 2); 
            signal_sem(sem_id, 0); 
        }
        exit(0);
    }


    wait(NULL);
    wait(NULL);
    shmdt(buffer);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, 0);
    semctl(sem_id, 1, IPC_RMID, 0);
    semctl(sem_id, 2, IPC_RMID, 0);

  return 0;
}
