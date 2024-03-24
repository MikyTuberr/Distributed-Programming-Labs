#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h> 

void execute_command_parallel(const char *command) {
    pid_t pid = fork(); 

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } 
}

void wait_for_processes(int num_commands) {
    for (int i = 0; i < num_commands; ++i) {
        int status;
        if (wait(&status) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("CHILD PROCESS TERMINATED WITH EXIT STATUS: %d\n", toupper(WEXITSTATUS(status)));
    }

    printf("ALL CHILD PROCESSES HAVE FINISHED.\n");
}

int main(int argc, char** argv, char** envp) {
    const char *commands[] = {
        "wget -O large_file https://ash-speed.hetzner.com/100MB.bin", 
        "ping -c 10 facebook.com", 
        "sleep 5", 
        "ls -l",    
        "date",     
        "df -h",    
        "whoami"    
    };

    int num_commands = sizeof(commands) / sizeof(commands[0]); 
    for (int i = 0; i < num_commands; ++i) {
        execute_command_parallel(commands[i]);
    }

    wait_for_processes(num_commands);

    return 0;
}