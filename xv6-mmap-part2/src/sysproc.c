#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_kmalloc(void)
{
  int bytes;
  argint(0, &bytes);
  return (int)kmalloc(bytes);
}

int sys_kmfree(void)
{
  void *addr;
  argint(0, (int*)&addr);
  kmfree(addr);
  return 0;
}

int sys_mmap(void)
{
  void *addr;
  int length;
  int prot;
  int flags;
  int fd;
  int offset;

  argint(0, (int*)&addr);
  argint(1, &length);
  argint(2, &prot);
  argint(3, &flags);
  argint(4, &fd);
  argint(5, &offset);

  return (int)mmap(addr, length, prot, flags, fd, offset);
}

int sys_munmap(void)
{
  void *addr;
  int length;

  argint(0, (int*)&addr);
  argint(1, &length);

  return munmap(addr, length);
}

int sys_msync(void)
{
  void *start_addr;
  int length;

  argint(0, (int*)&start_addr);
  argint(1, &length);

  return msync(start_addr, length);
}