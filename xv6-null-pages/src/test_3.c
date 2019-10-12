#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

/*testing bad arguments for mprotect*/
int main(int argc, char *argv[])
{
    int ret = mprotect((void*)4096, 0);
    printf(1, "XV6_TEST_OUTPUT: Return value of mprotect with len 0: %d\n", ret);

    ret = mprotect((void*)4096, -1);
    printf(1, "XV6_TEST_OUTPUT: Return value of mprotect with len -1: %d\n", ret);

    ret = mprotect((void*)5000, 1);
    printf(1, "XV6_TEST_OUTPUT: Return value of mprotect with unaligned Page Address: %d\n", ret);
    exit();
}