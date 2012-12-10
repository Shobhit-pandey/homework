/* CS261- Assignment 1 - Q.4*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: Creates an array of ten students and
 *     assigns them random ids and scores. Outputs these values,
 *     sorts the array of students by test scores, and outputs
 *     them again.
 */
 
#include <stdio.h>
#include <stdlib.h>

struct student{
	int id;
	int score;
};

/*
 * Swap two given elements in the array of students
 */
void swap(struct student* arr, int i, int j) {
    struct student tmp;
    tmp = arr[i];
    arr[i]= arr[j];
    arr[j] = tmp;
}

/*
 * Sort the array of students using the bubble sort
 *  algorithm.
 */
void sort(struct student* students, int n){
     /*Sort the n students based on their score*/
     int sorted, i;
     sorted = 0;
     while( sorted != 1 ) {
        sorted = 1;
        for( i = 0; i < n-1; i++ ) {
            if( students[i].score > students[i+1].score ) {
                swap( students, i, i+1);
                sorted = 0;
            }
        }
    }
}

int main(){
    /*Declare an integer n and assign it a value.*/
    int n;
    n = 10;
    
    /*Allocate memory for n students using malloc.*/
    struct student* students = (struct student*) malloc(sizeof(struct student)*n);
    
    /*Generate random IDs and scores for the n students, using rand().*/
    int i;
    for( i = 0; i < n; i++ ) {
        students[i].id = (rand() % 10) + 1;
        students[i].score = rand() % 101;
    }
    
    /*Print the contents of the array of n students.*/
    printf("Unsorted Array\n----------\n");
    for( i = 0; i < n; i++ ) {
        printf("ID%3d Score%4d\n", students[i].id, students[i].score);
    }
    
    /*Pass this array along with n to the sort() function*/
    sort(students, n);
    
    /*Print the contents of the array of n students.*/
    printf("\nSorted Array\n----------\n");
    for( i = 0; i < n; i++ ) {
        printf("ID%3d Score%4d\n", students[i].id, students[i].score);
    }
    return 0;
}
