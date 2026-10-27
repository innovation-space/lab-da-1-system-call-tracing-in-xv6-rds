#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096
#define NPAGES 4

int
main(int argc, char *argv[])
{
  printf("=== vmtest: BEFORE allocation ===\n");
  vmmap();

  // Allocate NPAGES pages on the heap
  char *mem = sbrk(NPAGES * PGSIZE);
  if(mem == (char *)-1){
    printf("vmtest: sbrk failed\n");
    exit(1);
  }
  printf("\nvmtest: sbrk returned %p (%d pages)\n", mem, NPAGES);

  // Write a distinct pattern to each page
  for(int p = 0; p < NPAGES; p++){
    for(int i = 0; i < PGSIZE; i++)
      mem[p * PGSIZE + i] = (char)(p + 1);
  }

  // Read back and verify
  int errors = 0;
  for(int p = 0; p < NPAGES; p++){
    for(int i = 0; i < PGSIZE; i++){
      if(mem[p * PGSIZE + i] != (char)(p + 1)){
        printf("vmtest: mismatch at page %d offset %d\n", p, i);
        errors++;
        break;
      }
    }
  }

  printf("\n=== vmtest: AFTER allocation ===\n");
  vmmap();

  if(errors == 0)
    printf("\nvmtest: PASSED (read/write verified on %d pages)\n", NPAGES);
  else
    printf("\nvmtest: FAILED (%d bad pages)\n", errors);

  exit(errors ? 1 : 0);
}