#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
int shared_counter = 0;
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
uint64
sys_trace(void)
{
  int n;

  argint(0, &n);

  myproc()->trace_mask = n;

  return 0;
}
uint64
sys_race_test(void)
{
    int n;
    int i;
    int temp;

    argint(0, &n);

    if(n == 0){
        shared_counter = 0;
        return 0;
    }

    for(i = 0; i < n; i++){
        temp = shared_counter;

        for(volatile int j = 0; j < 100000; j++);

        temp++;

        shared_counter = temp;
    }

    return shared_counter;
}
uint64
sys_get_counter(void)
{
    return shared_counter;
}
uint64
sys_vmmap(void)
{
  struct proc *p = myproc();
  uint64 va;
  pte_t *pte;

  printk("Page table mappings for PID %d\n", p->pid);
  printk("Virtual Address -> Physical Address | R W X\n");

  for (va = 0; va < p->sz; va += PGSIZE) {
    pte = walk(p->pagetable, va, 0);

    if (pte == 0 || (*pte & PTE_V) == 0)
      continue;

    if ((*pte & PTE_U) == 0)
      continue;

    printk("%p -> %p | %c %c %c\n",
           (void *)va,
           (void *)PTE2PA(*pte),
           (*pte & PTE_R) ? 'R' : '-',
           (*pte & PTE_W) ? 'W' : '-',
           (*pte & PTE_X) ? 'X' : '-');
  }

  return 0;
}