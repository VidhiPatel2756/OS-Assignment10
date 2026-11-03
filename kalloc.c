#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  // --- Task a: Tracking Counters ---
  uint64 total_allocated;
  uint64 total_freed;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  kmem.total_allocated = 0;
  kmem.total_freed = 0;
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Task a: Increment total_freed on page free
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch freed memory use.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.total_freed++;
  release(&kmem.lock);
}

// Task a: Increment total_allocated on page allocation
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    kmem.total_allocated++;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE);
  return (void*)r;
}

// Task b: Helper function to calculate memory statistics
uint64
get_memstats(void)
{
  uint64 free_pages = 0;
  struct run *r;

  acquire(&kmem.lock);

  // 1. Count current nodes in the freelist
  for(r = kmem.freelist; r; r = r->next)
    free_pages++;

  // 2. Calculate total system physical pages managed by kmem
  uint64 total_physical_pages = ((uint64)PHYSTOP - PGROUNDUP((uint64)end)) / PGSIZE;

  // 3. Currently allocated pages = Total pages - Free pages
  uint64 currently_allocated = total_physical_pages - free_pages;

  printk("\n=== Memory Statistics ===\n");
  printk("Total Allocated Pages : %d\n", (int)kmem.total_allocated);
  printk("Total Freed Pages     : %d\n", (int)kmem.total_freed);
  printk("Currently Alloc Pages : %d\n", (int)currently_allocated);
  printk("Available Free Pages  : %d (%d KB)\n", (int)free_pages, (int)((free_pages * PGSIZE) / 1024));
  printk("=========================\n\n");

  release(&kmem.lock);
  return 0;
}
