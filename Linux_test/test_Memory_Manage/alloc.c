// alloc.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("PID: %d\n", getpid());
    getchar(); // wait for user input
    void *mem = malloc(100 * 1024 * 1024); // allocate 100MB
    memset(mem, 0, 100 * 1024 * 1024);
    if (mem == NULL) {
        perror("malloc");
        return 1;
    }
    getchar(); // wait again
    return 0;
}

