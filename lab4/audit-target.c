#include <stdio.h>
#include <unistd.h>

int main(void) {
	printf("audit-target is ready. PID=%ld\n", (long)getpid());
	fflush(stdout);
	sleep(120);
	return 0;
}
