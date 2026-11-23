#include <stdio.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char **argv) {
	printf("childinfo PID=%ld\n", (long)getpid());

	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}

	for (char **p = environ; *p; p++) {
		printf("env: %s\n", *p);
	}

	return 0;
}
