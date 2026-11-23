#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
	int fd[2];
	if (pipe(fd) < 0) {
		perror("pipe");
		return 1;
	}

	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return 1;
	}

	if (child == 0) {
		close(fd[0]);
		
		FILE *out = fdopen(fd[1], "w");
		if (!out) {
			perror("fdopen");
			_exit(1);
		}

		int failed = fputs("first line\nsecond line\nthird line\n", out) == EOF;
		if (fclose(out) == EOF) {
			failed = 1;
		}
		if (failed) {
			perror("write pipe");
		}
		_exit(failed);
	}

	close(fd[1]);

	int failed = 0;
	FILE *in = fdopen(fd[0], "r");
	if (!in) {
		perror("fdopen");
		close(fd[0]);
		failed = 1;
	}
	else {
		char line[128];
		while (fgets(line, sizeof line, in)) {
			fputs(line, stdout);
		}

		if (ferror(in)) {
			perror("read pipe");
			failed = 1;
		}
		else {
			puts("Parent received EOF");
		}
		fclose(in);
	}

	int status;
	if (waitpid(child, &status, 0) < 0) {
		perror("waitpid");
		return 1;
	}
	return failed || !WIFEXITED(status) || WEXITSTATUS(status) != 0;
}
