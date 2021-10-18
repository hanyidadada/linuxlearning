#include <stdio.h>
#include <stdlib.h>

extern char ** environ;

int main(void) {
    int i;
    char *env;
    for(i = 0; environ[i]; i++) {
       printf("%s\n", environ[i]);
    }
    env = getenv("HOME");
    printf("\n%s\n",env); 
    return 0;
}
