
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int q;
  struct mq_message msg;
  char low[] = "low";
  char high[] = "high";

  printf("Message Queue Test Starting\n");

  q = mq_create();
  if(q < 0){
    printf("FAIL: queue creation\n");
    exit(1);
  }
  printf("PASS: queue created, id=%d\n", q);

  if(mq_try_receive(q, &msg) != -1){
    printf("FAIL: empty queue should return -1\n");
    exit(1);
  }
  printf("PASS: empty non-blocking receive\n");

  if(mq_try_send(q, low, 3, 1) != 0){
    printf("FAIL: low-priority send\n");
    exit(1);
  }

  if(mq_try_send(q, high, 4, 3) != 0){
    printf("FAIL: high-priority send\n");
    exit(1);
  }
  printf("PASS: messages sent\n");

  if(mq_try_receive(q, &msg) != 0 ||
     msg.priority != 3 || msg.length != 4 ||
     msg.data[0] != 'h'){
    printf("FAIL: priority selection\n");
    exit(1);
  }
  printf("PASS: high-priority message received first\n");

  if(mq_try_receive(q, &msg) != 0 ||
     msg.priority != 1 || msg.length != 3 ||
     msg.data[0] != 'l'){
    printf("FAIL: low-priority message\n");
    exit(1);
  }
  printf("PASS: remaining message received\n");

  if(mq_destroy(q) != 0){
    printf("FAIL: queue destruction\n");
    exit(1);
  }
  printf("PASS: queue destroyed\n");

  printf("ALL BASIC TESTS PASSED\n");
  exit(0);
}
