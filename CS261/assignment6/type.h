/*	type.h
	
	Defines the type to be stored in the data structure.  These macros
	are for convenience to avoid having to search and replace/dup code
	when you want to build a dynArray of doubles as opposed to ints
	for example.
*/

#ifndef __TYPE_H
#define __TYPE_H

# ifndef KeyType
# define KeyType      char *
# endif

# ifndef ValueType
# define ValueType    int
# endif

# ifndef EQ
# define EQ(A, B) (strcmp((A),(B)) == 0)
# endif


#endif
