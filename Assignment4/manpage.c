
/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "manpage.h"
#include "stdio.h"
#include <stdlib.h>
#include <semaphore.h> 
#include <pthread.h>
void *helperfunc(void * arg);


int count = 0;
pthread_t threads[7]; //7 threads to be created
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// sem_t mutex; 

/*
 * See manpage.h for details.
 *
 * As supplied, shows random single messages.
 */
void manpage() 
{
  for (int i = 0; i < 7; i++){
    pthread_create(&threads[i], NULL, helperfunc, NULL);
  }
   for(int i = 0; i < 7; i++){
    pthread_join(threads[i], NULL); 
  }

}

void *helperfunc(void *args){
  int mid = getParagraphId();
  for (;;){ //busy wait :(
    if (mid == count) {
      pthread_mutex_lock(&lock);
      showParagraph();
      count++;
      pthread_mutex_unlock(&lock);
      pthread_exit(NULL);
    }
  }
}