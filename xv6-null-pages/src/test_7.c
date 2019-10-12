#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

/*test mprotect and munprotect by using it on the page that main() is mapped to and attempting to write things to main.*/
int main(int argc, char *argv[])
{
    int ret = mprotect((void *)main, 1);
    printf(1, "XV6_TEST_OUTPUT: Return value of mprotect on Page where MAIN is mapped: %d\n", ret);

    *(char**)main = (char*)0x2000; //page fault trap 14 should occur here

    printf(1, "XV6_TEST_OUTPUT: this shouldn't print\n");
   
    exit();
}