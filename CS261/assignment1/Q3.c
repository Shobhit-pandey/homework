/* CS261- Assignment 1 - Q.3*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: This program creates an array of twenty 
 *     numbers and assigns them random values. Outputs these values,
 *     sorts the array of students by test scores, and outputs
 *     them again.
 */
 
#include <stdio.h>
#include <stdlib.h>

/*
 * Swap two elements (i, j) of the array arr
 */
void swap(int* arr, int i, int j) {
    int tmp;
    tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

/*
 * Sort an array of number using the bubble sort
 *  algorithm.
 */
void sort(int* number, int n){
     /*Sort the given array number , of length n*/
     int sorted, i;
     sorted = 0;
     while( sorted != 1 ) {
        sorted = 1;
        for( i = 0; i < n-1; i++ ) {
            if( number[i] > number[i+1] ) {
                swap( number, i, i+1);
                sorted = 0;
            }
        }
    }
}

int main(){
    /*Declare an integer n and assign it a value of 20.*/
    int n;
    n = 20;
    
    /*Allocate memory for an array of n integers using malloc.*/
    int* intArr = (int * ) malloc(sizeof(int) * n);
    
    /*Fill this array with random numbers, using rand().*/
    int i;
    for( i = 0; i < n; i++ ) {
        intArr[i] = rand();
    }
    
    /*Print the contents of the array.*/
    printf("Unsorted Array\n------------\n");
    for( i = 0; i < n; i++ ) {
        printf("%02d: %10d\n", i, intArr[i]);
    }
    
    /*Pass this array along with n to the sort() function of part a.*/
    sort(intArr, n);
    printf("\nSorted Array\n------------\n");
    
    /*Print the contents of the array.*/    
    for( i = 0; i < n; i++ ) {
        printf("%02d: %10d\n", i, intArr[i]);
    }
    
    return 0;
}
