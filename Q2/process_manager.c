// process_manager.c
// Purpose: Create child processes, monitor them, prevent zombies,
// and terminate unresponsive children using signals.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CHILDREN 3
#define TIMEOUT_SECONDS 5

pid_t child_pids[NUM_CHILDREN];

// Handler to reap zombie processes asynchronously (event-driven, not polling)
void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    // WNOHANG: don't block if no child has exited yet
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("[Parent] Child %d exited normally with status %d\n",
                   pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Child %d was killed by signal %d\n",
                   pid, WTERMSIG(status));
        }
    }
}

int main() {
    // Register SIGCHLD handler so terminated children are reaped immediately,
    // preventing them from becoming zombies.
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    printf("[Parent] PID %d starting %d child processes...\n", getpid(), NUM_CHILDREN);

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            // ---- Child process ----
            printf("[Child %d] PID %d started\n", i, getpid());

            if (i == 1) {
                // Simulate an unresponsive/hung child (infinite loop)
                printf("[Child %d] Simulating unresponsive process (infinite loop)\n", i);
                while (1) {
                    sleep(1); // hangs "forever"
                }
            } else {
                // Simulate a normal short-lived task
                sleep(2);
                printf("[Child %d] Finished work, exiting normally\n", i);
                exit(0);
            }
        }
        else {
            // ---- Parent process ----
            child_pids[i] = pid;
        }
    }

    // Parent monitors children for TIMEOUT_SECONDS
    printf("[Parent] Monitoring children for %d seconds...\n", TIMEOUT_SECONDS);
    sleep(TIMEOUT_SECONDS);

    // Check which children are still alive (unresponsive) and terminate them
    for (int i = 0; i < NUM_CHILDREN; i++) {
        // kill(pid, 0) checks if process still exists, without actually sending a signal
        if (kill(child_pids[i], 0) == 0) {
            printf("[Parent] Child %d (PID %d) is unresponsive. Sending SIGTERM...\n",
                   i, child_pids[i]);
            kill(child_pids[i], SIGTERM);
            sleep(1);

            // If it's still alive after SIGTERM, force kill with SIGKILL
            if (kill(child_pids[i], 0) == 0) {
                printf("[Parent] Child %d (PID %d) ignored SIGTERM. Sending SIGKILL...\n",
                       i, child_pids[i]);
                kill(child_pids[i], SIGKILL);
            }
        }
    }

    // Give SIGCHLD handler a moment to reap the just-killed children
    sleep(1);

    printf("[Parent] All children handled. Exiting.\n");
    return 0;
}
