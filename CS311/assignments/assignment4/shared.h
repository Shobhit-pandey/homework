
#ifndef _SHARED_H
#define _SHARED_H

int shared_main(long max, long procs, FILE* output);
int tmp_shared_main(long max, long procs, FILE* output);

void *mount_shmem(char *path, int object_size);
void *attach_shmem(int shmem_fd, int object_size);
void *map_shmem(char *path, int object_size);
void *create_shmem(char *path, int object_size);

#endif
