
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
int
msgqueue_send(int queue_id, int sender_pid,
              const char *data, int length, int priority)
{
  struct message_queue *q;
  struct message *m;
  struct proc *pr = myproc();
  int i;

  if(queue_id < 0 || queue_id >= MSGQUEUE_COUNT)
    return -1;
  if(data == 0 || length <= 0 || length > MSGQUEUE_MAX_SIZE)
    return -1;
  if(priority < MSG_PRIORITY_LOW || priority > MSG_PRIORITY_HIGH)
    return -1;

  q = &queues[queue_id];
  acquire(&queue_locks[queue_id]);

  while(q->count == MSGQUEUE_CAPACITY){
    if(killed(pr)){
      release(&queue_locks[queue_id]);
      return -1;
    }

    wakeup(q);
    sleep_prepare(q);
    release(&queue_locks[queue_id]);
    sleep();
    acquire(&queue_locks[queue_id]);
  }

  m = &q->messages[q->tail];
  m->sender_pid = sender_pid;
  m->length = length;
  m->priority = priority;

  for(i = 0; i < length; i++)
    m->data[i] = data[i];

  q->tail = (q->tail + 1) % MSGQUEUE_CAPACITY;
  q->count++;

  wakeup(q);
  release(&queue_locks[queue_id]);
  return 0;
}
int
msgqueue_receive(int queue_id, struct message *out)
{
  struct message_queue *q;
  struct message *m;
  struct proc *pr = myproc();
  int i;
  int best;
  int index;

  if(queue_id < 0 || queue_id >= MSGQUEUE_COUNT || out == 0)
    return -1;

  q = &queues[queue_id];
  acquire(&queue_locks[queue_id]);

  while(q->count == 0){
    if(killed(pr)){
      release(&queue_locks[queue_id]);
      return -1;
    }

    sleep_prepare(q);
    release(&queue_locks[queue_id]);
    sleep();
    acquire(&queue_locks[queue_id]);
  }

  best = 0;
  for(i = 1; i < q->count; i++){
    index = (q->head + i) % MSGQUEUE_CAPACITY;
    if(q->messages[index].priority >
       q->messages[(q->head + best) % MSGQUEUE_CAPACITY].priority)
      best = i;
  }

  index = (q->head + best) % MSGQUEUE_CAPACITY;
  m = &q->messages[index];

  out->sender_pid = m->sender_pid;
  out->length = m->length;
  out->priority = m->priority;

  for(i = 0; i < m->length; i++)
    out->data[i] = m->data[i];

  for(i = best; i < q->count - 1; i++){
    int current = (q->head + i) % MSGQUEUE_CAPACITY;
    int next = (q->head + i + 1) % MSGQUEUE_CAPACITY;
    q->messages[current] = q->messages[next];
  }

  q->tail = (q->tail - 1 + MSGQUEUE_CAPACITY) % MSGQUEUE_CAPACITY;
  q->count--;

  wakeup(q);
  release(&queue_locks[queue_id]);
  return 0;
}
