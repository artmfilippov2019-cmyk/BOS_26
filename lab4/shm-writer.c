#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

int main(void) {
	int id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
	if (id < 0) {
		perror("shmget");
		return 1;
	}

	char *memory = shmat(id, NULL, 0);
	if (memory == (void *)-1) {
		perror("shmat");
		shmctl(id, IPC_RMID, NULL);
		return 1;
	}

	strcpy(memory, "Hello from shared memory!");

	if (shmdt(memory) < 0) {
		perror("shmdt");
		shmctl(id, IPC_RMID, NULL);
		return 1;
	}

	printf("%d\n", id);
	return 0;
}
