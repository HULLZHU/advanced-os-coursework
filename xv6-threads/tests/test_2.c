#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

/*testing bad arguments for clone function call*/
int main(int argc, char *argv[])
{
  int ret = clone((void*)0xFFFF, (void*)31415, (void*)0, (void*)0);
  printf(1, "XV6_TEST_OUTPUT : Return value of clone system call: %d\n", ret);

  exit();
}
