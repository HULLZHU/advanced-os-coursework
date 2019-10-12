
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

lock_t mylock;
int global;

void threadfunc(void *arg1, void *arg2) {
  for (int i = 0; i < 1000; i++) {
    lock_acquire(&mylock);
    global++;
    lock_release(&mylock);
  }
  exit();
}

void threadfunc2(void *arg1, void *arg2) {
  for (int i = 0; i < 1000; i++) {
    lock_acquire(&mylock);
    global--;
    lock_release(&mylock);
  }
  exit();
}

int main(int argc, char *argv[])
{
  lock_init(&mylock);
  thread_create(threadfunc, (void*)0x123, (void *)0x456);
  thread_create(threadfunc2, (void*)0xaaa, (void *)0xbbb);
  thread_join();
  thread_join();

  printf(1, "XV6_TEST_OUTPUT %d\n", global);
  exit();
}
