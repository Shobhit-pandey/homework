/* CS261- Assignment 1 - Q.1*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: Create an array of 10 students.
 *     Populate each element of the array. Assign a random
 *     id and score to each student. Ouput the array of
 *     students along with statistics regarding their test
 *     scores.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define STUD_NUM 10

struct student{
	int id;
	int score;
};

struct student* allocate(){
     /*Allocate memory for ten students*/
     struct student* s = (struct student*) malloc(sizeof(struct student)*STUD_NUM);
     /*return the pointer*/
     return s;
}

void generate(struct student* students){
     /*Generate random ID and scores for ten students, ID being between 1 and 10, scores between 0 and 100*/

     int i;
     for(i = 0; i < STUD_NUM; i++) {
        students[i].id = (rand() % 10) + 1;
        students[i].score = rand() % 101;
     }
}

void output(struct student* students){
     /*Output information about the ten students in the format:
              ID1 Score1
              ID2 score2
              ID3 score3
              ...
              ID10 score10*/
     int i;
     for( i = 0; i < STUD_NUM; i++ ) {
        printf("ID%d Score%d\n", students[i].id, students[i].score);
     }
}

void summary(struct student* students){
     /*Compute and print the minimum, maximum and average scores of the ten students*/
     int i;
     int min;
     int max;
     int avg;
     min = students[0].score;
     max = students[0].score;
     avg = 0;
     
     int score;
     for( i = 0; i < STUD_NUM; i++ ) {
        score = students[i].score;
        if(score < min) {
            min = score;
        }
        if(score > max) {
            max = score;
        }
        avg = avg + score;
     }
     
     avg = avg/STUD_NUM;
     
     printf("\nScoring Statistics\n");
     printf("--------------------\n");
     printf("Minimum: %d\n", min);
     printf("Maximum: %d\n", max);
     printf("Average: %d\n", avg);
}

void deallocate(struct student* stud){
     /*Deallocate memory from stud*/
     if( stud != 0) {
        free(stud);
     }
}

int main(){
    struct student* stud = NULL;
    
    /*call allocate*/
    stud = allocate();
    /*call generate*/
    generate(stud);
    /*call output*/
    output(stud);
    /*call summary*/
    summary(stud);
    /*call deallocate*/
    deallocate(stud);
    return 0;
}
