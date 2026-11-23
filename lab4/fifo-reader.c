#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FIFO\n", argv[0]);
		return 1;
	}

	FILE *fifo = fopen(argv[1], "r");
	if (!fifo) {
		perror("open FIFO");
		return 1;
	}

	char line[256];
	while (fgets(line, sizeof line, fifo)) {
		fputs(line, stdout);
	}

	if (ferror(fifo)) {
		perror("read FIFO");
		fclose(fifo);
		return 1;
	}

	fclose(fifo);
	puts("Reader received EOF");
	return 0;
}
