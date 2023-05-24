#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
# include <sys/ipc.h>
#include <errno.h>
#include <signal.h>
void wait_sem(int sem_id, unsigned short sem_num) {
    struct sembuf sem_buf = { sem_num, -1, 0 };
    semop(sem_id, &sem_buf, 1);
}

void signal_sem(int sem_id, unsigned short sem_num) {
    struct sembuf sem_buf = { sem_num, 1, 0 };
    semop(sem_id, &sem_buf, 1);
}
int main()
{
// Создание семафоров
key_t key=ftok("t",1);
int sem_id = semget(key,4,IPC_CREAT|0666);
// 0 mutex, 1 t, 2 p, 3 s 
 semctl(sem_id, 1, SETVAL,0 );
 semctl(sem_id, 2, SETVAL, 0);
 semctl(sem_id, 3, SETVAL, 0); 
 semctl(sem_id, 0, SETVAL, 1); 
    // Создание процессов для курильщиков
    pid_t pidT, pidP, pidM;
    pidT = fork();
    if (pidT == 0) {
        // Курильщик T
        while (true) {
            wait_sem(sem_id,1);
            wait_sem(sem_id,0);
            std::cout << "T" << std::endl;
            signal_sem(sem_id,0);
            signal_sem(sem_id,1);
            sleep(1);
        }
    } 
    pidP = fork();
    if (pidP == 0) {
            // Курильщик P
            while (true) {
            wait_sem(sem_id,2);
            wait_sem(sem_id,0);
            std::cout << "P" << std::endl;
            signal_sem(sem_id,0);
            signal_sem(sem_id,2);
            sleep(1);
            }
        } 
     pidM = fork();
     if (pidM == 0) {
                // Курильщик M
                while (true) {
                    wait_sem(sem_id,3);
                    wait_sem(sem_id,0);
                    std::cout << "M" << std::endl;
                    signal_sem(sem_id,0);
                    signal_sem(sem_id,3);
                    sleep(1);
                }
            } 
// Бармен
 
char command;
while (std::cin >> command) {
wait_sem(sem_id,0);
int item;
  if(command=='t'){
    signal_sem(sem_id,1);
    signal_sem(sem_id,0);
  }
  else if (command=='p'){
    signal_sem(sem_id,2);
    signal_sem(sem_id,0);
  }
  else if (command=='m'){
    signal_sem(sem_id,3);
    signal_sem(sem_id,0);
  }
}
 if (std::cin.eof()){
   if(kill(pidT, SIGTERM)!=0){
     std::string errorMessage = std::strerror(errno);
   }
   if(kill(pidP, SIGTERM)!=0){
     std::string errorMessage = std::strerror(errno);
   }
   if(kill(pidM, SIGTERM)!=0){
     std::string errorMessage = std::strerror(errno);
   }
   
 }    
 //не поняла как написать момент когда курильщики выкуривают самокрутку, они должны игнорировать просьбы Бармена покинуть помещение, А моделирующие их процессы завершить свою работу, 
 //после того, как будет завершена операция вывода символа. обработка сигнала для этих процессов?         
    wait(NULL);
    wait(NULL);
    wait(NULL);
    semctl(sem_id, 0, IPC_RMID, 0);
    semctl(sem_id, 1, IPC_RMID, 0);
    semctl(sem_id, 2, IPC_RMID, 0);
    semctl(sem_id, 3, IPC_RMID, 0);
  
  return 1;
}
