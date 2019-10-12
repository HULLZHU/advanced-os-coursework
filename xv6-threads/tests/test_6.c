#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

lock_t mylock;

void threadfunc(void *arg1, void *arg2) 
{
  lock_acquire(&mylock);
  *(int *)arg2 = (int)arg1;
  lock_release(&mylock);
  
  exit();
}

/*testing clone() and join() system call with a simple thread function.*/
int main(int argc, char *argv[])
{
  int i = 0;
  printf(1, "XV6_TEST_OUTPUT : i before threadfunc = %x\n", i);

  void *stack = sbrk(4096);
  memset(stack, 0, 4096);

  (void)clone(threadfunc, (void*)31415, (void*)&i, stack); 
  (void)join(&stack);

  printf(1, "XV6_TEST_OUTPUT : i after threadfunc = %d\n", i);
  exit();
}
