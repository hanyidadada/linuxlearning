#include <stdio.h>
#include <unistd.h>
int main(void) {
    printf("fd 0: %s\n", ttyname(0));
    printf("fd 1: %s\n", ttyname(1));
    printf("fd 2: %s\n", ttyname(2));
    printf("fd 3: %s\n", ttyname(3));
    printf("fd 4: %s\n", ttyname(4));
    return 0;
}