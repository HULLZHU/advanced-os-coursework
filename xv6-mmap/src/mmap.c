#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

// Recursively free mmap regions
void free_mmap_regions(struct mmap_region *mmap_region)
{
    if(mmap_region && mmap_region->next)
        free_mmap_regions(mmap_region->next);  
    if (mmap_region)
        kmfree(mmap_region);
}

void *mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
    struct proc *curproc = myproc();
    int address = curproc->sz;

    cprintf("Before allocuvm\n");
    if((curproc->sz = allocuvm(curproc->pgdir, address, address + length)) == 0)
        return (void*)-1;
    cprintf("After allocuvm\n");

    switchuvm(curproc);
    cprintf("After switchuvm\n");

    struct mmap_region *new_mmap_region = kmalloc(sizeof(struct mmap_region));
    cprintf("After kmalloc\n");
    new_mmap_region->addr = (void*)PGROUNDDOWN((uint)address);
    new_mmap_region->offset = offset;
    new_mmap_region->fd = fd;
    new_mmap_region->length = length;
    new_mmap_region->next = 0;
    cprintf("After assigning mmap stuff\n");

    if(curproc->mmap_sz == 0) // first region, save as head
    {
        curproc->mmap_hd = new_mmap_region;
    }
    else 
    {
        struct mmap_region *itr = curproc->mmap_hd;
        while (itr->next)
        {
            itr = itr->next;
        }

        itr->next = new_mmap_region;
    }
    
    curproc->mmap_sz += 1;
    return new_mmap_region->addr;
}

int munmap(void *addr, uint length)
{
    struct proc *curproc = myproc();
    int address = curproc->sz;
    

    if (curproc->mmap_sz == 0)
    {
        return -1;
    }

    struct mmap_region *head = curproc->mmap_hd;
    struct mmap_region *itr = head;
    struct mmap_region *tmp; 
    int found = 0;

    if(head->addr == addr && head->length == length)
    {
        tmp = head;
        curproc->mmap_hd = head->next;
        found = 1;
    }
    else
    {
        while(itr->next)
        {
            if(itr->next->addr == addr && itr->next->length == length)
            {
                tmp = itr->next;
                itr->next = itr->next->next;
                found = 1;
                break;
            }
            itr = itr->next;
        }        
    }

    if(found)
    {
        if((curproc->sz = deallocuvm(curproc->pgdir, address, address - length)) == 0)
        {
            free_mmap_regions(curproc->mmap_hd);
            return -1;
        }
        
        switchuvm(curproc);
        kmfree(tmp);
        curproc->mmap_sz -= 1;
        return 0;
    }

    free_mmap_regions(curproc->mmap_hd);
    return -1;
}