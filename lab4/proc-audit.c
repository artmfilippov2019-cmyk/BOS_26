#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *read_field(int directory, const char *name, int delimiter) {
	int fd = openat(directory, name, O_RDONLY);
	if (fd < 0) {
		perror(name);
		exit(1);
	}

	FILE *file = fdopen(fd, "r");
	if (!file) {
		perror("fdopen");
		exit(1);
	}

	char *text = NULL;
	size_t size = 0;
	ssize_t n = getdelim(&text, &size, delimiter, file);
	
	if (n < 0 && ferror(file)) {
		perror(name);
		exit(1);
	}

	if (n < 0) {
		free(text);
		text = strdup("");
		if (!text) {
			perror("strdup");
			exit(1);
		}
	}
	else if (n > 0 && text[n - 1] == delimiter) {
		text[n - 1] = '\0';
	}

	fclose(file);
	return text;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s PID\n", argv[0]);
		return 1;
	}

	char *end;
	errno = 0;
	long pid = strtol(argv[1], &end, 10);
	if (errno || end == argv[1] || *end || pid < 1 || pid > INT_MAX) {
		fprintf(stderr, "Invalid PID\n");
		return 1;
	}

	char path[64];
	snprintf(path, sizeof path, "/proc/%ld", pid);
	int directory = open(path, O_RDONLY | O_DIRECTORY);
	if (directory < 0) {
		perror(path);
		return 1;
	}

	char *arg0 = read_field(directory, "cmdline", '\0');
	char *comm = read_field(directory, "comm", '\n');
	
	char exe[PATH_MAX + 1];
	ssize_t n = readlinkat(directory, "exe", exe, sizeof exe);
	if (n < 0) {
		perror("readlink exe");
		return 1;
	}
	if ((size_t)n == sizeof exe) {
		fprintf(stderr, "Executable path is too long\n");
		return 1;
	}
	exe[n] = '\0';

	const char *slash = strrchr(exe, '/');
	const char *file_name = slash ? slash + 1 : exe;

	printf("PID:             %ld\n", pid);
	printf("cmdline argv[0]: %s\n", arg0);
	printf("comm:            %s\n", comm);
	printf("exe:             %s\n", exe);
	printf("argv[0] equals executable basename: %s\n",
			strcmp(arg0, file_name) == 0 ? "yes" : "no");

	free(arg0);
	free(comm);
	close(directory);
	return 0;
} 
