#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

/*test that forking the process keeps the same protection bits.*/
int main(int argc, char *argv[])
{   
    char *p = (char *)0x1000;
    int ret = mprotect((void *)p, 1);
    printf(1, "XV6_TEST_OUTPUT: Return value of mprotect on memory 0x1000: %d\n", ret);

    if(fork() == 0)
    {
        printf(1, "XV6_TEST_OUTPUT: Inside child process\n");
        *p = 'w'; //page fault trap 14 should occur here

        printf(1, "XV6_TEST_OUTPUT: This should not be printed\n");
        exit();
    }
    else
    {
        (void)wait();
    }
    
    exit();
}