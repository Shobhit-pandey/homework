/* CS261- Assignment 1 - Q.5*/
/* Name: Trevor Bramwell
 * Date: 10/05/10
 * Solution description: Given a string in any 'case' (e.g. lower, upper)
 *     change the case of the string to sticky caps. (i.e. RaNdOm)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*converts ch to upper case, assuming it is in lower case currently*/
char toUpperCase(char ch){
     return ch-'a'+'A';
}

/*converts ch to lower case, assuming it is in upper case currently*/
char toLowerCase(char ch){
     return ch-'A'+'a';
}

void sticky(char* word){
     /*Convert to sticky caps*/
     int i;     
     for( i = 0; i < strlen(word)-1; i++ ) {        
        if( i % 2 == 0) {
            /* even indexed character */
            if( word[i] >= 'a' && word[i] <= 'z') {
                /* upper case */
                word[i] = toUpperCase(word[i]);
            }
        } else {
            /* odd indexed character */
            if( word[i] >= 'A' && word[i] <= 'Z' ) {
                /* lower case */
                word[i] = toLowerCase(word[i]);
            }
        }
     }
}

int main(){
    /*Input a word*/
    char word[128];
    printf("Input a word to be put in StIcKy CaPs.\n");
    scanf("%s", word);
    
    /*Call sticky*/
    sticky(word);
    
    /*Print the new word*/
    printf("%s\n", word);
    
    return 0;
}
