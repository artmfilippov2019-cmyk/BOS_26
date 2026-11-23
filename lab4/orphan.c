#include <stdio.h>
#include <unistd.h>

int main(void) {
	int ready[2];
	if (pipe(ready) < 0) {
		perror("pipe");
		return 1;
	}

	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return 1;
	}

	if (child == 0) {
		close(ready[0]);
		pid_t old_parent = getppid();
		
		printf("Child PID=%ld, old PPID=%ld\n",
			(long)getpid(), (long)old_parent);
		fflush(stdout);

		if (write(ready[1], "x", 1) != 1) {
			perror("write");
			return 1;
		}
		close(ready[1]);

		pid_t new_parent;
		while ((new_parent = getppid()) == old_parent) {
			usleep(10000);
		}

		printf("Child PID=%ld, new PPID=%ld\n",
			(long)getpid(), (long)new_parent);
		fflush(stdout);
		sleep(15);
		return 0;
	}

	close(ready[1]);
	char byte;
	if (read(ready[0], &byte, 1) != 1) {
		fprintf(stderr, "Child did not confirm readiness");
		return 1;
	}
	close(ready[0]);

	printf("Original parent PID=%ld exits\n", (long)getpid());
	return 0;
}
