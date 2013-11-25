#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

void *attach_shmem(int shmem_fd, int object_size){
	void *addr;

	/* resize it to something reasonable */
	if (ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "failed to resize shared memory object\n");
		exit(EXIT_FAILURE);		
	}

	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED){
		fprintf(stdout, "failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return addr;

}

void *map_shmem(char *path, int object_size){
	int shmem_fd;

	/* open one that has already been created */
	shmem_fd = shm_open(path, O_RDWR, S_IRUSR | S_IWUSR);
	if (shmem_fd == -1){
		fprintf(stdout, "failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return attach_shmem(shmem_fd, object_size);

}


void *create_shmem(char *path, int object_size){
	int shmem_fd;
	
	/* open it with O_CREAT */
	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmem_fd == -1){
		fprintf(stdout, "failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return attach_shmem(shmem_fd, object_size);
}


int main(int argc, char **argv){
	unsigned char *bitmap;
	int bitmap_size = 34000000 / 8 + 1; /* we want to be able to find 5 numbers */

	int *perfect_numbers;
	int perfect_number_count = 10;

	int **memoizer;

	sem_t *sem;
	sem_t *sem2;

	int object_size = 1024 * 1024 * 6; /* 6 MB, since we need just over 4 for bit map */
	
	void *addr = create_shmem("dmcgrath", object_size);

	/* at this point, everything is done as an offset relative to addr */
	/* BE VERY CAREFUL WITH POINTER ARITHMETIC! IT DOESN'T NECESSARILY FUNCTION HOW YOU THINK
	   IT DOES! */

	/* store the semaphore at the start */
	sem = addr;
	sem_init(sem, 1, 1);
	sem2 = addr + sizeof(sem_t);
	sem_init(sem2, 1, 1);

	/* store the bitmap (which we just use as bits) after the semaphore */
	bitmap = (char*)(sem + 2 * sizeof(sem_t));

	/* store the perfect numbers array after the bitmap -- will generate a warning */
	perfect_numbers = (int*)(bitmap + bitmap_size);

	/* if memoizing -- will generate a warning */
	memoizer = (int**)(perfect_numbers + perfect_number_count);

	/* check that they are at different, logical locations */
	fprintf(stdout, "bitmap: %x/%u\nperfect numbers array: %x/%u\nmemoizer: %x/%u\n", 
	        bitmap, bitmap, perfect_numbers, perfect_numbers, memoizer, memoizer);

	fprintf(stdout, "writer attempting to obtain semaphore\n");
	if (sem_wait(sem) == -1){
		fprintf(stdout, "sem_wait failure\n");
	}

	if (sem_wait(sem2) == -1){
		fprintf(stdout, "sem2_wait failure\n");
	}

	fprintf(stdout, "writer obtained semaphore, sleeping for 10 seconds\n");

	sleep(10);

	fprintf(stdout, "woke up, updating array\n");

	perfect_numbers[0] = 6;
	perfect_numbers[1] = 28;
	perfect_numbers[2] = 496;
	perfect_numbers[3] = 8128;
	perfect_numbers[4] = 33550336;

	sem_destroy(sem);

	fprintf(stdout, "writer releasing semaphore\n");
	if (sem_post(sem) == -1){
		fprintf(stdout, "sem_post failure\n");
	}

	if (sem_post(sem2) == -1){
		fprintf(stdout, "sem2_post failure\n");
	}

	fprintf(stdout, "writer released semaphore\n");
	
	return 0;
}
