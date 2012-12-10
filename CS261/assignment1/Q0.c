/* CS261- Assignment 1 - Q. 0*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: Given a pointer to an int
 *    output the value of the pointer, the address of
 *    the pointer, and the address pointed to by the
 *    pointer.
 */
 
#include <stdio.h>
#include <stdlib.h>

/**
 * Take a pointer to a integer and print out
 *  1) The value of the pointer
 *  2) The address pointed to by the pointer
 *  3) The address of the pointer itself
 *
 * pre: int* is not null
 * post: none
 *
 * @param int* - an integer pointer
 */
void fooA(int* iptr){
     /*Print the value of iptr*/
     printf("%d\n", *iptr);
     /*Print the address pointed to by iptr*/
     printf("%p\n", iptr);
     /*Print the address of iptr itself*/
     printf("%p\n", &iptr);
}

/**
 * Declare an int and output information relating to
 * it and a pointer to it.
 */
int main(){
    /*declare an integer x*/
    int x = 2;
    /*print the address of x*/
    printf("%p\n", &x);
    /*Call fooA() with the address of x*/
    fooA(&x);
    /*print the value of x*/
    printf("%d\n", x);
    return 0;
}
