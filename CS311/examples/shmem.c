#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *mount_shmem(char *path, int object_size){
	int shmem_fd;
	void *addr;

	/* create and resize it */
	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmem_fd == -1){
		fprintf(stdout, "failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}
	/* resize it to something reasonable */
	if (ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "failed to resize shared memory object\n");
		exit(EXIT_FAILURE);		
	}

	//0xdeadbeef
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED){
		fprintf(stdout, "failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return addr;
}

int main(int argc, char **argv){
	unsigned char *bitmap;
	int bitmap_size = 34000000 / 8 + 1; /* we want to be able to find 5 numbers */

	int *perfect_numbers;
	int perfect_number_count = 10;

	int **memoizer;

	int object_size = 1024 * 1024 * 512; /* 6 MB, since we need just over 4 for bit map */
	
	void *addr = mount_shmem("dmcgrath", object_size);

	/* at this point, everything is done as an offset relative to addr */
	/* BE VERY CAREFUL WITH POINTER ARITHMETIC! IT DOESN'T NECESSARILY FUNCTION HOW YOU THINK
	   IT DOES! */

	/* store the bitmap (which we just use as bits) at the beginning */
	bitmap = addr;

	/* store the perfect numbers array after the bitmap -- will generate a warning */
	perfect_numbers = (int*)(bitmap + bitmap_size);

	/* if memoizing -- will generate a warning */
	memoizer = (int**)(perfect_numbers + perfect_number_count);

	/* check that they are at different, logical locations */
	fprintf(stdout, "bitmap: %x/%u\nperfect numbers array: %x/%u\nmemoizer: %x/%u\n", 
	        bitmap, bitmap, perfect_numbers, perfect_numbers, memoizer, memoizer);

	perfect_numbers[0] = 6;
	perfect_numbers[1] = 28;
	perfect_numbers[2] = 496;
	perfect_numbers[3] = 8128;
	perfect_numbers[4] = 33550336;

	return 0;
}
