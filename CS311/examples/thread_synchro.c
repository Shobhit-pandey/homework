#include <pthread.h>
#include <stdio.h>
#include <mt.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 512
#define LENGTH 300000000

double *a_t;
double *b_t;
double sum;

pthread_mutex_t mutex;


double two_norm(double *a, double *b, long length){
	double ret_val = 0.0;

	for(long i = 0; i < length; ++i){
		ret_val += a[i] * b[i];
	}
	
	return ret_val;
}

void *two_norm_thread(void *arg)
{
	long id = (long)arg;
	long length = LENGTH;

	double partial_sum = 0.0;

	
	for(long i = id * (length / NUM_THREADS); i < (id + 1) * (length / NUM_THREADS) && i < length; ++i){
		partial_sum += a_t[i] * b_t[i];
	}
 
	pthread_mutex_lock(&mutex);	
	sum += partial_sum;
	pthread_mutex_unlock(&mutex);
	
	return (void *)0;
}

int main(int argc, char **argv)
{
	pthread_t thread[NUM_THREADS];
	clock_t start;			/* start time for measuring program performance */
	clock_t stop;			/* stop time for measuring program performance */
	double total_time;      /* total time used by all threads */
	long ncpus;
	double norm2;
	double serial_time;
	double serial_overhead;

	pthread_mutex_init(&mutex, NULL);

	sum = 0.0;
	
	init_genrand(47u);
	
	a_t = malloc(LENGTH * sizeof(double));
	b_t = malloc(LENGTH * sizeof(double));

	for(long i = 0; i < LENGTH; ++i){
		a_t[i] = genrand_res53();
		b_t[i] = genrand_res53();
	}

	start = clock();	
	for (long i = 1; i < NUM_THREADS; i++) {
		if (pthread_create(&thread[i], 			 	/* thread */
		                   NULL, 				 	/* thread attribute */
		                   two_norm_thread, 	 		/* function to execute */
		                   (void*) (i)) != 0) {		/* thread argument */
			perror("Cannot create the thread.");
			exit(-1);
		}
	}

	//master thread runs as well
	two_norm_thread((void *)0);


	for (int i = 1; i < NUM_THREADS; i++) {
		pthread_join(thread[i], NULL);
	}
	stop = clock();

	/* calculate total time -- includes all overhead as well */
	total_time = (double)(stop - start) / (double)CLOCKS_PER_SEC;

	start = clock();
	norm2 = two_norm(a_t, b_t, LENGTH);
	stop = clock();

	serial_time = ((double)stop - (double)start) / (double)CLOCKS_PER_SEC;


	printf("two norm is \t\t%f, took %f seconds\n", norm2, serial_time);
	printf("two norm (threaded) is \t%f, took %f seconds\n", sum, total_time);

	free(a_t);
	free(b_t);

	return 0;
}
