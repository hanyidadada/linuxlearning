#include <stdio.h>
#include <unistd.h>

int main(void) {
    int i, ret;
    ret = alarm(1);
    if (ret < 0) {
        perror("alarm error");
        exit(-1);
    }
    for (i = 1; i; i++) {
        printf("%d\n",i);
    }
    
    return 0;
    
}

