#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int counter = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void check(int error, const char *operation) {
	if (error != 0) {
		fprintf(stderr, "%s: %s\n", operation, strerror(error));
		exit(1);
	}
}

static void *worker(void *arg) {
	int number = *(int *)arg;

	check(pthread_mutex_lock(&mutex), "pthread_mutex_lock");
	counter++;
	printf("Worker=%d PID=%ld TID=%ld counter=%d address=%p\n",
		number, (long)getpid(), (long)gettid(), counter, (void *)&counter);
	fflush(stdout);
	check(pthread_mutex_unlock(&mutex), "pthread_mutex_unlock");

	sleep(120);
	return NULL;
}

int main(void) {
	pthread_t threads[3];
	int numbers[] = {1, 2, 3};

	printf("Main PID=%ld TID=%ld\n", (long)getpid(), (long)gettid());

	for (int i = 0; i < 3; i++) {
		check(pthread_create(&threads[i], NULL, worker, &numbers[i]), "pthread_create");
	}

	for (int i = 0; i < 3; i++) {
		check(pthread_join(threads[i], NULL), "pthread_join");
	}

	check(pthread_mutex_destroy(&mutex), "pthread_mutex_destroy");
	printf("Final counter=%d\n", counter);
	return 0;
} 
