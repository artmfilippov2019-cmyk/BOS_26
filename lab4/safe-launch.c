#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int close_extra_fds(void) {
	DIR *dir = opendir("/proc/self/fd");
	if (!dir) {
		perror("opendir /proc/self/fd");
		return -1;
	}

	int own_fd = dirfd(dir);
	for (;;) {
		errno = 0;
		struct dirent *entry = readdir(dir);
		if (!entry) {
			if (errno) {
	 			perror("readdir");
				closedir(dir);
				return -1;
			}
			break;
		}

		char *end;
		long fd = strtol(entry->d_name, &end, 10);
		if (end == entry->d_name || *end || fd < 3 || fd > INT_MAX || fd == own_fd) {
			continue;
		}

		if (close((int)fd) < 0) {
			perror("close");
			closedir(dir);
			return -1;
		}
	}

	return closedir(dir);
}

int main(int argc, char **argv)  {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s childinfo TEXT\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "childinfo") != 0) {
		fprintf(stderr, "Command denied: %s\n", argv[1]);
		return 1;
	}

	if (argc != 3 || argv[2][0] == '\0' || strlen(argv[2]) > 100) {
		fprintf(stderr, "Expected one argument of 1 to 100 bytes\n");
		return 1;
	}

	char *args[] = {"childinfo", argv[2], NULL};
	char *env[] = {"LANG=C", "LAB=4", NULL};
	
	if (close_extra_fds() < 0) {
		return 1;
	}

	execve(CHILDINFO_PATH, args, env);
	perror("execve");
	return 1;
}
