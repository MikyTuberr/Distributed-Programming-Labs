#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>

#define MAX_THREADS 10
#define COMMANDS_NUM 5
#define a_ASCII 'a'


void print_priority_info() {
    printf("1. THREAD_PRIORITY_ABOVE_NORMAL: One point above the priority of the process\n");
    printf("2. THREAD_PRIORITY_BELOW_NORMAL: One point below the priority of the process\n");
    printf("3. THREAD_PRIORITY_HIGHEST: Two points above the priority of the process\n");
    printf("4. THREAD_PRIORITY_IDLE: Depending on the priority of the process - real-time level of value 16; others - level of value 1\n");
    printf("5. THREAD_PRIORITY_LOWEST: Two points below the priority of the process\n");
    printf("6. THREAD_PRIORITY_NORMAL: Priority of the process\n");
    printf("7. THREAD_PRIORITY_TIME_CRITICAL: Depending on the priority of the process - real-time level of value 31; others - level of value 15\n");
}

DWORD WINAPI do_some_work(LPVOID lpParam) {
    DWORD tid = GetCurrentThreadId();
    int sum = 0;
    for (int i = 1; i <= tid % 1000; ++i) {
        sum += i * i;
    }
    printf("Thread %lu finished work. Sum of squares: %d\n", tid, sum);
    Sleep(60000); 
    return 0;
}

void create_new_thread(HANDLE* threads, int* thread_count) {
    DWORD dwThreadId, dwThrdParam = 1;
    HANDLE hThread;
    int priority;

    print_priority_info();
    printf("Enter the priority for the new thread (1-7): ");
    scanf("%d", &priority);
    getchar();

    hThread = CreateThread(NULL, 0, do_some_work, &dwThrdParam, 0, &dwThreadId);
    if (hThread) {
        SetThreadPriority(hThread, priority);
        printf("Thread created with ID: %lu\n", dwThreadId);
        threads[(*thread_count)++] = hThread;
    } else {
        printf("Failed to create thread. Error: %d\n", GetLastError());
    }
}

void remove_thread(HANDLE* threads, int* thread_count) {
    unsigned long tid;
    printf("Enter the thread ID to remove: ");
    scanf("%lu", &tid);
    getchar();

    for (int i = 0; i < *thread_count; ++i) {
        if (GetThreadId(threads[i]) == tid) {
            TerminateThread(threads[i], 0);
            CloseHandle(threads[i]);
            threads[i] = threads[--(*thread_count)];
            printf("Thread %lu has been removed.\n", tid);
            return;
        }
    }
    printf("Thread with the specified ID was not found.\n");
}

void change_thread_priority(HANDLE* threads, int* thread_count) {
    unsigned long tid;
    int new_priority;

    printf("Enter the thread ID to change its priority: ");
    scanf("%lu", &tid);
    getchar();

    print_priority_info();
    printf("Enter the new priority for the thread (1-7): ");
    scanf("%d", &new_priority);
    getchar();

    for (int i = 0; i < *thread_count; ++i) {
        if (GetThreadId(threads[i]) == tid) {
            SetThreadPriority(threads[i], new_priority);
            printf("Thread %lu priority has been changed to %d.\n", tid, new_priority);
            return;
        }
    }
    printf("Thread with the specified ID was not found.\n");
}

void list_threads(HANDLE* threads, int* thread_count) {
    printf("List of running threads:\n");
    for (int i = 0; i < *thread_count; ++i) {
        printf("ID: %lu, Priority: %d\n", GetThreadId(threads[i]), GetThreadPriority(threads[i]));
    }
}

void close_process(HANDLE* threads, int* thread_count) {
    for (int i = 0; i < *thread_count; ++i) {
        TerminateThread(threads[i], 0);
        CloseHandle(threads[i]);
    }
    printf("Process has been closed.\n");
    exit(0);
}

void print_menu() {
    printf("\nMenu:\n");
    printf("a. Create new thread\n");
    printf("b. Remove thread\n");
    printf("c. Change thread priority\n");
    printf("d. List threads\n");
    printf("e. Close process\n");
    printf("Enter your choice: ");
}

int main() {
    HANDLE threads[MAX_THREADS];
    int thread_count = 0;
    char choice;
    void (*func_arr[])(HANDLE*, int*) = {create_new_thread, remove_thread, 
                                    change_thread_priority, list_threads, close_process};

    while (1) {
        print_menu();
        scanf("%c", &choice);
        getchar(); 

        if(choice >= a_ASCII && choice < a_ASCII + COMMANDS_NUM) {
            func_arr[choice - a_ASCII](threads, &thread_count);
        }
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
