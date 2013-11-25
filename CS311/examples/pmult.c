#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "mt.h"


double **matrix_A;
double **matrix_B;
double **matrix_C;

int thread_num;
int dimension;
int print;

void *mult_matrix(void *arg)
{
	int i;
	int j;
	int k;

	long tid = (long) arg;

	int end;
	int start;

	start = (tid * dimension) / thread_num;
	end = ((tid + 1) * dimension) / thread_num;

	for(i = start; i < end; ++i){
		for(j = 0; j < dimension; ++j){
			matrix_C[i][j] = 0;
			for(k = 0; k < dimension; ++k){
				matrix_C[i][j] += matrix_A[i][k] * matrix_B[k][j];
			}
		}
	}

	return 0;
}

void fill_matrix(double **matrix)
{
	int i;
	int j;

	for(i = 0; i < dimension; ++i){
		for(j = 0; j < dimension; ++j){
			matrix[i][j] = genrand_res53();
		}
	}
}

void print_matrix(double **matrix)
{
	int i;
	int j;

	printf("[");
	for(i = 0; i < dimension; ++i){
		printf("[");
		for(j = 0; j < dimension; ++j){
			if (j == dimension - 1){
				printf("%1.10f", matrix[i][j]);
			}else{
				printf("%1.10f\t", matrix[i][j]);

			}
		}
		printf("]\n");
	}
	printf("]\n");
}


int main(int argc, char **argv)
{

	pthread_t *thread;
	pthread_attr_t attr;

	long i;

	thread_num = atoi(argv[1]);
	dimension = atoi(argv[2]);
	print = atoi(argv[3]);
	
	matrix_A = (double **)malloc(sizeof(double *) * dimension);
	matrix_B = (double **)malloc(sizeof(double *) * dimension);
	matrix_C = (double **)malloc(sizeof(double *) * dimension);
	
	for(i = 0; i < dimension; ++i){
		matrix_A[i] = (double *)malloc(sizeof(double) * dimension);
		matrix_B[i] = (double *)malloc(sizeof(double) * dimension);
		matrix_C[i] = (double *)malloc(sizeof(double) * dimension);
	}

	unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};
	unsigned long length = 4;
	init_by_array(init, length);

	fill_matrix(matrix_A);
	fill_matrix(matrix_B);

	thread = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);

	pthread_attr_init(&attr);

	for(i = 1; i < thread_num; ++i){
		if( pthread_create(&thread[i], &attr, mult_matrix, (void*)i) != 0){
			perror("thread creation failure");
			exit(-1);
		}
	}

	mult_matrix(0);

	for(i = 1; i < thread_num; ++i) pthread_join(thread[i], NULL);
	
	if (print){
		print_matrix(matrix_A);
		print_matrix(matrix_B);
		print_matrix(matrix_C);
	}

	return 0;
}
