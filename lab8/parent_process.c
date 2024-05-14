#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_NAME "potok_WEJSCIOWY"

typedef struct {
    int num_processes;
    int lower_bound;
    int upper_bound;
} Task;

int compare_integers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void CalcBounds(int* lower_bound, int* upper_bound, Task task, int bound_diff, int i) {
    *lower_bound = task.lower_bound + (i * bound_diff / task.num_processes);
    if(i == task.num_processes - 1) {
        *upper_bound = task.upper_bound;
    } else {
        *upper_bound  = task.lower_bound + ((i + 1) * bound_diff / task.num_processes) - 1;
    }
} 

void ExecuteProgram(int lower_bound, int upper_bound, int num_processes, int* pipe_fd) {
    char lower_bound_str[20];
    char upper_bound_str[20];
    char num_processes_str[10];
    char write_fd_str[10];

    snprintf(lower_bound_str, sizeof(lower_bound_str), "%d", lower_bound);
    snprintf(upper_bound_str, sizeof(upper_bound_str), "%d", upper_bound);
    snprintf(num_processes_str, sizeof(num_processes_str), "%d", num_processes); 
    snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd[1]);

    execlp("./child_process", "child_process", num_processes_str, lower_bound_str, upper_bound_str, write_fd_str, NULL);
    perror("Error during executing program");
    exit(EXIT_FAILURE);
}

void ReceiveNumbers(int pipe_fd[], int** all_numbers, int* total_elements, int bound_diff) {
    while(true) {
        int* numbers = calloc(bound_diff, sizeof(int));
        if (numbers == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        
        int num_read = read(pipe_fd[0], numbers, bound_diff * sizeof(int));
        if (num_read == -1) {
            perror("Error reading from pipe");
            exit(EXIT_FAILURE);
        } else if (num_read == 0) {
            continue; 
        }

        *total_elements = *total_elements + (num_read / sizeof(int));
        *all_numbers = realloc(*all_numbers, *total_elements * sizeof(int));
        if (*all_numbers == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        memcpy(*all_numbers, numbers, num_read);
        free(numbers); 
        break;
    }
}


void DisplaySortedNumbers(Task task, int* all_numbers, int total_elements) {
    printf("\n\nSorted numbers in range <%d,%d> with %d processes: ", task.lower_bound, task.upper_bound, task.num_processes);
    for (int i = 0; i < total_elements; i++) {
        printf("%d ", all_numbers[i]);
    }
    printf("\n\n");
}

void ReceiveTaskAndProcess(int fifo_fd, int pipe_fd[]) {
    while (true) {
        char buffer[256];
        int num_read = read(fifo_fd, buffer, sizeof(buffer));
        if (num_read == -1) {
            perror("Error during reading from pipe");
            exit(EXIT_FAILURE);
        } else if (num_read == 0) {
            continue;
        }

        Task task;
        sscanf(buffer, "%d %d %d", &task.num_processes, &task.lower_bound, &task.upper_bound);

        if (task.lower_bound == -1 && task.upper_bound == -1 && task.num_processes == -1) {
            break;
        }

        int bound_diff = task.upper_bound - task.lower_bound;

        for (int i = 0; i < task.num_processes; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error during creating child process");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                int lower_bound = -1, upper_bound = -1;
                CalcBounds(&lower_bound, &upper_bound, task, bound_diff, i);
                ExecuteProgram(lower_bound, upper_bound, 3, pipe_fd);
            }
        }

        for (int i = 0; i < task.num_processes; i++) {
            wait(NULL);
        }

        int* all_numbers = NULL;
        int total_elements = 0;

        ReceiveNumbers(pipe_fd, &all_numbers, &total_elements, bound_diff);
        qsort(all_numbers, total_elements, sizeof(int), compare_integers);
        DisplaySortedNumbers(task, all_numbers, total_elements);

        free(all_numbers);
    }
}

int main() {
    mkfifo(FIFO_NAME, 0666);

    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Error during reading from pipe");
        exit(EXIT_FAILURE);
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Error during creation of anonymous pipe");
        return EXIT_FAILURE;
    }

    ReceiveTaskAndProcess(fifo_fd, pipe_fd);

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    close(fifo_fd);
    unlink(FIFO_NAME);
    return 0;
}
