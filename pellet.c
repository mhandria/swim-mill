//
//  pellet.c
//  swim_mill
//
//  Created by Michael Handria on 10/29/16.
//  Copyright © 2016 Michael Handria. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define Size 210
#define wait 0
#define go   1

const key_t key = 9456;
int shmid;
int sleeping;
char (*pool)[11][10];

void *child(int *location);
void killProcess();
void endProcess();

FILE *fp;


//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]){

//CONNECT SHARED MEMORY
////////////////////////////////////////////////////



    //connencts the shared memory id

    //the starting point will be mem# 9886

    //the size will be 100 bytes

    //will have read and write access for all three
    //users
    if((shmid = shmget(key, Size, 0666))< 0){
        perror("shmget");
        exit(1);
    }

    if((pool =  (char(*)[11][10])shmat(shmid, NULL, 0)) == (char(*)[11][10]) -1){
        perror("shmat");
        exit(1);
    }
///////////////////////////////////////////////////////////

//SET UP SIGNALS
///////////////////////////////////////////////////////////
  signal(SIGINT, killProcess);

  //user defined signal (can be for any user)

  signal(SIGUSR1, endProcess);
///////////////////////////////////////////////////////////


//SET UP RANDOM GENERATOR
///////////////////////////////////////////////////////////
    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));
////////////////////////////////////////////////////////////
    fp = fopen("./log.txt", "a");
    pthread_t pellet[14];
    //int uid = 3;

    //for(int i = 0; i < 6; i++){
      /*int xaxis[2] = {0, 0};
      int x1 = rand()%8;
      int x2 = rand()%8;
      if(x1 > x2){
        *(xaxis) = x2;
        *(xaxis+1) = x1;
      }
      else{
        *(xaxis) = x1;
        *(xaxis+1) = x2;
      }*/
      for(int i = 0; i < 15; i++){
        //int xaxis = rand()%8;
        int position[2] = {rand()%5, rand()%10};
        //printf("location: %d, %d", *position, *(position+1));
        //*pool[10][uid+i] = 1;
        pthread_create(&pellet[i], NULL, child, position);
        sleeping = (rand()%3)+1;
        sleep(sleeping);
      }
      for(int i = 0; i < 15; i++){
        pthread_join(pellet[i], NULL);
      }
    //}
}


void *child(int *location){
  int r = *location;
  int col = *(location+1);
  fprintf(fp, "\nPellet created @ col: %d row: %d\n",  col, r);
  printf("\nPellet created @ col: %d row: %d\n",  col, r);

  for(int row = r; row <= 9; row++){

    if(*pool[row+1][col] == 'F' || *pool[row][col+1] == 'F' || *pool[row][col-1] == 'F'){



      *pool[row][col] = '.';
      *pool[row+1][col] = 'F';
      fprintf(fp, "\nPellet %d was EATEN at column %d!\n", pthread_self(), col);
      printf("\nPellet %d was EATEN at column %d!\n", pthread_self(), col);

      pthread_exit(0);
      break;
    }
    else{
      if(row+1 == 10){

        *pool[row][col] = '.';
        fprintf(fp, "\nPellet %d LEFT at column %d!\n", pthread_self(), col);
        printf("\nPellet %d LEFT at column %d!\n", pthread_self(), col);

      }
      else{

        *pool[row][col] = '.';
        *pool[row+1][col] = 'o';

      }
    }
    sleep(1);
  }

  pthread_exit(0);
}

void killProcess(){
  shmdt(pool);
  printf("\nInterrupt signal recieved PID: %d pellet killed\n", getpid());
  fclose(fp);
  exit(0);
}

void endProcess(){
  shmdt(pool);
  printf("\nTime limit reached PID: %d pellet killed\n", getpid());
  fclose(fp);
  exit(0);
}
