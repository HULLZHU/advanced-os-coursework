#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

/*testing bad arguments for munprotect*/
int main(int argc, char *argv[])
{
    int ret = munprotect((void*)4096, 0);
    printf(1, "XV6_TEST_OUTPUT: Return value of munprotect with len 0: %d\n", ret);

    ret = munprotect((void*)4096, -1);
    printf(1, "XV6_TEST_OUTPUT: Return value of munprotect with len -1: %d\n", ret);

    ret = munprotect((void*)5000, 1);
    printf(1, "XV6_TEST_OUTPUT: Return value of munprotect with unaligned Page Address: %d\n", ret);
    exit();
}