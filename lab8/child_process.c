#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_THREADS 100

typedef struct {
    int num_threads;
    int lower_bound;
    int upper_bound;
    int pipe_write_fd;
} ProgramParams;

typedef struct {
    pthread_t thread_id;
    int lower_bound;
    int upper_bound;
    int numbers_count;
} ThreadData;

int *numbers;
int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void ParseProgramParams(int argc, char **argv, ProgramParams *program_params) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <num_threads> <lower_bound> <upper_bound> <pipe_write_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    program_params->num_threads = atoi(argv[1]);
    program_params->lower_bound = atoi(argv[2]);
    program_params->upper_bound = atoi(argv[3]);
    program_params->pipe_write_fd= atoi(argv[4]);

    if (program_params->num_threads <= 0 || program_params->num_threads > MAX_THREADS || 
        program_params->lower_bound >= program_params->upper_bound) {
        fprintf(stderr, "Invalid input parameters\n");
        exit(EXIT_FAILURE);
    }
}

bool IsPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

void *ThreadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    //printf("Thread -> ID: %lu searching between %d and %d\n", data->thread_id, data->lower_bound, data->upper_bound);
    data->numbers_count = 0;

    for (int i = data->lower_bound; i <= data->upper_bound; i++) {
        if (IsPrime(i)) {
            pthread_mutex_lock(&mutex);
            numbers[counter++] = i;
            data->numbers_count++;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void CreateThreads(ThreadData *thread_data, int num_threads, int lower_bound, int upper_bound) {
    int bound_diff = upper_bound - lower_bound;
    for(int i = 0; i < num_threads; i++) {
        thread_data[i].lower_bound = lower_bound + (i * bound_diff / num_threads);
        if(i == num_threads - 1) {
            thread_data[i].upper_bound = upper_bound;
        } else {
        thread_data[i].upper_bound = lower_bound + ((i + 1) * bound_diff / num_threads) - 1;
        }

        if (pthread_create(&thread_data[i].thread_id, NULL, &ThreadFunction, (void *)&thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void WaitForThreads(ThreadData *thread_data, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(thread_data[i].thread_id, NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void PrintThreadResults(ThreadData *thread_data, int num_threads) {
    printf("Results from all threads:\n");

    for (int i = 0; i < num_threads; i++) {
        printf("Thread -> ID: %lu found %d prime numbers between %d and %d\n", 
                thread_data[i].thread_id, thread_data[i].numbers_count, 
                thread_data[i].lower_bound, thread_data[i].upper_bound);
    }

    qsort(numbers, counter, sizeof(int), compare);

    printf("Sorted prime numbers: %d\n", counter);
    for (int i = counter - 1; i >= 0; i--) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

void SendResults(int pipe_fd_write) {
    int bytes_written = write(pipe_fd_write, numbers, counter * sizeof(int));
    if (bytes_written == -1) {
        perror("Error writing to pipe");
        exit(EXIT_FAILURE);
    }
}

void FreeMem(ThreadData *thread_data, int num_threads) {
    free(thread_data);
    free(numbers);
}

int main(int argc, char** argv) {
    ProgramParams program_params;

    ParseProgramParams(argc, argv, &program_params);

    numbers = malloc((program_params.upper_bound - program_params.lower_bound) * sizeof(int));

    int num_threads = program_params.num_threads;
    ThreadData *thread_data = malloc(num_threads * sizeof(ThreadData)); 
    if (thread_data == NULL) {
        fprintf(stderr, "Error allocating memory for thread_data\n");
        exit(EXIT_FAILURE);
    }

    CreateThreads(thread_data, num_threads, program_params.lower_bound, program_params.upper_bound);
    WaitForThreads(thread_data, num_threads);
    SendResults(program_params.pipe_write_fd);
    //PrintThreadResults(thread_data, num_threads);

    FreeMem(thread_data, num_threads);
    return 0;
}
