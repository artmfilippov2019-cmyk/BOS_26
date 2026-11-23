#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s PROGRAM [ARGS...]\n", argv[0]);
		return 1;
	}
	
	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return 1;
	}

	if (child == 0) {
		execvp(argv[1], &argv[1]);
		int code = errno == ENOENT ? 127 : 126;
		perror(argv[1]);
		_exit(code);
	}

	printf("Started PID=%ld\n", (long)child);
	fflush(stdout);

	int status;
	while (waitpid(child, &status, 0) < 0) {
		if (errno != EINTR) {
			perror("waitpid");
			return 1;
		}
	}

	if (WIFEXITED(status)) {
		printf("Exit code=%d\n", WEXITSTATUS(status));
		return WEXITSTATUS(status);
	}

	printf("Terminated by signal=%d\n", WTERMSIG(status));
	return 128 + WTERMSIG(status);
}
