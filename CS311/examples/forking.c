#include <stdio.h>
#include <unistd.h>



int main(int argc, char **argv)
{
	//on fork, copy-on-write semantics are used

	int pid;

	switch(pid = fork()){
		
	case -1:
		//oops...
		
	case 0:
		//child specific code goes here

		//we have lots of execs
		execlp("ls", "ls", (char *)NULL);

	default:
		//parent specific code goes here
		
	}
	
	//both parent and child continue executing here

	
	return 0;
}
