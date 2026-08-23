#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  trace(1);
  printf("parent pid: %d\n", getpid());

  int pid = fork();
  if(pid == 0){
    printf("child pid: %d\n", getpid());
    exit(0);
  } else {
    wait(0);
    printf("done\n");
  }
  exit(0);
}