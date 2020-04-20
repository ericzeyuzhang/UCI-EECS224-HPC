/* test modification */

/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.hh"
#include <vector>
#include <cstring>

#define PAR_TH 1000

int binarySearch(keytype* A, int N, int target) 
{ 
   
   int l = 0, r = N - 1;
   int mid = 0;
   while (r > l){
      mid = l + (r - l) / 2;
      if (A[mid] <= target){
         l = mid + 1;
      }
      else {
         r = mid;
      }
   }
   return l;
} 



keytype* merge(keytype* A, keytype* B, int a, int b) {
   
   keytype* rval = newKeys(a+b);

   int i = 0;
   int j = 0;
   int ti = 0;

   while (i < a && j < b) {
      if (A[i] < B[j]) {
         rval[ti] = A[i];
         ti++; 
         i++;
      } 
      else {
         rval[ti] = B[j];
         ti++; 
         j++;
      }
   }
   while (i < a) { /* finish up lower half */
      rval[ti] = A[i];
      ti++; 
      i++;
   }
   while (j < b) { /* finish up upper half */
      rval[ti] = B[j];
      ti++; 
      j++;
   }
   return rval;

} 

keytype *Pmerge(keytype* A, keytype* B, int a, int b) {
   keytype* rval = newKeys(a+b);
   if (a+b < 10000){
      int i = 0;
      int j = 0;
      int ti = 0;
      while (i < a && j < b) {
         if (A[i] < B[j]) {
            rval[ti] = A[i];
            ti++; 
            i++;
         } 
         else {
            rval[ti] = B[j];
            ti++; 
            j++;
         }
      }
      while (i < a) { /* finish up lower half */
         rval[ti] = A[i];
         ti++; 
         i++;
      }
      while (j < b) { /* finish up upper half */
         rval[ti] = B[j];
         ti++; 
         j++;
      }
   }
   else{
      int midA = a / 2;
      int midB = binarySearch(B, b, A[midA]);
      int a1 = midA, a2 = a - midA, b1 = midB, b2 = b - midB;
      if (midB == b - 1 && B[midB] <= A[midA]){
         b1 = b;
         b2 = 0;
      }
      keytype *tmp1=NULL, *tmp2=NULL;
      #pragma omp task
      tmp1 = Pmerge(A, B, a1, b1);
      tmp2 = Pmerge(A+a1, B+b1, a2, b2);

      #pragma omp taskwait
      #pragma omp task
      memcpy(rval, tmp1, (a1+b1) * sizeof(keytype));
      memcpy(rval+(a1+b1), tmp2, (a2+b2) * sizeof(keytype));
      #pragma omp taskwait
      if (tmp1 != NULL) delete[] tmp1;
      if (tmp2 != NULL) delete[] tmp2;      
   }
   return rval;

} 
void mergeSort(keytype* A, int N)
{
    if (N < 2) return;
    if (N > PAR_TH){
      #pragma omp task
      mergeSort(A, N/2);
      #pragma omp task
      mergeSort(A+(N/2), N-(N/2));
      /* merge sorted halves into sorted list */
      #pragma omp taskwait
   }
   else{
      sequentialSort(N, A);
   }

   keytype* tmp = Pmerge(A, A+(N/2), N/2, N-(N/2));

   memcpy(A, tmp, sizeof(keytype) * N);
   delete[] tmp;
}


void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
   #pragma omp parallel
   {
      #pragma omp single
      mergeSort(A, N);
   }
}

/* eof */
