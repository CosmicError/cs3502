# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <getopt.h>
# include <time.h>

int main (int argc , char * argv []) {
    int max_lines = -1; // -1 means unlimited
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "n:v")) != -1) {

        switch (opt) {
            case 'n':
                max_lines = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            default:
                fprintf(stderr, "Usage: %s [-n max_lines] [-v]\n", argv[0]);
                exit(1);
        }

    }

    char buffer[4096];
    int line_count = 0;
    int char_count = 0;

    while (fgets(buffer, sizeof(buffer), stdin)) {
        line_count++;
        char_count += strlen(buffer);

        if (verbose) {
            fputs(buffer, stdout);
        }

        if (max_lines > 0 && line_count >= max_lines) {
            break;
        }
    }

    fprintf(stderr, "Lines: %d\n", line_count);
    fprintf(stderr, "Characters: %d\n", char_count);

    return 0;
}
