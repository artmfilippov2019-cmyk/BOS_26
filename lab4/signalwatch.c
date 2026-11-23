#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile sig_atomic_t got_usr1 = 0;
static volatile sig_atomic_t got_term = 0;

static void handler(int sig) {
	if (sig == SIGUSR1) {
		got_usr1 = 1;
	}
	if (sig == SIGTERM) {
		got_term = 1;
	}
}

int main(void) {
	sigset_t blocked, previous, wait_mask;
	sigemptyset(&blocked);
	sigaddset(&blocked, SIGUSR1);
	sigaddset(&blocked, SIGTERM);

	if (sigprocmask(SIG_BLOCK, &blocked, &previous) < 0) {
		perror("sigprocmask");
		return 1;
	}

	struct sigaction action = {0};
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);

	if (sigaction(SIGUSR1, &action, NULL) < 0 ||
		 sigaction(SIGTERM, &action, NULL) < 0) {
		perror("sigaction");
		return 1;
	}

	wait_mask = previous;
	sigdelset(&wait_mask, SIGUSR1);
	sigdelset(&wait_mask, SIGTERM);

	printf("Ready. PID=%ld\n", (long)getpid());
	fflush(stdout);

	while (!got_term) {
		while (!got_usr1 && !got_term) {
			sigsuspend(&wait_mask);
		}

		if (got_usr1) {
			got_usr1 = 0;
			puts("Received SIGUSR1. Continuing.");
			fflush(stdout);
		}
	}

	puts("Received SIGTERM. Exiting.");
	return 0;
}
