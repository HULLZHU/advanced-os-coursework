#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

int global;

void threadfunc(void *arg1, void *arg2) {
  *(int *)arg2 = (int)arg1;
  exit();
}
void threadfunc2(void *arg1, void *arg2) {
  global = 12345;
  exit();
}

int main(int argc, char *argv[])
{
  int i = 0;
  thread_create(threadfunc, (void*)314159, (void *)&i);
  thread_create(threadfunc2, 0, 0);
  thread_join();
  thread_join();

  printf(1, "XV6_TEST_OUTPUT : global = %d i = %d\n", global, i);

  exit();
}
