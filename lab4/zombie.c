#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int collect = argc == 2 && strcmp(argv[1], "--wait") == 0;
	if (argc != 1 && !collect) {
		fprintf(stderr, "Usage: %s [--wait]\n", argv[0]);
		return 1;
	}

	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return 1;
	}
	if (child == 0) {
		_exit(7);
	}

	printf("Parent PID=%ld, child PID=%ld\n", (long)getpid(), (long)child);

	if (collect) {
		int status;
		if (waitpid(child, &status, 0) < 0) {
			perror("waitpid");
			return 1;
		}
		if (WIFEXITED(status)) {
			printf("Child exit code=%d\n", WEXITSTATUS(status));
		}
	}

	puts("Parent will stay alive for 15 seconds");
	fflush(stdout);
	sleep(15);
	return 0;
}
