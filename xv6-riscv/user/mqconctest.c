
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int q, pid, i, j;
  int seen[2][10] = {0};
  int counts[2] = {0};
  struct mq_message msg;
  char data[2];

  printf("Concurrent Message Queue Test Starting\n");

  q = mq_create();
  if(q < 0){
    printf("FAIL: queue creation\n");
    exit(1);
  }

  for(j = 0; j < 2; j++){
    pid = fork();

    if(pid < 0){
      printf("FAIL: fork\n");
      exit(1);
    }

    if(pid == 0){
      data[0] = 'A' + j;

      for(i = 0; i < 10; i++){
        data[1] = '0' + i;

        if(mq_send(q, data, 2, 1) != 0){
          printf("FAIL: child send\n");
          exit(1);
        }
      }

      exit(0);
    }
  }

  printf("PASS: two child processes created\n");

  for(i = 0; i < 20; i++){
    if(mq_receive(q, &msg) != 0){
      printf("FAIL: parent receive\n");
      exit(1);
    }

    if(msg.length != 2 ||
       msg.data[0] < 'A' || msg.data[0] > 'B' ||
       msg.data[1] < '0' || msg.data[1] > '9'){
      printf("FAIL: invalid message received\n");
      exit(1);
    }

    j = msg.data[0] - 'A';
    int index = msg.data[1] - '0';

    if(seen[j][index]){
      printf("FAIL: duplicate message\n");
      exit(1);
    }

    seen[j][index] = 1;
    counts[j]++;
  }

  wait(0);
  wait(0);

  if(counts[0] != 10 || counts[1] != 10){
    printf("FAIL: incorrect message counts\n");
    exit(1);
  }

  for(j = 0; j < 2; j++){
    for(i = 0; i < 10; i++){
      if(!seen[j][i]){
        printf("FAIL: missing message\n");
        exit(1);
      }
    }
  }

  printf("PASS: all 20 messages received without loss or duplication\n");

  if(mq_destroy(q) != 0){
    printf("FAIL: queue destruction\n");
    exit(1);
  }

  printf("PASS: queue destroyed\n");
  printf("ALL CONCURRENCY TESTS PASSED\n");

  exit(0);
}
