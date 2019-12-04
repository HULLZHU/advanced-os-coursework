#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "mman.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

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
    curproc->sz += length;

    struct mmap_region *new_mmap_region = kmalloc(sizeof(struct mmap_region));
    new_mmap_region->addr = (void*)PGROUNDDOWN((uint)address);
    new_mmap_region->offset = offset;    
    new_mmap_region->length = length;
    new_mmap_region->next = 0;
    new_mmap_region->prot = prot;

    if ((flags & MAP_ANONYMOUS) && fd > -1)
    {
        free_mmap_regions(curproc->mmap_hd);
        return (void*)-1;
    }
    
    if ((flags & MAP_FILE ) && fd > -1)
    {
        //if(curproc->ofile[fd])
        //{
            filedup(curproc->ofile[fd]);
            new_mmap_region->fd = fd; 
        //}
    }
    else 
    {
        new_mmap_region->fd = -1;
    }

    if(!curproc->mmap_hd) // first region, save as head
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
    
    return new_mmap_region->addr;
}

int munmap(void *addr, uint length)
{
    struct proc *curproc = myproc();
    int oldSize = curproc->sz;

    if (!curproc->mmap_hd)
    {
        return -1;
    }

    struct mmap_region *itr = curproc->mmap_hd;
    struct mmap_region *tmp; 
    int found = 0;

    if(itr->addr == addr && itr->length == length)
    {
        tmp = itr;
        curproc->mmap_hd = itr->next;
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
        if((curproc->sz = deallocuvm(curproc->pgdir, (uint)oldSize, (uint)oldSize - length)) == 0)
        {
            free_mmap_regions(curproc->mmap_hd);
            return -1;
        }

        if(curproc->ofile[tmp->fd])
        {
            fileclose(curproc->ofile[tmp->fd]);
        }
        
        kmfree(tmp);
        return 0;
    }

    free_mmap_regions(curproc->mmap_hd);

    return -1;
}

int msync(void *start_addr, int length)
{
    struct proc *curproc = myproc();

    if (!curproc->mmap_hd)
    {
        return -1;
    }

    struct mmap_region *itr = curproc->mmap_hd;
    struct mmap_region *tmp; 
    int found = 0;

    if(itr->addr == start_addr && itr->length == length)
    {
        tmp = itr;
        found = 1;
    }
    else
    {
        while(itr->next)
        {
            if(itr->next->addr == start_addr && itr->next->length == length)
            {
                tmp = itr->next;
                found = 1;
                break;
            }
            itr = itr->next;
        }        
    }

    if(found)
    {
        if(walkpgdir(curproc->pgdir, tmp->addr, 1) > 0)
        {
            //fileseek(curproc->ofile[tmp->fd], (uint)tmp->offset);
            filewrite(curproc->ofile[tmp->fd], (char*)tmp, length);
            return 0;
        }
        else 
        {
            return -1;
        }
        
    }

    return -1;
}