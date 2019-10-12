#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

lock_t mylock;

void threadfunc(void *arg1, void *arg2) {
  printf(1, "XV6_TEST_OUTPUT : Inside threadfunc\n");

  lock_acquire(&mylock);
  printf(1, "XV6_TEST_OUTPUT : Lock Acquired inside threadfunc\n");

  *(int *)arg2 = (int)arg1;
  lock_release(&mylock);

  printf(1, "XV6_TEST_OUTPUT : Lock Released inside threadfunc\n");
  
  exit();
}

/*testing thread_create() and thread_join() system call with a simple thread function.*/
int main(int argc, char *argv[])
{
  lock_init(&mylock);

  int i = 0;
  printf(1, "XV6_TEST_OUTPUT : i before threadfunc = %x\n", i);

  (void)thread_create(threadfunc, (void*)31415, (void *)&i);
  (void)thread_join();
  printf(1, "XV6_TEST_OUTPUT : i after threadfunc = %d\n", i);

  exit();
}
