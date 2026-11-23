#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int is_pid(const char *name) {
	if (!*name) {
		return 0;
	}

	for (; *name; name++) {
		if (!isdigit((unsigned char)*name)) {
			return 0;
		}
	}
	return 1;
}

static void show_process(const char *directory, uid_t my_uid) {
	char path[512];
	snprintf(path, sizeof path, "/proc/%s/status", directory);
	
	FILE *file = fopen(path, "r");
	if (!file) {
		return;
	}

	char line[512], name[512] = "";
	int pid = -1, ppid = -1;
	unsigned long uid = ULONG_MAX;
	char state = '?';

	while (fgets(line, sizeof line, file)) {
		if (strncmp(line, "Name:\t", 6) == 0) {
			line[strcspn(line, "\n")] = '\0';
			strcpy(name, line + 6);
		}
		else if (strncmp(line, "Pid:", 4) == 0) {
			sscanf(line + 4, "%d", &pid);
		}
		else if (strncmp(line, "PPid:", 5) == 0) {
			sscanf(line + 5, "%d", &ppid);
		}
		else if (strncmp(line, "State:", 6) == 0) {
			sscanf(line + 6, " %c", &state);
		}
		else if (strncmp(line, "Uid:", 4) == 0) {
			sscanf(line + 4, "%lu", &uid);
		}
	}

	if (!ferror(file) && uid == (unsigned long)my_uid && pid > 0 && ppid >= 0 && state != '?') {
		printf("%7d %7d %5c  %s \n", pid, ppid, state, name);
	}

	fclose(file);
}

int main(void) {
	DIR *proc = opendir("/proc");
	if (!proc) {
		perror("opendir /proc");
		return 1;
	}

	uid_t uid = getuid();
	printf("%7s %7s %5s  %s\n", "PID", "PPID", "STATE", "NAME");

	for (;;) {
		errno = 0;
		struct dirent *entry = readdir(proc);
		if (!entry) {
			if (errno) {
				perror("readdir");
				closedir(proc);
				return 1;
			}
			break;
		}
		if (is_pid(entry->d_name)) {
			show_process(entry->d_name, uid);
		}
	}

	closedir(proc);
	return 0;
}
