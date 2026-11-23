#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FIFO\n", argv[0]);
		return 1;
	}

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		perror("signal");
		return 1;
	}

	int fd = open(argv[1], O_WRONLY);
	if (fd < 0) {
		perror("fdopen");
		close(fd);
		return 1;
	}
	FILE *fifo = fdopen(fd, "w");

	int failed = fputs("Message A\nMessage B\nMessage C\n", fifo) == EOF;
	if (fclose(fifo) == EOF) {
		failed = 1;
	}
	if (failed) {
		perror("write FIFO");
	}

	return failed;
}
