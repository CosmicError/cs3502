#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
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
    FILE *input = stdin;
    int buffer_size = 4096;
    int opt;

    // Parse arguments (-f filename, -b buffer_size)
    while ((opt = getopt(argc, argv, "f:b:")) != -1) {
        switch (opt) {
            case 'f':
                input = fopen(optarg, "r");
                if (!input) {
                    perror("fopen");
                    exit(1);
                }
                break;
            case 'b':
                buffer_size = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-f filename] [-b buffer_size]\n", argv[0]);
                exit(1);
        }
    }

    // Setup signal handlers
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    start_time = clock();

    char *buffer = malloc(buffer_size);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }

    // Producer loop
    while (!shutdown_flag && fgets(buffer, buffer_size, input)) {
        size_t len = strlen(buffer);
        write(STDOUT_FILENO, buffer, len);

        line_count++;
        byte_count += len;

        if (stats_flag) {
            double elapsed = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            fprintf(stderr, "[Producer] So far: %ld lines, %ld bytes, Time: %.2f sec, Throughput: %.2f MB/s\n",
                    line_count, byte_count, elapsed,
                    (byte_count / 1024.0 / 1024.0) / elapsed);
            stats_flag = 0;
        }
    }

    double elapsed = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    fprintf(stderr, "[Producer] Shutdown. Total lines: %ld, Total bytes: %ld, Time: %.2f sec, Throughput: %.2f MB/s\n",
            line_count, byte_count, elapsed,
            (byte_count / 1024.0 / 1024.0) / elapsed);

    // Cleanup
    free(buffer);
    if (input != stdin) fclose(input);

    return 0;
}

