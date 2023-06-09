#include "headers.h"
#include "Semaphore.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    int x = getClk();
    int follow = x;
    int pid = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    union Semun semun; /*Semaphpre union*/
    key_t sID;
    int sem;

    printf("Process %d: Current Time is %d\n", pid, x);
    /*Create semaphore for each recieved process*/
    sem = semget(pid, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    semun.val = 0;
    if (semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    printf("Process %d: Semaphore is %d\n", pid, sem);

    // TODO The process needs to get the remaining time from somewhere
    // remainingtime = ??;

    // semaphore 
    while (remainingtime > 0)
    {
        x = getClk();
        /*Enter loop each one second only*/
        if (follow != x)
        {
            follow = x;
           if(remainingtime == 1){printf("<<<<<<<<<<<<<<<<<<<< Process %d BEFORE SEM >>>>>>>>>>>>>>>>>\n",pid);};
            down(sem);
            --remainingtime;
            printf("------------------------------------------------------------\n");
            printf("Process %d: Semaphore %d Remaining is %d\n", pid, sem,remainingtime);
            printf("Process %d: Clock is: %d\n", pid, getClk());
            printf("------------------------------------------------------------\n");
        }
    }
    printf("<<<<<<<<<<<<<<<<<<<< Process %d Terminates >>>>>>>>>>>>>>>>>\n",pid);
    destroyClk(false);

    return 0;
}
