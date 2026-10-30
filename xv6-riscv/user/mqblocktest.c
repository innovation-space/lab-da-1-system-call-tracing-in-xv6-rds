
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int q, i, pid;
  struct mq_message msg;
  char data[] = "hello";
  char blocked[] = "blocked";
  char fill[] = "F";

  printf("Blocking Message Queue Test Starting\n");

  q = mq_create();
  if(q < 0){
    printf("FAIL: queue creation\n");
    exit(1);
  }

  // Test blocking receive on an empty queue.
  pid = fork();
  if(pid < 0){
    printf("FAIL: fork\n");
    exit(1);
  }

  if(pid == 0){
    if(mq_receive(q, &msg) != 0 ||
       msg.length != 5 || msg.data[0] != 'h'){
      printf("FAIL: blocking receive\n");
      exit(1);
    }
    printf("PASS: blocked receiver received message\n");
    exit(0);
  }

  {
  volatile int delay;
  for(delay = 0; delay < 10000000; delay++) {
  }
}

  if(mq_try_send(q, data, 5, 1) != 0){
    printf("FAIL: sending to waiting receiver\n");
    exit(1);
  }

  wait(0);

  // Fill the queue completely.
  for(i = 0; i < 16; i++){
    if(mq_try_send(q, fill, 1, 1) != 0){
      printf("FAIL: filling queue\n");
      exit(1);
    }
  }

  printf("PASS: queue filled\n");

  // Test blocking send on a full queue.
  pid = fork();
  if(pid < 0){
    printf("FAIL: fork\n");
    exit(1);
  }

  if(pid == 0){
    if(mq_send(q, blocked, 7, 2) != 0){
      printf("FAIL: blocking send\n");
      exit(1);
    }
    printf("PASS: blocked sender sent after space became available\n");
    exit(0);
  }

 {
  volatile int delay;
  for(delay = 0; delay < 10000000; delay++) {
  }
}

  if(mq_try_receive(q, &msg) != 0){
    printf("FAIL: freeing queue space\n");
    exit(1);
  }

  wait(0);

  // Drain the queue.
  for(i = 0; i < 16; i++){
    if(mq_try_receive(q, &msg) != 0){
      printf("FAIL: draining queue\n");
      exit(1);
    }
  }

  if(mq_destroy(q) != 0){
    printf("FAIL: queue destruction\n");
    exit(1);
  }

  printf("PASS: queue destroyed\n");
  printf("ALL BLOCKING TESTS PASSED\n");
  exit(0);
}
