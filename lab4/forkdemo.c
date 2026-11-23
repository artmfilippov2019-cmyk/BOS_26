#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
	int value = 10;
	pid_t child = fork();

	if (child < 0) {
		perror("fork");
		return 1;
	}

	if (child == 0) {
		value += 1;
		printf("Child: PID=%ld, PPID=%ld value=%d address=%p\n",
			(long)getpid(), (long)getppid(), value, (void *)&value);
		return 0;
	}

	value += 10;
	printf("Parent: PID=%ld PPID=%ld value=%d address=%p\n",
		(long)getpid(), (long)getppid(), value, (void *)&value);

	if (waitpid(child, NULL, 0) < 0) {
		perror("waitpid");
		return 1;
	}
	return 0;
}
