#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int valid_permissions(const char *s) {
	return strlen(s) == 4 &&
		(s[0] == 'r' || s[0] == '-') &&
		(s[1] == 'w' || s[1] == '-') &&
		(s[2] == 'x' || s[2] == '-') &&
		(s[3] == 'p' || s[3] == 's');
}

int main(int argc, char **argv) {
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s PID [permissions]\n", argv[0]);
		return 1;
	}

	char *endptr;
	errno = 0;
	long pid = strtol(argv[1], &endptr, 10);
	if (errno || endptr == argv[1] || *endptr || pid < 1 || pid > INT_MAX) {
		fprintf(stderr, "Invalid PID\n");
		return 1;
	}

	const char *filter = argc == 3 ? argv[2] : NULL;
	if (filter && !valid_permissions(filter)) {
		fprintf(stderr, "Invalid permissions. Examples: r-xp, rw-p\n");
		return 1;
	}

	char path[64];
	snprintf(path, sizeof path, "/proc/%ld", pid);
	int directory = open(path, O_RDONLY | O_DIRECTORY);
	if (directory < 0) {
		perror(path);
		return 1;
	}

	int fd = openat(directory, "maps", O_RDONLY);
	if (fd < 0) {
		perror("open maps");
		close(directory);
		return 1;
	}

	FILE *maps = fdopen(fd, "r");
	if (!maps) {
		perror("fdopen");
		close(fd);
		close(directory);
		return 1;
	}

	char *line = NULL;
	size_t capacity = 0;
	unsigned long long total = 0, shown = 0;
	int incomplete = 0;

	printf("%-17s %-17s %12s %-5s %s\n",
		"START", "END", "BYTES", "PERM", "FILE");

	while (getline(&line, &capacity, maps) >= 0) {
		unsigned long long start, end, offset, inode;
		char perm[5], device[32];
		int pos = 0;
	
		int fields = sscanf(line, "%llx-%llx %4s %llx %31s %llu %n",
				&start, &end, perm, &offset, device, &inode, &pos);
		if (fields != 6 || end < start) {
			fprintf(stderr, "Could not parse maps entry\n");
			incomplete = 1;
			continue;
		}

		unsigned long long bytes = end - start;
		total += bytes;
		
		if (filter && strcmp(filter, perm) != 0) {
			continue;
		}

		char *name = line + pos;
		name[strcspn(name, "\n")] = '\0';
		printf("%016llx %016llx %12llu %-5s %s\n",
			start, end, bytes, perm, *name ? name : "-");
		shown += bytes;
	}
		
	if (ferror(maps)) {
		perror("read maps");
		incomplete = 1;
	}

	struct stat info;
	if (fstatat(directory, "status", &info, 0) < 0) {
		perror("process status after reading maps");
		incomplete = 1;
	}

	printf("Total=%llu KiB, shown=%llu KiB%s\n",
		total / 1024, shown / 1024,
		incomplete ? " (possibly incomplete)" : "");

	free(line);
	fclose(maps);
	close(directory);
	return incomplete;
}
