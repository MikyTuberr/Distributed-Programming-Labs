#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#define MAX_PROCESSES 10
#define EXE_PATH "sub_program.exe"
#define COMMANDS_NUM 4

void create_new_process(DWORD* processes, int* process_count) {
    PROCESS_INFORMATION procInfo;
    STARTUPINFO startupInfo;

    GetStartupInfo(&startupInfo);

    if (CreateProcess(NULL, EXE_PATH, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, 0, 0, &startupInfo, &procInfo)) {
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
        processes[(*process_count)++] = procInfo.dwProcessId;
        printf("Process created with ID: %lu\n", procInfo.dwProcessId);
    } else {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }
}

void stop_process(DWORD* processes, int* process_count) {
    DWORD pid;
    printf("Enter the process ID to stop: ");
    scanf("%lu", &pid);
    getchar();

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    
    if (hProcess != NULL) {
        if (TerminateProcess(hProcess, 0)) {
            printf("Process %lu has been stopped.\n", pid);
            CloseHandle(hProcess);
            for (int i = 0; i < *process_count; ++i) {
                if (processes[i] == pid) {
                    processes[i] = processes[(*process_count)--];
                    break;
                }
            }
        } else {
            printf("TerminateProcess failed (%d).\n", GetLastError());
        }
        CloseHandle(hProcess);
    } else {
        printf("No process found with the specified ID.\n");
    }
}

void list_processes(DWORD* processes, int* process_count) {
    printf("List of running processes:\n");
    for (int i = 0; i < *process_count; ++i) {
        printf("ID: %d\n", processes[i]);
    }
}

void close_program(DWORD* processes, int* process_count) {
    for (int i = 0; i < *process_count; ++i) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processes[i]);
        if (hProcess != NULL) {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
        }
    }
    exit(EXIT_SUCCESS);
}

void print_menu() {
    printf("\nMenu:\n");
    printf("1. Create new process\n");
    printf("2. Stop process\n");
    printf("3. List processes\n");
    printf("4. Close program\n");
    printf("Enter your choice: ");
}

int main(int argc, char** argv) {
    DWORD processes[MAX_PROCESSES];
    int process_count = 0;
    int choice;
    void (*func_arr[])(DWORD*, int*) = {create_new_process, stop_process, 
                                    list_processes, close_program};

    while (1) {
        print_menu();
        scanf("%d", &choice);
        getchar(); 

        if(choice > 0 && choice <= COMMANDS_NUM) {
            func_arr[choice - 1](processes, &process_count);
        }
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}

