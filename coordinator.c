//
//  coordinator.c
//  swim_mill
//
//  Created by Michael Handria on 10/29/16.
//  Copyright © 2016 Michael Handria. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define Size 210
#define wait 0
#define go   1

FILE *fp;
const key_t key = 9456;
//char *pool[10][10];
int shmid;
pid_t fish, pellet;

void killProcess();
void endProcess();

char (*pool)[11][10];

int main(int argc, char *argv[]){
char (*pool)[11][10];
//SET UP SHARED MEMORY
//////////////////////////////////////////////////////

    int countdown = 0;

    //makes the shared memory id

    //the starting point will be mem# 9886

    //the size will be 100 bytes

    //will have read and write access for all three
    //users
    if((shmid = shmget(key, Size, IPC_CREAT|0666))< 0){
        perror("shmget");
        exit(1);
    }

    if((pool =  (char(*)[11][10])shmat(shmid, NULL, 0)) == (char(*)[11][10]) -1){
        perror("shmat");
        exit(1);
    }

///////////////////////////////////////////////////////

//SET UP SIGNALS
///////////////////////////////////////////////////////////
  signal(SIGINT, killProcess);
///////////////////////////////////////////////////////////

//INITIALIZE POOL
///////////////////////////////////////////////////////
    for(int r = 0; r < 10; r++){
      for(int c = 0; c < 10; c++){
        *(pool)[r][c] = '.';
      }
    }
/////////////////////////////////////////////////////////

    //executes pellets
    if((pellet = fork()) == 0){
      *pool[10][0] = wait;
      execv("pellet", argv);
    }

    //executes fish
    if((fish = fork())== 0){
      *pool[10][1] = wait;
      execv("fish", argv);
    }

    *pool[10][2] = go;
    int rmfile = remove("./log.txt");
    fp = fopen("./log.txt", "w+");
    //loop and keep printing the pool
    while(1){
          for(int r = 0; r < 10; r++){
              fprintf(fp, "\n");
              printf("\n");
              for(int c = 0; c < 10; c++){
                fprintf(fp, "%c", *pool[r][c]);
                printf("%c", *pool[r][c]);
              }
          }
      if(countdown == 30){
        printf("\ntimes up\n");
        endProcess();
        exit(1);
      }
      else{
        fprintf(fp, "\n time left %d \n", countdown);
        printf("\n time left %d \n", countdown);
        countdown++;
      }
      sleep(1);
    }

}

void killProcess() {
    // Kill child processes because user entered ^C
    //fprintf(fp, "\nInterupt signal recieved PID: %d Swim Mill killed\n", getpid());
    printf("\nInterupt signal recieved PID: %d Swim Mill killed\n", getpid());
    kill(fish, SIGINT);
    kill(pellet, SIGINT);

    // Detach & deallocate shared memory
    shmdt(pool);
    shmctl(shmid , IPC_RMID, 0);
    fclose(fp);

    exit(0);
}

void endProcess() {
    // Kill child processes because time limit has reached
    //fprintf(fp, "\nTime limit reached PID: %d Swim Mill killed\n", getpid());
    printf("\nTime limit reached PID: %d Swim Mill killed\n", getpid());
    kill(fish, SIGUSR1);
    kill(pellet, SIGUSR1);

    // Detach & deallocate shared memory
    shmdt(pool);
    shmctl(shmid , IPC_RMID, 0);

    fclose(fp);

    exit(0);
}
