
#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "msgqueue.h"

static struct message_queue queues[MSGQUEUE_COUNT];
static struct spinlock queue_locks[MSGQUEUE_COUNT];

void
msgqueue_init(void)
{
  int i;

  for(i = 0; i < MSGQUEUE_COUNT; i++){
    initlock(&queue_locks[i], "msgqueue");
    queues[i].head = 0;
    queues[i].tail = 0;
    queues[i].count = 0;
    queues[i].id = i;
    queues[i].initialized = 1;
  }
}
