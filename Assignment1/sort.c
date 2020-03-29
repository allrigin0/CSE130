#include "merge.h"
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h> 
#include <unistd.h> 
#include <string.h>

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 *  This function performs mergesort using one fork() instance, which creates one child process
 * that will sort the right side while the parent process sorts the left side and waits for the child
 * This should make the execution time much faster than it's single-process-merge-sort counterpart.
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  int n = right + 1; // array size
  int middle = (left+right)/2;
  int mid = middle + 1;

  key_t k = ftok("ryjsun", 65); //copied from lecture 6 slides
  int shmid = shmget(k, 1024, 0666|IPC_CREAT); //copied from lecture 6 slides
  int *sharedmem = (int*)shmat(shmid, (void*)0, 0);  //copied from lecture 6 slides, using integer shared memory values.

  memcpy(sharedmem, arr, sizeof(*arr));  //initalize shared mem as arr[]
  int child = fork(); //first child, return value is either negative, zero, or positive.
  switch(child) { //switch on the process id returned by fork()
    case -1: //if either process returns a negative id, it means fork error
      printf("Fork Error\n");
      _exit(0); //the exit command terminates the current process.
    case 0: // fork() returns 0, this is the child process's part
      singleProcessMergeSort(arr, mid, right); // sort the right side
      int i = mid;
      while (i < n) { // copy sorted values into sharedmem, right side. 
        sharedmem[i] = arr[i]; 
        i++;
      }
      shmdt(sharedmem); //detach the shared memory 
      _exit(0); //the exit command terminates the current process.
    default: // positive value means returned to parent process; parent process does this part
      singleProcessMergeSort(arr, left, middle);// sort the left side
      int l = left;
      while ( l < mid ){ //copy the arr values into shared memory, shared mem, left side.
        sharedmem[l] = arr[l];
        l++;
      }
      wait(NULL);  //system call to wait for child process to terminate, pass it NULL.
  }
  int j = left;
  while (j < n){ //finally, copy from sorted shared memory to arr for merge().
    arr[j] = sharedmem[j];  
    j++;
  }
  merge(arr, left, middle, right); //merge magic
  shmdt(sharedmem); //detach the shared memory 
  shmctl(shmid, IPC_RMID, NULL); //delete the shared memory found at http://man7.org/linux/man-pages/man2/shmctl.2.html.
}
