#include <stdio.h>
#include <unistd.h>

int main(void) {
	printf("PID=%ld PPID=%ld UID=%ld EUID=%ld\n",
		(long)getpid(), (long)getppid(),
		(long)getuid(), (long)geteuid());
	fflush(stdout);
	sleep(50);
	return 0;
}
