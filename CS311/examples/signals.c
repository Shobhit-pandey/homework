#define _POSIX_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

//in order to print the message
//extern void psignal(int sig, const char *msg);
//extern const char *sys_siglist[];

//to be used with either signal or sigaction sa_handler
void intr_sig(int signal){
	psignal(signal, "Caught it!"); /*not a good idea, in general*/

	printf("Caught signal %d: %s\n", signal, sys_siglist[signal]);
	

	/* exit(EXIT_SUCCESS); */
}


//to be used with sigaction sa_sigaction
void intr_siga(int signal, siginfo_t *siginfo, void *ucontext){
	psignal(signal, "Caught it!"); /*not a good idea, in general*/

	printf("Caught signal %d: %s\n", signal, sys_siglist[signal]);
	
	printf("\tsi_signo=%d, si_code=%d (%s), ",
	       siginfo->si_signo,
	       siginfo->si_code,
	       (siginfo->si_code == SI_USER) ? "SI_USER" :
	       (siginfo->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");

	printf("si_value = %d\n", siginfo->si_value.sival_int);
	printf("\tsi_pid=%ld, si_uid=%ld\n",
	       (long)siginfo->si_pid,
	       (long)siginfo->si_uid);
	/* exit(EXIT_SUCCESS); */
}


int main(int argc, char **argv){
	struct sigaction s;
	struct sigaction t;

	s.sa_handler = intr_sig;
	sigemptyset(&s.sa_mask);
	s.sa_flags = 0;

	sigemptyset(&t.sa_mask);
	t.sa_sigaction = intr_siga;
	t.sa_flags = SA_SIGINFO;

	//use only one
	//sigaction(SIGINT, &s, NULL);
	sigaction(SIGQUIT, &s, NULL);
	//sigaction(SIGKILL, &s, NULL);
	sigaction(SIGINT, &t, NULL);

	/* SIG_DFL, SIG_IGN */

	for(int i = 0;;i++){
		printf("i = %d\n", i);
		sleep(3);
	}
	

	return 0;
}
