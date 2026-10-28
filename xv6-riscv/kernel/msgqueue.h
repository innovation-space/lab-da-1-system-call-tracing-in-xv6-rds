
#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include "types.h"

#define MSGQUEUE_COUNT 4
#define MSGQUEUE_CAPACITY 16
#define MSGQUEUE_MAX_SIZE 64

#define MSG_PRIORITY_LOW    1
#define MSG_PRIORITY_MEDIUM 2
#define MSG_PRIORITY_HIGH   3

struct message {
  int sender_pid;
  int length;
  int priority;
  char data[MSGQUEUE_MAX_SIZE];
};

struct message_queue {
  struct message messages[MSGQUEUE_CAPACITY];
  int head;
  int tail;
  int count;
  int id;
  int initialized;
};

void msgqueue_init(void);

#endif
