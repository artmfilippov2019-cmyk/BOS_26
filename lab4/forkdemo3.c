#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int signal_mode = argc == 2 && strcmp(argv[1], "--signal") == 0;
	if (argc != 1 && !signal_mode) {
		fprintf(stderr, "Usage: %s [--signal]\n", argv[0]);
		return 1;
	}

	pid_t children[3];
	int count = 0;
	int result = 0;

	for (int i = 0; i < 3; i++) {
		pid_t child = fork();

		if (child < 0) {
			perror("fork");
			result = 1;
			break;
		}

		if (child == 0) {
			if (signal_mode && i == 1) {
				raise(SIGTERM);
			}
			_exit(10 + i);
		}

		children[count++] = child;
	}

	for (int i = 0; i < count; i++) {
		int status;
		pid_t pid;

		do {
			pid = waitpid(children[i], &status, 0);
		} while (pid < 0 && errno == EINTR);

		if (pid < 0) {
			perror("waitpid");
			result = 1;
		}
		else if (WIFEXITED(status)) {
			printf("PID=%ld exit=%d\n",
				(long)children[i], WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			printf("PID=%ld signal=%d\n",
				(long)children[i], WTERMSIG(status));
		}
	}

	return result;
}
