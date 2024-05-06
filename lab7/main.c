#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_THREADS 100

typedef struct {
    int num_threads;
    int lower_bound;
    int upper_bound;
} ProgramParams;

typedef struct {
    pthread_t thread_id;
    int lower_bound;
    int upper_bound;
    int numbers_count;
    int *numbers;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void ParseProgramParams(int argc, char **argv, ProgramParams *program_params) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <num_threads> <lower_bound> <upper_bound>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    program_params->num_threads = atoi(argv[1]);
    program_params->lower_bound = atoi(argv[2]);
    program_params->upper_bound = atoi(argv[3]);

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

void WriteToFile(int number, pthread_t thread_id) {
    FILE *file = fopen("results.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file results.txt\n");
        return;
    }
    fprintf(file, "Thread ID: %lu, Number: %d\n", thread_id, number);
    fclose(file);
}

void *ThreadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("Thread -> ID: %lu searching between %d and %d\n", data->thread_id, data->lower_bound, data->upper_bound);
    data->numbers_count = 0;

    for (int i = data->lower_bound; i <= data->upper_bound; i++) {
        if (IsPrime(i)) {
            pthread_mutex_lock(&mutex);
            data->numbers[data->numbers_count++] = i;
            WriteToFile(i, pthread_self());
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

void CreateThreads(ThreadData *thread_data, int num_threads, int lower_bound, int upper_bound) {
    for(int i = 0; i < num_threads; i++) {
        int bound_diff = upper_bound - lower_bound;
        thread_data[i].lower_bound = lower_bound + (i * bound_diff / num_threads);
        thread_data[i].upper_bound = lower_bound + ((i + 1) * bound_diff / num_threads) - 1;

        thread_data[i].numbers = malloc(bound_diff * sizeof(int));
        if(thread_data[i].numbers == NULL) {
            fprintf(stderr, "Error allocating memory for numbers in thread %d\n", i);
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&thread_data[i].thread_id, NULL, &ThreadFunction, (void *)&thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void WiatForThreads(ThreadData *thread_data, int num_threads) {
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

    int total_primes = 0;
    for (int i = 0; i < num_threads; i++) {
        total_primes += thread_data[i].numbers_count;
    }

    int *all_primes = malloc(total_primes * sizeof(int));
    if (all_primes == NULL) {
        fprintf(stderr, "Error allocating memory for all_primes\n");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    for (int i = 0; i < num_threads; i++) {
        printf("Thread -> ID: %lu found %d prime numbers between %d and %d\n", 
                thread_data[i].thread_id, thread_data[i].numbers_count, 
                thread_data[i].lower_bound, thread_data[i].upper_bound);
        
        for (int j = 0; j < thread_data[i].numbers_count; j++) {
            all_primes[index++] = thread_data[i].numbers[j];
        }
    }

    qsort(all_primes, total_primes, sizeof(int), compare);

    printf("Sorted prime numbers:\n");
    for (int i = total_primes - 1; i >= 0; i--) {
        printf("%d ", all_primes[i]);
    }
    printf("\n");

    free(all_primes);
}

void FreeMem(ThreadData *thread_data, int num_threads) {
    for(int i = 0; i < num_threads; i++) {
        free(thread_data[i].numbers);
    }
    free(thread_data);
}

int main(int argc, char** argv) {
    ProgramParams program_params;

    ParseProgramParams(argc, argv, &program_params);

    int num_threads = program_params.num_threads;
    ThreadData *thread_data = malloc(num_threads * sizeof(ThreadData)); 
    if (thread_data == NULL) {
        fprintf(stderr, "Error allocating memory for thread_data\n");
        exit(EXIT_FAILURE);
    }

    CreateThreads(thread_data, num_threads, program_params.lower_bound, program_params.upper_bound);
    WiatForThreads(thread_data, num_threads);

    PrintThreadResults(thread_data, num_threads);

    FreeMem(thread_data, num_threads);
    return 0;
}