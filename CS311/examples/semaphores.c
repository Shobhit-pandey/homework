#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
  semaphores come in 2 flavors: named and unnamed
  named: exist in the file system
  unnamed: logical construct
 */

//sem_open is variadic: sem_t *sem_open(const char *name, int oflag, ...);

int main(int argc, char **argv)
{
	int count;
	sem_t *sem = sem_open("/my_very_own_semaphore", O_CREAT, 0777, 2);
	
	printf("%d: About to wait on semaphore\n", getpid()); fflush(stdout);
	sem_wait(sem);
	
	printf("%d: obtained semaphore, Beginning \"work\"\n", getpid()); fflush(stdout);
	sleep(1);

	sem_getvalue(sem, &count); fprintf(stderr, "count: %d\n", count); fflush(stderr);

	sem_post(sem);
	sem_getvalue(sem, &count); fprintf(stderr, "count: %d\n", count); fflush(stderr);
	sem_post(sem);
	sem_getvalue(sem, &count); fprintf(stderr, "count: %d\n", count); fflush(stderr);
	sem_post(sem);
	sem_getvalue(sem, &count); fprintf(stderr, "count: %d\n", count); fflush(stderr);
	sem_post(sem);
	sem_getvalue(sem, &count); fprintf(stderr, "count: %d\n", count); fflush(stderr);
	
	printf("%d: released semaphore\n", getpid()); fflush(stdout);

	sem_close(sem);
	sem_unlink("/my_very_own_semaphore");
	return 0;
}
