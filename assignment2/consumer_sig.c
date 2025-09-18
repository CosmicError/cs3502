#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

long line_count = 0;
long byte_count = 0;

clock_t start_time;

void handle_sigint(int sig) {
    shutdown_flag = 1;   // graceful shutdown
}

void handle_sigusr1(int sig) {
    stats_flag = 1;      // request stats
}

int main(int argc, char *argv[]) {
    struct sigaction sa;

    // Setup SIGINT (Ctrl+C)
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Setup SIGUSR1 (stats)
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    start_time = clock();

    char buffer[1024];
    while (!shutdown_flag && fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        // Consumer can echo if verbose mode is wanted
        // write(STDOUT_FILENO, buffer, len);

        line_count++;
        byte_count += len;

        if (stats_flag) {
            double elapsed = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            fprintf(stderr, "[Consumer] Lines: %ld, Bytes: %ld, Time: %.2f sec, Throughput: %.2f lines/sec\n",
                    line_count, byte_count, elapsed, line_count / elapsed);
            stats_flag = 0;
        }
    }

    fprintf(stderr, "[Consumer] Shutdown. Total lines: %ld, Total bytes: %ld\n",
            line_count, byte_count);
    return 0;
}
