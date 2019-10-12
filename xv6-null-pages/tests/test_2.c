#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    volatile char *p = (char *)0;

    *p; //page fault trap 14 should occur here
    printf(1, "XV6_TEST_OUTPUT: this shouldn't print\n");
    exit();
}