#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s SHMID\n", argv[0]);
		return 1;
	}

	char *end;
	errno = 0;
	long id = strtol(argv[1], &end, 10);
	if (errno || end == argv[1] || *end || id < 0 || id > INT_MAX) {
		fprintf(stderr, "Invalid SHMID\n");
		return 1;
	}

	struct shmid_ds info;
	if (shmctl((int)id, IPC_STAT, &info) < 0) {
		perror("shmctl IPC_STAT");
		return 1;
	}

	const char *memory = shmat((int)id, NULL, SHM_RDONLY);
	if (memory == (void *)-1) {
		perror("shmat");
	}

	size_t length = strnlen(memory, info.shm_segsz);
	fwrite(memory, 1, length, stdout);
	putchar('\n');

	if (shmdt(memory) < 0) {
		perror("shmdt");
		return 1;
	}
	if (shmctl((int)id, IPC_RMID, NULL) < 0) {
		perror("shmctl IPC_RMID");
		return 1;
	}

	puts("Segment removed");
	return 0;
}
