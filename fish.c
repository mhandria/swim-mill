//
//  fish.c
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
const key_t key = 9456;

int found;
int shmid;
char (*pool)[11][10];

void killProcess();
void endProcess();


FILE *fp;

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


  //SIGNAL Setup
  ///////////////////////////////////////////////////////////
    signal(SIGINT, killProcess);
    signal(SIGUSR1, endProcess);
  ////////////////////////////////////////////////////////////

      //initalize fish position
      *pool[9][5] = 'F';
      int yloc = 5;
      int lastyloc = 5;
      //fp = fopen("./log.txt", "a");


      while(1){
          //find the current location of the fish
          for(int c = 0; c < 10; c++){
            if(*pool[9][c] == 'F'){
              yloc = c;
              break;
            }
            else{
              yloc = lastyloc;
            }
          }
          for(int r = 9; r >= 0; r--){
            found = 0;
            for(int c = 9; c >= 0;  c--){
              if(*pool[r][c] == 'o'){
                found = 1;
                if(c > yloc){
                  *pool[9][yloc] = '.';
                  *pool[9][yloc+1] = 'F';
                  break;
                }//end if incre

                else if(c < yloc){
                  *pool[9][yloc] = '.';
                  *pool[9][yloc-1] = 'F';
                  break;
                }//end else decre

                else{
                  *pool[9][yloc] = 'F';
                }
              }//end if pool
            }//end for c
            if(found){
              lastyloc = yloc;
              break;
            }
          }
          sleep(1);

      }//end superloop

}

void killProcess(){
  shmdt(pool);
  //fprintf(fp, "\nInterrupt signal recieved PID: %d Fish killed\n", getpid());
  printf("\nInterrupt signal recieved PID: %d Fish killed\n", getpid());
  fclose(fp);
  exit(0);
}

void endProcess(){
  shmdt(pool);
  //fprintf(fp, "\nTime Limit reached PID: %d Fish Killed\n", getpid());
  printf("\nTime Limit reached PID: %d Fish Killed\n", getpid());
  fclose(fp);
  exit(0);
}
