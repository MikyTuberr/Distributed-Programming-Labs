#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h> 

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void execute_command_parallel(const char *command) {
    pid_t pid = fork(); 

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        execl("/usr/bin/bash", "bash", "-c", command, (char*)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } 
}

void wait_for_processes(int num_commands) {
    while (num_commands > 0) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG); 
        if (pid > 0) {
            if (WIFEXITED(status)) {
                printf(ANSI_COLOR_GREEN"CHILD PROCESS TERMINATED WITH EXIT STATUS: %d"ANSI_COLOR_RESET"\n" , toupper(WEXITSTATUS(status)));
            } else {
                printf(ANSI_COLOR_RED"CHILD PROCESS TERMINATED ABNORMALLY\n"ANSI_COLOR_RESET);
            }
            num_commands--; 
        } 
        else if (pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    printf(ANSI_COLOR_BLUE"ALL CHILD PROCESSES HAVE FINISHED.\n"ANSI_COLOR_RESET);

}

int main(int argc, char** argv, char** envp) {
    const char *commands[] = {
        "wget -O large_file https://ping.virtua.cloud/100MB.bin",    
        "ping -c 10 facebook.com",                                       
        "sleep 5 && echo ENDED WAITING",                                                                                                                                                                  
        "openssl speed -elapsed -evp sha256"     
    };

    int num_commands = sizeof(commands) / sizeof(commands[0]); 
    for (int i = 0; i < num_commands; ++i) {
        execute_command_parallel(commands[i]);
    }

    wait_for_processes(num_commands);

    return 0;
}