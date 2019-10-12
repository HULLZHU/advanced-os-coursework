#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

void threadfunc(void *arg1, void *arg2) {
  *(int *)arg2 = (int)arg1;
  exit();
}

/*testing whether PIDs returned by clone() and join() system calls are correct*/
int main(int argc, char *argv[])
{
  void *stack = sbrk(4096);
  memset(stack, 0, 4096);

  int pid1 = clone(threadfunc, (void*)31415, (void*)0, stack);
  int pid2 = join(&stack);

  if(pid1 == pid2)
  {
    printf(1, "XV6_TEST_OUTPUT : Correct PIDs returned by clone() and join()\n");
  }
  else
  {
    printf(1, "XV6_TEST_OUTPUT : PIDs returned by clone() and join() do not match..!\n");
  }
  exit();
}
