#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static long long now_ms(void) {
	struct timespec t;
	if (clock_gettime(CLOCK_MONOTONIC, &t) < 0) {
		perror("clock_gettime");
		exit(1);
	}
	return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

static int wait_until(pid_t child, int *status, long long deadline) {
	for (;;) {
		pid_t result = waitpid(child, status, WNOHANG);
		if (result == child) {
			return 1;
		}
		if (result < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("waitpid");
			return -1;
		}

		long long remaining = deadline - now_ms();
		if (remaining <= 0) {
			return 0;
		}

		long ms = remaining < 10 ? (long)remaining : 10;
		struct timespec pause = {.tv_sec = 0, .tv_nsec = ms * 1000000L};
		nanosleep(&pause, NULL);
	}
}

static int send_signal(pid_t child, int sig) {
	if (kill(child, sig) < 0 && errno != ESRCH) {
		perror("kill");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s SECONDS PROGRAM [ARGS...]\n", argv[0]);
		return 1;
	}

	char *end;
	errno = 0;
	long seconds = strtol(argv[1], &end, 10);
	if (errno || end == argv[1] || *end || seconds < 1 || seconds > INT_MAX) {
		fprintf(stderr, "SECONDS must be a positive integer\n");
		return 1;
	}

	long long deadline = now_ms() + 1000LL * seconds;
	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return 1;
	}
	if (child == 0) {
		execvp(argv[2], &argv[2]);
		int code = errno == ENOENT ? 127 : 126;
		perror(argv[2]);
		_exit(code);
	}

	int status;
	int done = wait_until(child, &status, deadline);
	if (done < 0) {
		return 1;
	}
	
	int timed_out = !done;
	if (timed_out) {
		printf("Timeout. Sending SIGTERM to PID=%ld\n", (long)child);
		if (send_signal(child, SIGTERM) < 0) {
			return 1;
		}

		done = wait_until(child, &status, now_ms() + 1000);
		if (done < 0) {
			return 1;
		}
		if (!done) {
			puts("Grace period expired. Sending SIGKILL");
			if (send_signal(child, SIGKILL) < 0) {
				return 1;
			}
			while (waitpid(child, &status, 0) < 0) {
				if (errno != EINTR) {
					perror("waitpid");
					return 1;
				}
			}
		}
	}
	else {
		puts("Finished within time limit");
	}

	if (WIFEXITED(status)) {
		printf("Exit code=%d\n", WEXITSTATUS(status));
		return timed_out ? 124 : WEXITSTATUS(status);
	}

	printf("Terminated by signal=%d\n", WTERMSIG(status));
	return timed_out ? 124 : 128 + WTERMSIG(status);
}
