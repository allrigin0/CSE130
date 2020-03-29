#include "cartman.h"
#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 

pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockE = PTHREAD_MUTEX_INITIALIZER;
sem_t mutex; 
int count = 0;
void *helperfunc(void * arg);

pthread_t threads[15]; //15 threads to be created

typedef struct t{
  int cart; 
  enum track track;
  enum junction junction;
} t;
struct t temp[15];

/*
 * You need to implement this function, see cartman.h for details 
 */
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  if (count == 0){ //since cartman() isnt the entry point, I put this here.
    sem_init(&mutex, 0, 1); 
    count++;
  }
  sem_wait(&mutex);
  temp[cart].cart = cart;
  temp[cart].track = track;
  temp[cart].junction = junction;

  // printf("DEBUG>> Arrival of cart %d on track %i at junction %i\n", cart, track, junction);

  pthread_create(&threads[cart], NULL, helperfunc, &temp[cart]);
  pthread_join(threads[cart], NULL);
  sem_post(&mutex); 
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void depart(unsigned int cart, enum track track, enum junction junction) 
{
  // printf("DEBUG>> Departing of cart %d on track %i at junction %i\n", cart, track, junction);
  switch (track){
    case Red: 
      release(cart, B);
      release(cart, A);
      pthread_mutex_unlock(&lockB);
      pthread_mutex_unlock(&lockA);
      break;
    case Green:
      release(cart, C);
      release(cart, B);
      pthread_mutex_unlock(&lockC);
      pthread_mutex_unlock(&lockB);
      break;
    case Blue:
      release(cart, D);
      release(cart, C);
      pthread_mutex_unlock(&lockD);
      pthread_mutex_unlock(&lockC);
      break;
    case Yellow:
      release(cart, E);
      release(cart, D);
      pthread_mutex_unlock(&lockE);
      pthread_mutex_unlock(&lockD);
      break;
    default: //black
      release(cart, A);
      release(cart, E);
      pthread_mutex_unlock(&lockA);
      pthread_mutex_unlock(&lockE);
      break;
  }
  // printf("DEBUG>> COMPLETED cart %d on track %i at junction %i\n", cart, track, junction);
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void cartman() 
{
  for (;;) { }
}

void *helperfunc(void *args){
  
  struct t *arg = (struct t *) args;
  switch (arg->track){
    case Red: 
      pthread_mutex_lock(&lockA);
      pthread_mutex_lock(&lockB);
      reserve(arg->cart, A);
      reserve(arg->cart, B);
      break;
    case Green:
      pthread_mutex_lock(&lockB);
      pthread_mutex_lock(&lockC);
      reserve(arg->cart, B);
      reserve(arg->cart, C);
      break;
    case Blue:
      pthread_mutex_lock(&lockC);
      pthread_mutex_lock(&lockD);
      reserve(arg->cart, C);
      reserve(arg->cart, D);
      break;
    case Yellow:
      pthread_mutex_lock(&lockD);
      pthread_mutex_lock(&lockE);
      reserve(arg->cart, D);
      reserve(arg->cart, E);
      break;
    default: //Black
      pthread_mutex_lock(&lockE);
      pthread_mutex_lock(&lockA);
      reserve(arg->cart, E);
      reserve(arg->cart, A);
      break;
  }
  // printf("DEBUG>> Reserved junctions for cart %d on track %i at junction %i\n", cart, track, junction);
  cross(arg->cart, arg->track, arg->junction);
  // printf("DEBUG>> Crossing cart %d on track %i at junction %i\n", cart, track, junction);
  pthread_exit(NULL);
}