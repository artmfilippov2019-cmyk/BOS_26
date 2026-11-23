#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [path | --audit]\n", argv[0]);
		return 1;
	}
	
	char *env[] = {"LAB=4", "MODE=demo", NULL};
	char *normal_args[] = {"childinfo", "one", "two", NULL};
	char *audit_args[] = {"lab-demo", NULL};

	const char *path = "./childinfo";
	char **args = normal_args;

	if (argc == 2 && strcmp(argv[1], "--audit") == 0) {
		path = "./audit-target";
		args = audit_args;
	}
	else if (argc == 2) {
		path = argv[1];
	}

	printf("launcher PID=%ld\n", (long)getpid());
	fflush(stdout);

	execve(path, args, env);
	perror("execve");
	return 1;
}
