#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

int main(void)
{
    int i = 0;
    struct itimerval newval, oldval;
    memset(&newval, 0, sizeof(struct itimerval));
    memset(&oldval, 0, sizeof(struct itimerval));
    newval.it_value.tv_sec = 1;

    setitimer(ITIMER_REAL, &newval, &oldval);

    for (i = 0; 1 ; i++) {
        printf("%d\n", i);
    }
    
    return 0;
}
