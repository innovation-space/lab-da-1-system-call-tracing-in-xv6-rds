#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int sample = 123;

  printf("Page-table mapping diagnostic test\n");
  printf("PID: %d\n", getpid());
  printf("Sample variable address: %p\n", (void *)&sample);

  vmmap();

  exit(0);
}