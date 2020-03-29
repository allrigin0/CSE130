#include "merge.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <unistd.h> 
#include <string.h>
#include "pthread.h"
void *helperfunc(void * arg);
/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}
typedef struct t{
  int l,m,r,left,right,index; // bunch of parameters to pass for convenience
  int *arr; //array pointer
} t;
struct t temp[4];
pthread_t threads[4]; //4 threads to be created
/* 
 * This function uses multiple threads to perform mergesort, similar to the previous assignment but uses pthreads instead of forks
 */
void multiThreadedMergeSort(int arr[], int left, int right) 
{
  int middle = (right - left)/2;
  int index[] = {left, middle/2, middle/2+1, middle, middle+1, right-middle/2,right-middle/2+1,right}; //hardcoded the indices for convenience
  for(int i = 0; i < 4; i++){ //assign parameters and pass to helperfunction
    temp[i].l = left;
    temp[i].m = middle;
    temp[i].r = right;
    temp[i].index = i;
    temp[i].arr = arr;
    temp[i].left = index[2*i];
    temp[i].right = index[2*i+1];
    pthread_create(&threads[i], NULL, helperfunc, (void *) &temp[i]); //create thread to run helperfunction, passed with args depending on index
  }
  for(int i = 0; i < 4; i++){ //this works now :)
    pthread_join(threads[i], NULL); 
  }
  merge(arr, left, middle, right);
}
void *helperfunc(void *args){
  struct t *arg = (struct t *) args;
  singleThreadedMergeSort(arg->arr,arg->left,arg->right);
  if (arg->index == 0){ //based on secret sauce of lecture 9
    pthread_join(threads[1],NULL);
    merge(arg->arr, arg->l, arg->m/2, arg->m);
  }
  if (arg->index == 2){
    pthread_join(threads[3],NULL);
    merge(arg->arr, arg->m+1, arg->r - arg->m/2, arg->r);
  }
  pthread_exit(NULL);
}