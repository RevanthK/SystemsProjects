/* C program for Merge Sort */
#include<stdlib.h> 
#include<stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "multiThreadSorter.h"

// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 

void merge(struct Node arr[], int l, int m, int r, bool isAlpha) { 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    struct Node L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) { 
        if(isAlpha) {
            if (strcasecmp(L[i].sortingVal,R[j].sortingVal) < 0) { 
                arr[k] = L[i]; 
                i++; 
            } 
            else { 
                arr[k] = R[j]; 
                j++; 
            } 
            k++; 
        }
        else {
            if (atof(L[i].sortingVal) < atof(R[j].sortingVal)) { 
                arr[k] = L[i]; 
                i++; 
            } 
            else { 
                arr[k] = R[j]; 
                j++; 
            } 
            k++; 
        }
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(struct Node arr[], int l, int r, bool isAlpha) { 
    if (l < r) { 

        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m, isAlpha); 
        mergeSort(arr, m+1, r, isAlpha); 
  
        merge(arr, l, m, r, isAlpha); 
    } 
} 