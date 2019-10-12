#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

/*testing bad arguments for join function call*/
int main(int argc, char *argv[])
{
  int ret = join((void**)0xFFFF);
  printf(1, "XV6_TEST_OUTPUT : Return value of join system call: %d\n", ret);
  exit();
}
