#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

/*testing good arguments for munprotect*/
int main(int argc, char *argv[])
{
    int ret = munprotect((void*)4096, 1);
    printf(1, "XV6_TEST_OUTPUT: Return value of munprotect with len 1: %d\n", ret);

    ret = munprotect((void*)4096, 2);
    printf(1, "XV6_TEST_OUTPUT: Return value of munprotect with len 2: %d\n", ret);
    exit();
}