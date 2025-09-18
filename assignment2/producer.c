# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <getopt.h>

int main (int argc , char * argv []) {
    FILE * input = stdin;
    int buffer_size = 4096;
    char opt;

    while ((opt = getopt(argc, argv, "f:b:")) != -1) {

        switch (opt) {
            case 'f':
                input = fopen(optarg, "r");
                if (!input) {
                    perror("fopen"); // print an error if we can't open the file
                    exit(1); // exit the program since we error'd
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

    char *buffer = malloc(buffer_size);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }

    size_t bytes;
    while ((bytes = fread(buffer, 1, buffer_size, input)) > 0) {
        fwrite(buffer, 1, bytes, stdout);
    }

    free(buffer);
    if (input != stdin) {
        fclose(input);
    }

    return 0;
}
