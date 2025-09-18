# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>

int main () {
    int pipe1 [2]; // Parent to child
    int pipe2 [2]; // Child to parent
    pid_t pid ;

    // Create both pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // ----------------------
        // Child process
        // ----------------------
        close(pipe1[1]); // child doesn't write to pipe1
        close(pipe2[0]); // child doesn't read from pipe2

        char buf[256];
        int n;

        // Read message from parent
        n = read(pipe1[0], buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Child received: %s\n", buf);

            // Send response
            char reply[] = "Hi Parent, this is Child!";
            write(pipe2[1], reply, strlen(reply));
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);

    } else {
        // ----------------------
        // Parent process
        // ----------------------
        close(pipe1[0]); // parent doesn't read from pipe1
        close(pipe2[1]); // parent doesn't write to pipe2

        char msg[] = "Hello Child, this is Parent!";
        write(pipe1[1], msg, strlen(msg));

        char buf[256];
        int n = read(pipe2[0], buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Parent received: %s\n", buf);
        }

        close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL); // wait for child to finish

    }

    return 0;
}
