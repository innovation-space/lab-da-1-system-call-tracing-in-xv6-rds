
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int q;
  struct mq_message msg;
  char a[] = "A";
  char b[] = "B";
  char c[] = "C";

  printf("Priority and Validation Test Starting\n");

  q = mq_create();
  if(q < 0){
    printf("FAIL: queue creation\n");
    exit(1);
  }

  if(mq_try_send(q, a, 1, 1) != 0 ||
     mq_try_send(q, b, 1, 3) != 0 ||
     mq_try_send(q, c, 1, 3) != 0){
    printf("FAIL: sending messages\n");
    exit(1);
  }

  if(mq_try_receive(q, &msg) != 0 ||
     msg.data[0] != 'B' || msg.priority != 3){
    printf("FAIL: first high-priority message\n");
    exit(1);
  }

  if(mq_try_receive(q, &msg) != 0 ||
     msg.data[0] != 'C' || msg.priority != 3){
    printf("FAIL: equal-priority FIFO\n");
    exit(1);
  }

  if(mq_try_receive(q, &msg) != 0 ||
     msg.data[0] != 'A' || msg.priority != 1){
    printf("FAIL: low-priority message\n");
    exit(1);
  }

  printf("PASS: priority ordering and equal-priority FIFO\n");

  if(mq_try_send(q, a, 0, 1) != -1 ||
     mq_try_send(q, a, 65, 1) != -1){
    printf("FAIL: invalid message length accepted\n");
    exit(1);
  }

  if(mq_try_send(q, a, 1, 0) != -1 ||
     mq_try_send(q, a, 1, 4) != -1){
    printf("FAIL: invalid priority accepted\n");
    exit(1);
  }

  printf("PASS: invalid lengths and priorities rejected\n");

  if(mq_destroy(q) != 0){
    printf("FAIL: queue destruction\n");
    exit(1);
  }

  printf("PASS: queue destroyed\n");
  printf("ALL PRIORITY TESTS PASSED\n");

  exit(0);
}
