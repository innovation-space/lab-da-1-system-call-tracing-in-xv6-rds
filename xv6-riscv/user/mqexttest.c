
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int q, i;
  struct mq_message msg;
  char payload[2];

  printf("Extended Message Queue Test Starting\n");

  q = mq_create();
  if(q < 0){
    printf("FAIL: queue creation\n");
    exit(1);
  }

  printf("PASS: queue created\n");

  for(i = 0; i < 16; i++){
    payload[0] = 'A' + i;
    payload[1] = '\0';

    if(mq_try_send(q, payload, 1, 1) != 0){
      printf("FAIL: sending message %d\n", i);
      exit(1);
    }
  }

  printf("PASS: all 16 queue slots filled\n");

  payload[0] = 'X';
  if(mq_try_send(q, payload, 1, 1) != -1){
    printf("FAIL: full queue accepted another message\n");
    exit(1);
  }

  printf("PASS: 17th message rejected\n");

  if(mq_try_receive(q, &msg) != 0 ||
     msg.length != 1 || msg.data[0] != 'A'){
    printf("FAIL: first message received incorrectly\n");
    exit(1);
  }

  printf("PASS: first message received\n");

  payload[0] = 'Z';
  if(mq_try_send(q, payload, 1, 1) != 0){
    printf("FAIL: send after receiving\n");
    exit(1);
  }

  printf("PASS: sending succeeds after space becomes available\n");

  for(i = 1; i < 16; i++){
    if(mq_try_receive(q, &msg) != 0 ||
       msg.length != 1 || msg.data[0] != 'A' + i){
      printf("FAIL: FIFO order at message %d\n", i);
      exit(1);
    }
  }

  if(mq_try_receive(q, &msg) != 0 ||
     msg.length != 1 || msg.data[0] != 'Z'){
    printf("FAIL: final message\n");
    exit(1);
  }

  printf("PASS: remaining messages received in FIFO order\n");

  if(mq_destroy(q) != 0){
    printf("FAIL: queue destruction\n");
    exit(1);
  }

  printf("PASS: queue destroyed\n");
  printf("ALL EXTENDED TESTS PASSED\n");

  exit(0);
}
