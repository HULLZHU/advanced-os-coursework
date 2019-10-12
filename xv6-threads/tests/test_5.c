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

/*testing whether PIDs returned by thread_create() and thread_join() are correct*/
int main(int argc, char *argv[])
{
  int pid1 = thread_create(threadfunc, (void*)0x123, (void *)0x456);
  int pid2 = thread_join();

  if(pid1 == pid2)
  {
    printf(1, "XV6_TEST_OUTPUT : Correct PIDs returned by thread_create and thread_join\n");
  }
  else
  {
    printf(1, "XV6_TEST_OUTPUT :  PIDs returned by thread_create and thread_join do not match..!\n");
  }
  

  exit();
}
