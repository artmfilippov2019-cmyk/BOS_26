#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int show_memory(pid_t pid) {
	char path[80];
	snprintf(path, sizeof path, "/proc/%ld/smaps_rollup", (long)pid);

	FILE *file = fopen(path, "r");
	if (!file) {
		perror(path);
		return 1;
	}

	printf("PID=%ld\n", (long)pid);
	char line[256];
	while (fgets(line, sizeof line, file)) {
		if (strncmp(line, "Shared_Dirty:", 13) == 0 ||
			strncmp(line, "Private_Dirty:", 14) == 0 ||
			strncmp(line, "Pss:", 4) == 0) {
			fputs(line, stdout);
		} 
	}

	int failed = ferror(file);
	if (failed) {
		perror("read smaps_rollup");
	}
	fclose(file);
	return failed;
}

int main(void) {
	const size_t size = 32UL * 1024 * 1024;
	unsigned char *memory = malloc(size);
	if (!memory) {
		perror("malloc");
		return 1;
	}
	memset(memory, 1, size);

	setbuf(stdout, NULL);
	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		free(memory);
		return 1;
	}

	if (child == 0) {
		printf("Child: address=%p first=%u\n", (void *)memory, memory[0]);
		raise(SIGSTOP);

		memset(memory, 2, size / 2);
		printf("Child after write: first=%u\n", memory[0]);
		raise(SIGSTOP);

		free(memory);
		_exit(0);
	}

	printf("Parent: address=%p first=%u\n", (void *)memory, memory[0]);;\

	int result = 0;
	int status;
	for (int step = 0; step < 2; step++) {
		if (waitpid(child, &status, WUNTRACED) < 0) {
			perror("waitpid");
			result = 1;
			break;
		}
		if (!WIFSTOPPED(status)) {
			fprintf(stderr, "Child existed before the measurement\n");
			free(memory);
			return 1;
		}

		puts(step == 0 ? "\nBefore write:" : "\nAfter write:");
		result |= show_memory(getpid());
		result |= show_memory(child);

		if (kill(child, SIGCONT) < 0) {
			perror("SIGCONT");
			result = 1;
			break;
		}
	}
	if (result) {
		kill(child, SIGKILL);
	}
	if (waitpid(child, &status, 0) < 0) {
		perror("waitpid");
		result = 1;
	}

	printf("Parent after child write: first=%u\n", memory[0]);
	free(memory);
	return result;
}
