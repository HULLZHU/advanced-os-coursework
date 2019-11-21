#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void *mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
    struct spinlock lk;
    initlock(&lk, "test lock");
    acquire(&lk);

    pde_t *pgdir = mycpu()->proc->pgdir;
    uint old_sz = mycpu()->proc->mmap_sz;
    struct mmap_region *head = mycpu()->proc->mmap_hd;

    if (!head)
    {
        head = kmalloc(sizeof(head));
        head->addr = (void*)allocuvm(pgdir, old_sz, length);
        head->offset = offset;
        head->fd = fd;
        head->length = length;
    }
    else 
    {
        struct mmap_region *new = kmalloc(sizeof(head));
        new->addr = (void*)allocuvm(pgdir, old_sz, length);
        new->offset = offset;
        new->fd = fd;
        new->length = length;

        struct mmap_region *itr = head;
        while (itr->next)
        {
            itr = itr->next;
        }

        itr->next = new;
    }
    
    mycpu()->proc->mmap_sz += length;
    release(&lk);
    
    return head->addr;
}

int munmap(void *addr, uint length)
{
    struct spinlock lk;
    initlock(&lk, "test lock");
    acquire(&lk);

    pde_t *pgdir = mycpu()->proc->pgdir;
    uint old_sz = mycpu()->proc->mmap_sz;
    struct mmap_region *head = mycpu()->proc->mmap_hd;
    struct mmap_region *itr = head;

    if (head->addr == addr && head->length == length)
    {
        mycpu()->proc->mmap_hd = head->next;
        deallocuvm(pgdir, old_sz, old_sz - length);
        kmfree(head);

        release(&lk);

        return 0;        
    }

    while(itr->next)
    {
        if (itr->next->addr == addr && itr->next->length == length)
        {
            struct mmap_region *tmp = itr->next;
            itr->next = itr->next->next;
            deallocuvm(pgdir, old_sz, old_sz - length);
            kmfree(tmp);
            release(&lk);

            return 0;
        }
    }

    release(&lk);

    return -1;
}