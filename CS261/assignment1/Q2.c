/* CS261- Assignment 1 - Q.2*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: Creates three variables x,y,z
 *     and assign them each a different value. When passed
 *     to foo, these values will all change, except z's.
 *     Each variable will then be printed.
 */
 
#include <stdio.h>
#include <stdlib.h>

int foo(int* a, int* b, int c){
    /*Set a to double its original value*/
    *a = 2 * (*a);
    /*Set b to half its original value*/
    *b = (*b)/2;
    /*Assign a+b to c*/
    c = (*a)+(*b);
    /*Return c*/
    return c;
}

int main(){
    /*Declare three integers x,y and z and initialize them to 5, 6, 7 respectively*/
    int x, y, z, fooresult;
    x = 5;
    y = 6;
    z = 7;
    
    /*Print the values of x, y and z*/
    printf("X = %d\n", x);
    printf("Y = %d\n", y);
    printf("Z = %d\n", z);
    
    /*Call foo() appropriately, passing x,y,z as parameters*/
    fooresult = foo(&x, &y, z);
    
    /*Print the values of x, y and z*/
    printf("X = %d\n", x);
    printf("Y = %d\n", y);
    printf("Z = %d\n", z);
    
    /*Print the value returned by foo*/
    printf("The result of foo() is: %d\n", fooresult);
    
    /*Is the return value different than the value of z?  Why?*/
    /* Yes. Because Z was passed by value, unlike x and y which were passed
     * by reference. The assignments to x and y actually changed their values
     * because foo used a reference to them. Z's value did not change
     * because foo was actually using a copy of it's value.
     */
    return 0;
}
    
    
